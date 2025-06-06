#include "core/inst_handlers/b/RvzbsInsts.hpp"
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
    void RvzbsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "bclr", atlas::Action::createAction<&RvzbsInsts::bclrHandler<RV64>, RvzbsInsts>(
                            nullptr, "bclr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bclri", atlas::Action::createAction<&RvzbsInsts::bclriHandler<RV64>, RvzbsInsts>(
                            nullptr, "bclri", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bext", atlas::Action::createAction<&RvzbsInsts::bextHandler<RV64>, RvzbsInsts>(
                            nullptr, "bext", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bexti", atlas::Action::createAction<&RvzbsInsts::bextiHandler<RV64>, RvzbsInsts>(
                            nullptr, "bexti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "binv", atlas::Action::createAction<&RvzbsInsts::binvHandler<RV64>, RvzbsInsts>(
                            nullptr, "binv", ActionTags::EXECUTE_TAG));   
            inst_handlers.emplace(
                "binvi", atlas::Action::createAction<&RvzbsInsts::binviHandler<RV64>, RvzbsInsts>(
                            nullptr, "binvi", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "bset", atlas::Action::createAction<&RvzbsInsts::bsetHandler<RV64>, RvzbsInsts>(
                            nullptr, "bset", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "bseti", atlas::Action::createAction<&RvzbsInsts::bsetiHandler<RV64>, RvzbsInsts>(
                            nullptr, "bseti", ActionTags::EXECUTE_TAG));                                     
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "bclr", atlas::Action::createAction<&RvzbsInsts::bclrHandler<RV32>, RvzbsInsts>(
                            nullptr, "bclr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bclri", atlas::Action::createAction<&RvzbsInsts::bclriHandler<RV32>, RvzbsInsts>(
                            nullptr, "bclri", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bext", atlas::Action::createAction<&RvzbsInsts::bextHandler<RV32>, RvzbsInsts>(
                            nullptr, "bext", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bexti", atlas::Action::createAction<&RvzbsInsts::bextiHandler<RV32>, RvzbsInsts>(
                            nullptr, "bexti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "binv", atlas::Action::createAction<&RvzbsInsts::binvHandler<RV32>, RvzbsInsts>(
                            nullptr, "binv", ActionTags::EXECUTE_TAG));   
            inst_handlers.emplace(
                "binvi", atlas::Action::createAction<&RvzbsInsts::binviHandler<RV32>, RvzbsInsts>(
                            nullptr, "binvi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bset", atlas::Action::createAction<&RvzbsInsts::bsetHandler<RV32>, RvzbsInsts>(
                            nullptr, "bset", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "bseti", atlas::Action::createAction<&RvzbsInsts::bsetiHandler<RV32>, RvzbsInsts>(
                            nullptr, "bseti", ActionTags::EXECUTE_TAG));       
        }
    }

    template void RvzbsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bclrHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN index = rs2_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val & ~(XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bclriHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN index = imm_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val & ~(XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bextHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN index = rs2_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = (rs1_val >> index) & XLEN(1);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bextiHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN index = imm_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = (rs1_val >> index) & XLEN(1);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::binvHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN index = rs2_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val ^ (XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::binviHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN index = imm_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val ^ (XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bsetHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN index = rs2_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val | (XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbsInsts::bsetiHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN index = imm_val & (sizeof(XLEN) * 8 - 1);
        const XLEN rd_val = rs1_val | (XLEN(1) << index);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
}