#include "core/inst_handlers/v/RvvLoadStoreInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void
    RvvLoadStoreInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vle32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                RvvLoadStoreInsts>(nullptr, "vse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vlse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vlse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsse8.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            pegasus::Action::createAction<
                &RvvLoadStoreInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvLoadStoreInsts>(nullptr, "vsse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vloxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 8, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei8.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 16, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei16.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxComputeAddressHandler_<
                                              XLEN, 32, AddressingMode::IDX_ORDERED>,
                                          RvvLoadStoreInsts>(nullptr, "vsoxei32.v",
                                                             ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvvLoadStoreInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, true>,
                                                    RvvLoadStoreInsts>(nullptr, "vle8.v",
                                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vle16.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vle32.v",
                                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, false>,
                                                    RvvLoadStoreInsts>(nullptr, "vse8.v",
                                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vse16.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vse32.v",
                                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vlse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, true>,
                                                     RvvLoadStoreInsts>(nullptr, "vlse8.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vlse16.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, true>,
                                                      RvvLoadStoreInsts>(nullptr, "vlse32.v",
                                                                         ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsse8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<8, false>,
                                                     RvvLoadStoreInsts>(nullptr, "vsse8.v",
                                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<16, false>,
                                                      RvvLoadStoreInsts>(nullptr, "vsse16.v",
                                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseHandler_<32, false>,
                                                      RvvLoadStoreInsts>(nullptr, "vsse32.v",
                                                                         ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vloxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                       RvvLoadStoreInsts>(nullptr, "vloxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vloxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<true>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vloxei32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsoxei8.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                       RvvLoadStoreInsts>(nullptr, "vsoxei8.v",
                                                                          ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei16.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsoxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei32.v", pegasus::Action::createAction<&RvvLoadStoreInsts::vlseIdxHandler_<false>,
                                                        RvvLoadStoreInsts>(
                              nullptr, "vsoxei32.v", ActionTags::EXECUTE_TAG));
    }

    template void
    RvvLoadStoreInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void
    RvvLoadStoreInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvvLoadStoreInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvLoadStoreInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, RvvLoadStoreInsts::AddressingMode addrMode>
    Action::ItrType RvvLoadStoreInsts::vlseComputeAddressHandler_(pegasus::PegasusState* state,
                                                                  Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const PegasusInstPtr inst = state->getCurrentInst();
        VectorConfig* config = state->getVectorConfig();
        const size_t eewb = elemWidth / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        Addr stride;
        if constexpr (addrMode == AddressingMode::UNIT)
        {
            stride = eewb;
        }
        else if constexpr (addrMode == AddressingMode::STRIDED)
        {
            stride = READ_INT_REG<XLEN>(state, inst->getRs2());
        }
        if (inst->getVM())
        {
            for (size_t i = config->getVSTART(); i < config->getVL(); ++i)
            {
                inst->getTranslationState()->makeRequest(rs1_val + i * stride, eewb);
            }
        }
        else // masked
        {
            const MaskElements mask_elems{state, config, pegasus::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                inst->getTranslationState()->makeRequest(rs1_val + mask_iter.getIndex() * stride,
                                                         eewb);
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvLoadStoreInsts::AddressingMode addrMode>
    Action::ItrType RvvLoadStoreInsts::vlseIdxComputeAddressHandler_(pegasus::PegasusState* state,
                                                                     Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const PegasusInstPtr inst = state->getCurrentInst();
        VectorConfig* vector_config = state->getVectorConfig();
        const size_t sewb = vector_config->getSEW() / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};

        if (inst->getVM())
        {
            for (auto stride : elems_vs2)
            {
                inst->getTranslationState()->makeRequest(rs1_val + stride.getVal(), sewb);
            }
        }
        else
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                inst->getTranslationState()->makeRequest(
                    rs1_val + elems_vs2.getElement(mask_iter.getIndex()).getVal(), sewb);
            }
        }

        return ++action_it;
    }

    template <size_t elemWidth, bool load>
    Action::ItrType RvvLoadStoreInsts::vlseHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        const PegasusInstPtr inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems{state, state->getVectorConfig(),
                                                  load ? inst->getRd() : inst->getRs3()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                if constexpr (load)
                {
                    UintType<elemWidth> value = state->readMemory<UintType<elemWidth>>(
                        inst->getTranslationState()->getResult().getPAddr());
                    inst->getTranslationState()->popResult();
                    elems.getElement(iter.getIndex()).setVal(value);
                }
                else
                {
                    UintType<elemWidth> value = elems.getElement(iter.getIndex()).getVal();
                    state->writeMemory<UintType<elemWidth>>(
                        inst->getTranslationState()->getResult().getPAddr(), value);
                    inst->getTranslationState()->popResult();
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems.begin(), elems.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <bool load>
    Action::ItrType RvvLoadStoreInsts::vlseIdxHandler_(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vlseHandler_<8, load>(state, action_it);
            case 16:
                return vlseHandler_<16, load>(state, action_it);
            case 32:
                return vlseHandler_<32, load>(state, action_it);
            case 64:
                return vlseHandler_<64, load>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace pegasus
