#include "core/inst_handlers/v/RvvlsInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/VectorState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvlsInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // TODO: decide how to pass VLEN as argument.
        using VLEN = std::array<GetUintType<8>, 8>;

        inst_handlers.emplace(
            "vle8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<8>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vle8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<16>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vle16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vle32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<32>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vle32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<8>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<16>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vse32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<32>, Mop::Unit>,
                RvvlsInsts>(nullptr, "vse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vlse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<8>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vlse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<16>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vlse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<32>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vlse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsse8.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<8>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vsse8.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<16>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vsse16.v", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            atlas::Action::createAction<
                &RvvlsInsts::vlseComputeAddressHandler_<XLEN, VLEN, GetUintType<32>, Mop::Strided>,
                RvvlsInsts>(nullptr, "vsse32.v", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<8>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vloxei8.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<16>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vloxei16.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<32>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vloxei32.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<8>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vsoxei8.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<16>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vsoxei16.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseComputeAddressHandler_<
                                            XLEN, VLEN, GetUintType<32>, Mop::IdxOrdered>,
                                        RvvlsInsts>(nullptr, "vsoxei32.v",
                                                    ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvvlsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // TODO: decide how to pass VLEN as argument.
        using VLEN = std::array<GetUintType<8>, 8>;

        inst_handlers.emplace(
            "vle8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<8>, true>,
                                        RvvlsInsts>(nullptr, "vle8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<16>, true>,
                                        RvvlsInsts>(nullptr, "vle16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vle32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<32>, true>,
                                        RvvlsInsts>(nullptr, "vle32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vse8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<8>, false>,
                                        RvvlsInsts>(nullptr, "vse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<16>, false>,
                                        RvvlsInsts>(nullptr, "vse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<32>, false>,
                                        RvvlsInsts>(nullptr, "vse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vlse8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<8>, true>,
                                        RvvlsInsts>(nullptr, "vlse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<16>, true>,
                                        RvvlsInsts>(nullptr, "vlse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vlse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<32>, true>,
                                        RvvlsInsts>(nullptr, "vlse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsse8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<8>, false>,
                                        RvvlsInsts>(nullptr, "vsse8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<16>, false>,
                                        RvvlsInsts>(nullptr, "vsse16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsse32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseHandler_<VLEN, GetUintType<32>, false>,
                                        RvvlsInsts>(nullptr, "vsse32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vloxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, true>, RvvlsInsts>(
                nullptr, "vloxei8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, true>, RvvlsInsts>(
                nullptr, "vloxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vloxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, true>, RvvlsInsts>(
                nullptr, "vloxei32.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsoxei8.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, false>, RvvlsInsts>(
                nullptr, "vsoxei8.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei16.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, false>, RvvlsInsts>(
                nullptr, "vsoxei16.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsoxei32.v",
            atlas::Action::createAction<&RvvlsInsts::vlseIdxHandler_<VLEN, false>, RvvlsInsts>(
                nullptr, "vsoxei32.v", ActionTags::EXECUTE_TAG));
    }

    template void RvvlsInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvlsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    inline uint8_t decodeEEW(uint64_t width)
    {
        switch (width)
        {
            case 0b000:
                return 8;
            case 0b101:
                return 16;
            case 0b110:
                return 32;
            case 0b111:
                return 64;
            default:
                sparta_assert(false, "Unsupported EEW Width Encoding.");
                break;
        }
        return 0;
    }

    template <typename XLEN, typename VLEN, typename EEW, RvvlsInsts::Mop mop>
    Action::ItrType RvvlsInsts::vlseComputeAddressHandler_(atlas::AtlasState* state,
                                                           Action::ItrType action_it)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

        const AtlasInstPtr inst = state->getCurrentInst();
        VectorState* vector_state = state->getVectorState();
        const uint8_t vlenb = sizeof(VLEN);
        const uint8_t eewb = sizeof(EEW);
        const uint8_t sewb = vector_state->getSEW() / 8;
        const uint8_t vl = vector_state->getVL();
        const uint8_t vstart = vector_state->getVSTART();
        const VLEN mask = READ_VEC_REG<VLEN>(state, 0);
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        uint8_t index = vstart;
        Addr stride;

        if constexpr (mop == Mop::Unit)
        {
            stride = eewb;
        }
        else if constexpr (mop == Mop::Strided)
        {
            stride = READ_INT_REG<XLEN>(state, inst->getRs2());
        }
        if constexpr (mop == Mop::Unit || mop == Mop::Strided)
        {
            for (; index < vl; ++index)
            {
                if (inst->getVM()
                    || (reinterpret_cast<const uint8_t*>(&mask)[index / 8] >> (index % 8)) & 1)
                {
                    inst->getTranslationState()->makeRequest(rs1_val + index * stride, eewb);
                }
            }
        }
        else // Indexed
        {
            VLEN vs2_val = READ_VEC_REG<VLEN>(state, inst->getRs2());
            for (; index < vl; ++index)
            {
                if (index % (vlenb / eewb) == 0)
                {
                    vs2_val = READ_VEC_REG<VLEN>(state, inst->getRs2() + index / (vlenb / eewb));
                }
                if (inst->getVM()
                    || (reinterpret_cast<const uint8_t*>(&mask)[index / 8] >> (index % 8)) & 1)
                {
                    memcpy(&stride,
                           reinterpret_cast<uint8_t*>(&vs2_val) + index % (vlenb / eewb) * eewb,
                           eewb);
                    inst->getTranslationState()->makeRequest(rs1_val + stride, sewb);
                }
            }
        }
        return ++action_it;
    }

    template <typename VLEN, typename EW, bool load>
    Action::ItrType RvvlsInsts::vlseHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        VectorState* vector_state = state->getVectorState();
        constexpr uint8_t vlenb = sizeof(VLEN);
        constexpr uint8_t ewb = sizeof(EW);
        const uint8_t vl = vector_state->getVL();
        const uint8_t vstart = vector_state->getVSTART();
        const uint32_t rd = inst->getRd();
        const VLEN mask = READ_VEC_REG<VLEN>(state, 0);
        const uint8_t stop = std::min<uint8_t>(vl, (vstart / (vlenb / ewb) + 1) * (vlenb / ewb));
        uint8_t index = vstart;
        VLEN rd_val = READ_VEC_REG<VLEN>(state, rd + vstart / (vlenb / ewb));
        for (; index < stop; ++index)
        {
            if (inst->getVM()
                || (reinterpret_cast<const uint8_t*>(&mask)[index / 8] >> (index % 8)) & 1)
            {
                if constexpr (load)
                {
                    EW result =
                        state->readMemory<EW>(inst->getTranslationState()->getResult().getPAddr());
                    inst->getTranslationState()->popResult();
                    memcpy(reinterpret_cast<uint8_t*>(&rd_val) + (index % (vlenb / ewb)) * ewb,
                           &result, ewb);
                }
                else
                {
                    EW result;
                    memcpy(&result,
                           reinterpret_cast<uint8_t*>(&rd_val) + (index % (vlenb / ewb)) * ewb,
                           ewb);
                    state->writeMemory<EW>(inst->getTranslationState()->getResult().getPAddr(),
                                           result);
                    inst->getTranslationState()->popResult();
                }
            }
        }
        if constexpr (load)
        {
            WRITE_VEC_REG<VLEN>(state, rd + vstart / (vlenb / ewb), rd_val);
        }
        vector_state->setVSTART(index);
        if (index != vl)
        {
            return action_it;
        }
        return ++action_it;
    }

    template <typename VLEN, bool load>
    Action::ItrType RvvlsInsts::vlseIdxHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        VectorState* vector_state = state->getVectorState();
        switch (vector_state->getSEW())
        {
            case 8:
                return vlseHandler_<VLEN, uint8_t, load>(state, action_it);
                break;
            case 16:
                return vlseHandler_<VLEN, uint16_t, load>(state, action_it);
                break;
            case 32:
                return vlseHandler_<VLEN, uint32_t, load>(state, action_it);
                break;
            case 64:
                return vlseHandler_<VLEN, uint64_t, load>(state, action_it);
                break;
            default:
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace atlas
