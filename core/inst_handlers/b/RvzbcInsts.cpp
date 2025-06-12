#include "core/inst_handlers/b/RvzbcInsts.hpp"
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
    void RvzbcInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

            inst_handlers.emplace(
                "clmul", atlas::Action::createAction<&RvzbcInsts::clmulHandler<XLEN>, RvzbcInsts>(
                            nullptr, "clmul", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clmulh", atlas::Action::createAction<&RvzbcInsts::clmulhHandler<XLEN>, RvzbcInsts>(
                            nullptr, "clmulh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clmulr", atlas::Action::createAction<&RvzbcInsts::clmulrHandler<XLEN>, RvzbcInsts>(
                            nullptr, "clmulr", ActionTags::EXECUTE_TAG));
    }

    template void RvzbcInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbcInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> 
    Action::ItrType RvzbcInsts::clmulHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        XLEN output = 0;

        for(uint32_t i = 0; i < sizeof(XLEN) * 8; i++) {
            output ^= ((rs2_val >> i) & 1) * (rs1_val << i);
        }
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbcInsts::clmulhHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        XLEN output = 0;

        for(uint32_t i = 1; i < sizeof(XLEN) * 8; i++) {
            output ^= ((rs2_val >> i) & 1) * (rs1_val >> (sizeof(XLEN) * 8 - i));
        }
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbcInsts::clmulrHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        XLEN output = 0;

        for(uint32_t i = 0; i < sizeof(XLEN) * 8; i++) {
            output ^= ((rs2_val >> i) & 1) * (rs1_val >> (sizeof(XLEN) * 8 - i -1));
        }
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }
}