#include "core/inst_handlers/v/RvviaInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/VectorState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvviaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // TODO: decide how to pass VLEN as argument.
        using VLEN = std::array<GetUintType<8>, 8>;

        inst_handlers.emplace(
            "vadd.vv",
            atlas::Action::createAction<&RvviaInsts::viavv_handler_<VLEN, std::plus>, RvviaInsts>(
                nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            atlas::Action::createAction<&RvviaInsts::viavx_handler_<XLEN, VLEN, std::plus>,
                                        RvviaInsts>(nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            atlas::Action::createAction<&RvviaInsts::viavi_handler_<VLEN, std::plus>, RvviaInsts>(
                nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));
    }

    template void RvviaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvviaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename VLEN, typename SEW, typename OP>
    Action::ItrType viavv_helper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        VectorState* vector_state = state->getVectorState();
        uint8_t const vlenb = sizeof(VLEN);
        uint8_t const sewb = sizeof(SEW);
        uint8_t const vl = vector_state->getVL();
        uint8_t const vstart = vector_state->getVSTART();
        uint32_t const vs1 = inst->getRs1();
        uint32_t const vs2 = inst->getRs2();
        uint32_t const vd = inst->getRd();
        if (vstart >= vl)
        {
            return ++action_it;
        }
        VLEN const vs1_val = READ_VEC_REG<VLEN>(state, vs1 + vstart / (vlenb / sewb));
        VLEN const vs2_val = READ_VEC_REG<VLEN>(state, vs2 + vstart / (vlenb / sewb));
        VLEN vd_val = READ_VEC_REG<VLEN>(state, vd + vstart / (vlenb / sewb));
        uint8_t const stop = std::min<uint8_t>(vl, (vstart / (vlenb / sewb) + 1) * (vlenb / sewb));
        uint8_t index = vstart;
        for (; index < stop; ++index)
        {
            SEW result =
                OP()(*(reinterpret_cast<const SEW*>(reinterpret_cast<const uint8_t*>(&vs1_val)
                                                    + (index % (vlenb / sewb)) * sewb)),
                     *(reinterpret_cast<const SEW*>(reinterpret_cast<const uint8_t*>(&vs2_val)
                                                    + (index % (vlenb / sewb)) * sewb)));
            memcpy(reinterpret_cast<uint8_t*>(&vd_val) + (index % (vlenb / sewb)) * sewb, &result,
                   sewb);
        }
        WRITE_VEC_REG<VLEN>(state, vd + vstart / (vlenb / sewb), vd_val);
        vector_state->setVSTART(index);
        if (index != vl)
        {
            return action_it;
        }
        return ++action_it;
    }

    template <typename VLEN, template <typename> typename OP>
    Action::ItrType RvviaInsts::viavv_handler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorState* vector_state = state->getVectorState();
        switch (vector_state->getSEW())
        {
            case 8:
                return viavv_helper<VLEN, uint8_t, OP<uint8_t>>(state, action_it);
                break;
            case 16:
                return viavv_helper<VLEN, uint16_t, OP<uint16_t>>(state, action_it);
                break;
            case 32:
                return viavv_helper<VLEN, uint32_t, OP<uint32_t>>(state, action_it);
                break;
            case 64:
                return viavv_helper<VLEN, uint64_t, OP<uint64_t>>(state, action_it);
                break;
            default:
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, typename VLEN, template <typename> typename OP>
    Action::ItrType RvviaInsts::viavx_handler_(AtlasState* state, Action::ItrType action_it)
    {
        (void)state;
        return ++action_it;
    }

    template <typename VLEN, template <typename> typename OP>
    Action::ItrType RvviaInsts::viavi_handler_(AtlasState* state, Action::ItrType action_it)
    {
        (void)state;
        return ++action_it;
    }

} // namespace atlas
