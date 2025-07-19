#include "core/inst_handlers/b/RvzbaInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzbaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "add.uw", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 0, true>,
                                                        RvzbaInsts>(nullptr, "add.uw",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 1, false>,
                                                        RvzbaInsts>(nullptr, "sh1add",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add.uw",
                pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 1, true>,
                                              RvzbaInsts>(nullptr, "sh1add.uw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 2, false>,
                                                        RvzbaInsts>(nullptr, "sh2add",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add.uw",
                pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 2, true>,
                                              RvzbaInsts>(nullptr, "sh2add.uw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 3, false>,
                                                        RvzbaInsts>(nullptr, "sh3add",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add.uw",
                pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV64, 3, true>,
                                              RvzbaInsts>(nullptr, "sh3add.uw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slli.uw", pegasus::Action::createAction<&RvzbaInsts::slli_uw_handler, RvzbaInsts>(
                               nullptr, "slli.uw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "sh1add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV32, 1, false>,
                                                        RvzbaInsts>(nullptr, "sh1add",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV32, 2, false>,
                                                        RvzbaInsts>(nullptr, "sh2add",
                                                                    ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add", pegasus::Action::createAction<&RvzbaInsts::shxadd_handler<RV32, 3, false>,
                                                        RvzbaInsts>(nullptr, "sh3add",
                                                                    ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzbaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, uint32_t SHIFT, bool UW>
    Action::ItrType RvzbaInsts::shxadd_handler(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        XLEN mask = -1;
        if constexpr (UW)
        {
            mask = 0xFFFFFFFFul;
        }

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1()) & mask;
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << SHIFT) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbaInsts::slli_uw_handler(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;

        const uint32_t shamt = inst->getImmediate();

        const uint64_t rd_val = rs1_val << shamt;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
} // namespace pegasus