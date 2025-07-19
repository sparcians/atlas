#include "core/inst_handlers/b/RvzbsInsts.hpp"
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
    void RvzbsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "bclr", pegasus::Action::createAction<
                        &RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bclr<XLEN>>, RvzbsInsts>(
                        nullptr, "bclr", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bclri",
            pegasus::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bclr<XLEN>>,
                                          RvzbsInsts>(nullptr, "bclri", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bext", pegasus::Action::createAction<
                        &RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bext<XLEN>>, RvzbsInsts>(
                        nullptr, "bext", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bexti",
            pegasus::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bext<XLEN>>,
                                          RvzbsInsts>(nullptr, "bexti", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binv", pegasus::Action::createAction<
                        &RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::binv<XLEN>>, RvzbsInsts>(
                        nullptr, "binv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binvi",
            pegasus::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::binv<XLEN>>,
                                          RvzbsInsts>(nullptr, "binvi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bset", pegasus::Action::createAction<
                        &RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bset<XLEN>>, RvzbsInsts>(
                        nullptr, "bset", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bseti",
            pegasus::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bset<XLEN>>,
                                          RvzbsInsts>(nullptr, "bseti", ActionTags::EXECUTE_TAG));
    }

    template void RvzbsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename OP>
    Action::ItrType RvzbsInsts::binaryOpHandler(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = OP()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbsInsts::immOpHandler(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN rd_val = OP()(rs1_val, imm_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
} // namespace pegasus