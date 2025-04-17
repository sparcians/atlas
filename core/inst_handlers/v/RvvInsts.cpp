#include "core/inst_handlers/v/RvvInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/VectorState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        (void)inst_handlers;
    }

    template <typename XLEN>
    void RvvInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // TODO: decide how to pass VLEN as argument.
        using VLEN = XLEN;

        inst_handlers.emplace(
            "vsetvl", atlas::Action::createAction<&RvvInsts::vsetvl_handler<XLEN, VLEN>, RvvInsts>(
                          nullptr, "vsetvl", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetvli",
            atlas::Action::createAction<&RvvInsts::vsetvli_handler<XLEN, VLEN>, RvvInsts>(
                nullptr, "vsetvli", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetivli",
            atlas::Action::createAction<&RvvInsts::vsetivli_handler<XLEN, VLEN>, RvvInsts>(
                nullptr, "vsetivli", ActionTags::EXECUTE_TAG));
    }

    template void RvvInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvvInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename VLEN>
    ActionGroup* RvvInsts::vsetvl_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        VectorState* vector_state = state->getVectorState();
        vector_state->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_state->vsetAVL<XLEN, VLEN>(state, true));
            }
        }
        else
        {
            vector_state->vsetAVL<XLEN, VLEN>(state, false,
                                              READ_INT_REG<XLEN>(state, inst->getRs1()));
        }
        return nullptr;
    }

    template <typename XLEN, typename VLEN>
    ActionGroup* RvvInsts::vsetvli_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = inst->getImmediate();
        VectorState* vector_state = state->getVectorState();
        vector_state->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_state->vsetAVL<XLEN, VLEN>(state, true));
            }
        }
        else
        {
            vector_state->vsetAVL<XLEN, VLEN>(state, false,
                                              READ_INT_REG<XLEN>(state, inst->getRs1()));
        }
        return nullptr;
    }

    template <typename XLEN, typename VLEN>
    ActionGroup* RvvInsts::vsetivli_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = READ_INT_REG<XLEN>(state, inst->getImmediate());
        VectorState* vector_state = state->getVectorState();
        vector_state->vsetVTYPE<XLEN>(state, vtype_val);
        vector_state->vsetAVL<XLEN, VLEN>(state, false, READ_INT_REG<XLEN>(state, inst->getAVL()));
        return nullptr;
    }

} // namespace atlas
