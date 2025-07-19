#pragma once

#include "sim/PegasusSim.hpp"
#include "core/PegasusState.hpp"
#include "include/PegasusTypes.hpp"

class PegasusInstructionTester
{

  public:
    PegasusInstructionTester()
    {
        // Create the simulator
        const uint64_t ilimit = 0;
        pegasus_sim_.reset(new pegasus::PegasusSim(&scheduler_, {}, {}, ilimit));

        pegasus_sim_->buildTree();
        pegasus_sim_->configureTree();
        pegasus_sim_->finalizeTree();

        state_ = pegasus_sim_->getPegasusState();
        fetch_unit_ = state_->getFetchUnit();
        execute_unit_ = state_->getExecuteUnit();
    }

    virtual ~PegasusInstructionTester() = default;

    pegasus::PegasusState* getPegasusState() const { return state_; }

    void injectInstruction(const uint64_t pc, const uint32_t opcode)
    {
        state_->writeMemory(pc, opcode);
        state_->setPc(pc);

        // Fetch and Execute instruction
        pegasus::ActionGroup* next_action_group = fetch_unit_->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state_);
        } while (next_action_group
                 && (next_action_group->hasTag(pegasus::ActionTags::FETCH_TAG) == false));
    }

    void executeInstruction(pegasus::PegasusInst::PtrType instPtr)
    {
        state_->setCurrentInst(instPtr);

        // Execute instruction
        pegasus::ActionGroup* next_action_group = execute_unit_->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state_);
        } while (next_action_group
                 && (next_action_group->hasTag(pegasus::ActionTags::FETCH_TAG) == false));
    }

    template <typename XLEN> XLEN setVtypeCSR(XLEN vlmul, XLEN vsew, XLEN vta, XLEN vma, XLEN vill)
    {
        WRITE_CSR_FIELD<XLEN>(state_, pegasus::VTYPE, "vlmul", vlmul);
        WRITE_CSR_FIELD<XLEN>(state_, pegasus::VTYPE, "vsew", vsew);
        WRITE_CSR_FIELD<XLEN>(state_, pegasus::VTYPE, "vta", vta);
        WRITE_CSR_FIELD<XLEN>(state_, pegasus::VTYPE, "vma", vma);
        WRITE_CSR_FIELD<XLEN>(state_, pegasus::VTYPE, "vill", vill);
        return READ_CSR_REG<XLEN>(state_, pegasus::VTYPE);
    }

    template <typename... Args> pegasus::PegasusInst::PtrType makePegasusInst(const Args &... args)
    {
        pegasus::PegasusState* state = getPegasusState();
        mavis::ExtractorDirectInfo ex_info(args...);
        pegasus::PegasusInst::PtrType instPtr = state->getMavis()->makeInstDirectly(ex_info, state);
        std::cout << instPtr->dasmString() << std::endl;
        return instPtr;
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<pegasus::PegasusSim> pegasus_sim_;

    pegasus::PegasusState* state_ = nullptr;
    pegasus::Fetch* fetch_unit_ = nullptr;
    pegasus::Execute* execute_unit_ = nullptr;
};
