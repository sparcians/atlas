#include "core/Fetch.hpp"
#include "core/PegasusAllocatorWrapper.hpp"
#include "core/PegasusState.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "include/ActionTags.hpp"

#include "sparta/events/StartupEvent.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    Fetch::Fetch(sparta::TreeNode* fetch_node, const FetchParameters*) : sparta::Unit(fetch_node)
    {
        Action fetch_action =
            pegasus::Action::createAction<&Fetch::fetch_>(this, "fetch", ActionTags::FETCH_TAG);
        fetch_action_group_.addAction(fetch_action);

        Action decode_action =
            pegasus::Action::createAction<&Fetch::decode_>(this, "decode", ActionTags::DECODE_TAG);
        decode_action_group_.addAction(decode_action);

        sparta::StartupEvent(fetch_node, CREATE_SPARTA_HANDLER(Fetch, advanceSim_));
    }

    void Fetch::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        state_ = core_tn->getResourceAs<PegasusState>();

        // Connect Fetch, Translate and Execute
        Translate* translate_unit = core_tn->getChild("translate")->getResourceAs<Translate*>();
        Execute* execute_unit = core_tn->getChild("execute")->getResourceAs<Execute*>();

        ActionGroup* inst_translate_action_group = translate_unit->getInstTranslateActionGroup();
        ActionGroup* execute_action_group = execute_unit->getActionGroup();

        fetch_action_group_.setNextActionGroup(inst_translate_action_group);
        inst_translate_action_group->setNextActionGroup(&decode_action_group_);
        decode_action_group_.setNextActionGroup(execute_action_group);
        execute_action_group->setNextActionGroup(&fetch_action_group_);
    }

    Action::ItrType Fetch::fetch_(PegasusState* state, Action::ItrType action_it)
    {
        ILOG("Fetching PC 0x" << std::hex << state->getPc());

        // Reset the sim state
        PegasusState::SimState* sim_state = state->getSimState();
        sim_state->reset();

        PegasusTranslationState* translation_state = state->getFetchTranslationState();
        translation_state->reset();
        translation_state->makeRequest(state->getPc(), sizeof(Opcode));

        // Keep going
        return ++action_it;
    }

    Action::ItrType Fetch::decode_(PegasusState* state, Action::ItrType action_it)
    {
        // Get translation result
        const PegasusTranslationState::TranslationResult result =
            state->getFetchTranslationState()->getResult();
        state->getFetchTranslationState()->popResult();

        // When compressed instructions are enabled, it is possible for a full sized instruction (32
        // bits) to cross a 4K page boundary meaning that first 16 bits of the instruction are on a
        // different page than the second 16 bits. Fetch will always request translation for a 32
        // bit memory access but Translate may need to be performed twice if it detects that the
        // access crosses a 4K page boundary. Since it is possible for the first 16 bits translated
        // and read from memory to result in a valid compressed instruction, Decode must attempt to
        // decode the first 16 bits before asking Translate to translate the second 16 bit access.
        // This ensures that the correct translation faults are triggered.
        //
        // There are several possible scenarios that result in Decode generating a valid
        // instruction:
        //
        // 1. The 32 bit fetch access does not cross a page boundary. The 32 bits read from memory
        // are
        //    decoded as a non-compressed instruction.
        //
        // 2. The 32 bit fetch access does not cross a page boundary. The 32 bits read from memory
        // are
        //    decoded as a compressed instruction. The extra 16 bits are discarded.
        //
        // 3. The 32 bit fetch access crosses a page boundary. The first 16 bits read are a
        // compressed
        //    instruction. The second 16 bits are never translated or read from memory.
        //
        // 4. The 32 bit fetch access crosses a page boundary. The first 16 bits read are not a
        // valid
        //    compressed instruction. The second 16 bits are translated and read from memory. The
        //    combined 32 bits are decoded as a non-compressed instruction.
        const bool page_crossing_access = result.getSize() == 2;

        // Read opcode from memory
        Opcode & opcode = state->getSimState()->current_opcode;
        OpcodeSize opcode_size = 4;
        if (SPARTA_EXPECT_TRUE(!page_crossing_access))
        {
            // TBD: Opcode opcode = result.readMemory<Opcode>(result.physical_addr);
            opcode = state->readMemory<uint32_t>(result.getPAddr());

            // Compression detection
            if ((opcode & 0x3) != 0x3)
            {
                opcode = opcode & 0xFFFF;
                opcode_size = 2;
            }
        }
        else
        {
            if (opcode == 0)
            {
                // Load the first 2B, could be a valid 2B compressed inst
                opcode = state->readMemory<uint16_t>(result.getPAddr());
                opcode_size = 2;

                if ((opcode & 0x3) == 0x3)
                {
                    // Go back to inst translate
                    throw ActionException(fetch_action_group_.getNextActionGroup());
                }
            }
            else
            {
                // Load the second 2B of a possible 4B inst
                opcode |= state->readMemory<uint16_t>(result.getPAddr()) << 16;
            }
        }

        ++(state->getSimState()->current_uid);

        // Decode instruction with Mavis
        PegasusInstPtr inst = nullptr;
        try
        {
            inst = state->getMavis()->makeInst(opcode, state);
            assert(state->getCurrentInst() == nullptr);
            state->setCurrentInst(inst);
            // Set next PC, can be overidden by a branch/jump instruction or an exception
            state->setNextPc(state->getPc() + opcode_size);
        }
        catch (const mavis::BaseException & e)
        {
            THROW_ILLEGAL_INST;
        }

        // If we only fetched 2B and found a valid compressed inst, then cancel the translation
        // request for the second 2B
        if (page_crossing_access && (opcode_size == 2))
        {
            state->getFetchTranslationState()->popRequest();
        }

        if (SPARTA_EXPECT_FALSE(inst->hasCsr()))
        {
            const uint32_t csr =
                inst->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
            if (state->getCsrRegister(csr) == nullptr)
            {
                THROW_ILLEGAL_INST;
            }

            // TODO: This is probably not the best place for this check...
            if (csr == SATP)
            {
                const uint32_t tvm_val = READ_CSR_FIELD<RV64>(state, MSTATUS, "tvm");
                if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tvm_val)
                {
                    THROW_ILLEGAL_INST;
                }
            }
        }

        return ++action_it;
    }

    void Fetch::advanceSim_()
    {
        // Run
        ActionGroup* next_action_group = &fetch_action_group_;
        while (next_action_group)
        {
            next_action_group = next_action_group->execute(state_);
        }
        // End of sim
    }
} // namespace pegasus
