#include "core/inst_handlers/v/RvvlsInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvlsInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>, RvvlsInsts>(
                nullptr, "vle8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle16.v", atlas::Action::createAction<
                           &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                           RvvlsInsts>(nullptr, "vle16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle32.v", atlas::Action::createAction<
                           &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                           RvvlsInsts>(nullptr, "vle32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::UNIT>, RvvlsInsts>(
                nullptr, "vse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse16.v", atlas::Action::createAction<
                           &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::UNIT>,
                           RvvlsInsts>(nullptr, "vse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse32.v", atlas::Action::createAction<
                           &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::UNIT>,
                           RvvlsInsts>(nullptr, "vse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vlse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vlse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vlse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vlse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 8, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vsse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 16, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vsse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, 32, AddressingMode::STRIDED>,
                RvvlsInsts>(nullptr, "vsse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 8, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vloxei8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 16, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vloxei16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 32, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vloxei32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 8, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vsoxei8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 16, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vsoxei16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseIdxComputeAddressHandler_<XLEN, 32, AddressingMode::IDX_ORDERED>,
                RvvlsInsts>(nullptr, "vsoxei32.v", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvvlsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vle8.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<8, true>, RvvlsInsts>(
                          nullptr, "vle8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle16.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<16, true>, RvvlsInsts>(
                           nullptr, "vle16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle32.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<32, true>, RvvlsInsts>(
                           nullptr, "vle32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vse8.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<8, false>, RvvlsInsts>(
                          nullptr, "vse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<16, false>, RvvlsInsts>(
                nullptr, "vse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<32, false>, RvvlsInsts>(
                nullptr, "vse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vlse8.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<8, true>, RvvlsInsts>(
                           nullptr, "vlse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<16, true>, RvvlsInsts>(
                nullptr, "vlse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<32, true>, RvvlsInsts>(
                nullptr, "vlse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsse8.v", atlas::Action::createAction<&RvvlsInsts::vlseHandler_<8, false>, RvvlsInsts>(
                           nullptr, "vsse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<16, false>, RvvlsInsts>(
                nullptr, "vsse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<32, false>, RvvlsInsts>(
                nullptr, "vsse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<true>, RvvlsInsts>(
                nullptr, "vloxei8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<true>, RvvlsInsts>(
                nullptr, "vloxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<true>, RvvlsInsts>(
                nullptr, "vloxei32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<false>, RvvlsInsts>(
                nullptr, "vsoxei8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<false>, RvvlsInsts>(
                nullptr, "vsoxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<false>, RvvlsInsts>(
                nullptr, "vsoxei32.v", ActionTags::EXECUTE_TAG));
    }

    template void RvvlsInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t ElemWidth, RvvlsInsts::AddressingMode Mode>
    Action::ItrType RvvlsInsts::vlseComputeAddressHandler_(atlas::AtlasState* state,
                                                           Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const AtlasInstPtr inst = state->getCurrentInst();
        VectorConfig* config = state->getVectorConfig();
        const size_t eewb = ElemWidth / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        Addr stride;
        if constexpr (Mode == AddressingMode::UNIT)
        {
            stride = eewb;
        }
        else if constexpr (Mode == AddressingMode::STRIDED)
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
            const MaskElements mask_elems{state, config, atlas::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                inst->getTranslationState()->makeRequest(rs1_val + mask_iter.getIndex() * stride,
                                                         eewb);
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t ElemWidth, RvvlsInsts::AddressingMode Mode>
    Action::ItrType RvvlsInsts::vlseIdxComputeAddressHandler_(atlas::AtlasState* state,
                                                              Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const AtlasInstPtr inst = state->getCurrentInst();
        VectorConfig* vector_config = state->getVectorConfig();
        const size_t sewb = vector_config->getSEW() / 8;
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        Elements<Element<ElemWidth>, false> elems_vs2{state, state->getVectorConfig(),
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
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            for (auto mask_iter = mask_elems.maskBitIterBegin();
                 mask_iter != mask_elems.maskBitIterEnd(); ++mask_iter)
            {
                inst->getTranslationState()->makeRequest(
                    rs1_val + elems_vs2.getElement(mask_iter.getIndex()).getVal(), sewb);
            }
        }

        return ++action_it;
    }

    template <size_t ElemWidth, bool load>
    Action::ItrType RvvlsInsts::vlseHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        Elements<Element<ElemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                if constexpr (load)
                {
                    UintType<ElemWidth> value = state->readMemory<UintType<ElemWidth>>(
                        inst->getTranslationState()->getResult().getPAddr());
                    inst->getTranslationState()->popResult();
                    elems_vd.getElement(iter.getIndex()).setVal(value);
                }
                else
                {
                    UintType<ElemWidth> value = elems_vd.getElement(iter.getIndex()).getVal();
                    state->writeMemory<UintType<ElemWidth>>(
                        inst->getTranslationState()->getResult().getPAddr(), value);
                    inst->getTranslationState()->popResult();
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <bool load>
    Action::ItrType RvvlsInsts::vlseIdxHandler_(atlas::AtlasState* state, Action::ItrType action_it)
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
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace atlas
