#include "core/inst_handlers/b/RvzbaInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzbaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "add.uw", atlas::Action::createAction<&RvzbaInsts::add_uw_handler, RvzbaInsts>(
                            nullptr, "add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add", atlas::Action::createAction<&RvzbaInsts::sh1add_handler<RV64>, RvzbaInsts>(
                            nullptr, "sh1add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add.uw", atlas::Action::createAction<&RvzbaInsts::sh1add_uw_handler, RvzbaInsts>(
                        nullptr, "sh1add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add", atlas::Action::createAction<&RvzbaInsts::sh2add_handler<RV64>, RvzbaInsts>(
                        nullptr, "sh2add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add.uw", atlas::Action::createAction<&RvzbaInsts::sh2add_uw_handler, RvzbaInsts>(
                        nullptr, "sh2add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add", atlas::Action::createAction<&RvzbaInsts::sh3add_handler<RV64>, RvzbaInsts>(
                        nullptr, "sh3add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add.uw", atlas::Action::createAction<&RvzbaInsts::sh3add_uw_handler, RvzbaInsts>(
                        nullptr, "sh3add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slli.uw", atlas::Action::createAction<&RvzbaInsts::slli_uw_handler, RvzbaInsts>(
                        nullptr, "slli.uw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
        inst_handlers.emplace(
            "sh1add", atlas::Action::createAction<&RvzbaInsts::sh1add_handler<RV32>, RvzbaInsts>(
                nullptr, "sh1add", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sh2add", atlas::Action::createAction<&RvzbaInsts::sh2add_handler<RV32>, RvzbaInsts>(
                nullptr, "sh2add", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sh3add", atlas::Action::createAction<&RvzbaInsts::sh3add_handler<RV32>, RvzbaInsts>(
                nullptr, "sh3add", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzbaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    Action::ItrType RvzbaInsts::add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        // Ensures that rs1_val only receives the least-significant word of rs1
        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const uint64_t rd_val = rs1_val + rs2_val; 
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbaInsts::sh1add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 1) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbaInsts::sh1add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const uint64_t rd_val = (rs1_val << 1) + rs2_val;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbaInsts::sh2add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 2) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbaInsts::sh2add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const uint64_t rd_val = (rs1_val << 2) + rs2_val;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbaInsts::sh3add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 3) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbaInsts::sh3add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const uint64_t rd_val = (rs1_val << 3) + rs2_val;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbaInsts::slli_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;

        const uint32_t shamt = static_cast<uint64_t>(static_cast<uint32_t>(inst->getImmediate()));

        const uint64_t rd_val = rs1_val << shamt;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
}