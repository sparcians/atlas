#include "core/inst_handlers/v/RvvConfigSettingInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvConfigSettingInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vsetvl", pegasus::Action::createAction<&RvvConfigSettingInsts::vsetvlHandler_<XLEN>,
                                                    RvvConfigSettingInsts>(
                          nullptr, "vsetvl", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetvli", pegasus::Action::createAction<&RvvConfigSettingInsts::vsetvliHandler_<XLEN>,
                                                     RvvConfigSettingInsts>(
                           nullptr, "vsetvli", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetivli",
            pegasus::Action::createAction<&RvvConfigSettingInsts::vsetivliHandler_<XLEN>,
                                          RvvConfigSettingInsts>(nullptr, "vsetivli",
                                                                 ActionTags::EXECUTE_TAG));
    }

    template void RvvConfigSettingInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvConfigSettingInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetvlHandler_(pegasus::PegasusState* state,
                                                          Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_config->vsetAVL<XLEN>(state, true));
            }
        }
        else
        {
            vector_config->vsetAVL<XLEN>(state, false, READ_INT_REG<XLEN>(state, inst->getRs1()));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        }
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetvliHandler_(pegasus::PegasusState* state,
                                                           Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = inst->getImmediate();
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_config->vsetAVL<XLEN>(state, true));
            }
        }
        else
        {
            vector_config->vsetAVL<XLEN>(state, false, READ_INT_REG<XLEN>(state, inst->getRs1()));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        }
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetivliHandler_(pegasus::PegasusState* state,
                                                            Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = inst->getImmediate();
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        vector_config->vsetAVL<XLEN>(state, false, inst->getAVL());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        return ++action_it;
    }

} // namespace pegasus
