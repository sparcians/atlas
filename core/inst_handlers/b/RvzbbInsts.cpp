#include "core/inst_handlers/b/RvzbbInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "system/AtlasSystem.hpp"

#include <bit>

namespace atlas
{
    template <typename XLEN>
    void RvzbbInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "andn", atlas::Action::createAction<&RvzbbInsts::andnHandler<RV64>, RvzbbInsts>(
                            nullptr, "andn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", atlas::Action::createAction<&RvzbbInsts::clzHandler<RV64>, RvzbbInsts>(
                            nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clzw", atlas::Action::createAction<&RvzbbInsts::clzwHandler, RvzbbInsts>(
                            nullptr, "clzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", atlas::Action::createAction<&RvzbbInsts::cpopHandler<RV64>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));     
             inst_handlers.emplace(
                "cpopw", atlas::Action::createAction<&RvzbbInsts::cpopwHandler, RvzbbInsts>(
                            nullptr, "cpopw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "ctz", atlas::Action::createAction<&RvzbbInsts::ctzHandler<RV64>, RvzbbInsts>(
                            nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctzw", atlas::Action::createAction<&RvzbbInsts::ctzwHandler, RvzbbInsts>(
                            nullptr, "ctzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max", atlas::Action::createAction<&RvzbbInsts::maxHandler<RV64>, RvzbbInsts>(
                            nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu", atlas::Action::createAction<&RvzbbInsts::maxuHandler<RV64>, RvzbbInsts>(
                            nullptr, "maxu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min", atlas::Action::createAction<&RvzbbInsts::minHandler<RV64>, RvzbbInsts>(
                            nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu", atlas::Action::createAction<&RvzbbInsts::minuHandler<RV64>, RvzbbInsts>(
                            nullptr, "minu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b", atlas::Action::createAction<&RvzbbInsts::orc_bHandler<RV64>, RvzbbInsts>(
                            nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn", atlas::Action::createAction<&RvzbbInsts::ornHandler<RV64>, RvzbbInsts>(
                            nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", atlas::Action::createAction<&RvzbbInsts::rev8Handler<RV64>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol", atlas::Action::createAction<&RvzbbInsts::rolHandler<RV64>, RvzbbInsts>(
                            nullptr, "rol", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rolw", atlas::Action::createAction<&RvzbbInsts::rolwHandler, RvzbbInsts>(
                            nullptr, "rolw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "ror", atlas::Action::createAction<&RvzbbInsts::rorHandler<RV64>, RvzbbInsts>(
                            nullptr, "ror", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rori", atlas::Action::createAction<&RvzbbInsts::roriHandler<RV64>, RvzbbInsts>(
                            nullptr, "rori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "roriw", atlas::Action::createAction<&RvzbbInsts::roriwHandler, RvzbbInsts>(
                            nullptr, "roriw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rorw", atlas::Action::createAction<&RvzbbInsts::rorwHandler, RvzbbInsts>(
                            nullptr, "rorw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "sext.b", atlas::Action::createAction<&RvzbbInsts::sext_bHandler<RV64>, RvzbbInsts>(
                            nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", atlas::Action::createAction<&RvzbbInsts::sext_hHandler<RV64>, RvzbbInsts>(
                            nullptr, "sext.h", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xnor", atlas::Action::createAction<&RvzbbInsts::xnorHandler<RV64>, RvzbbInsts>(
                            nullptr, "xnor", ActionTags::EXECUTE_TAG));  
            inst_handlers.emplace(
                "zext.h", atlas::Action::createAction<&RvzbbInsts::zext_hHandler<RV64>, RvzbbInsts>(
                            nullptr, "zext.h", ActionTags::EXECUTE_TAG));         
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "andn", atlas::Action::createAction<&RvzbbInsts::andnHandler<RV32>, RvzbbInsts>(
                            nullptr, "andn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", atlas::Action::createAction<&RvzbbInsts::clzHandler<RV32>, RvzbbInsts>(
                            nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", atlas::Action::createAction<&RvzbbInsts::cpopHandler<RV32>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctz", atlas::Action::createAction<&RvzbbInsts::ctzHandler<RV32>, RvzbbInsts>(
                            nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max", atlas::Action::createAction<&RvzbbInsts::maxHandler<RV32>, RvzbbInsts>(
                            nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu", atlas::Action::createAction<&RvzbbInsts::maxuHandler<RV32>, RvzbbInsts>(
                            nullptr, "maxu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min", atlas::Action::createAction<&RvzbbInsts::minHandler<RV32>, RvzbbInsts>(
                            nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu", atlas::Action::createAction<&RvzbbInsts::minuHandler<RV32>, RvzbbInsts>(
                            nullptr, "minu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b", atlas::Action::createAction<&RvzbbInsts::orc_bHandler<RV32>, RvzbbInsts>(
                            nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn", atlas::Action::createAction<&RvzbbInsts::ornHandler<RV32>, RvzbbInsts>(
                            nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", atlas::Action::createAction<&RvzbbInsts::rev8Handler<RV32>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol", atlas::Action::createAction<&RvzbbInsts::rolHandler<RV32>, RvzbbInsts>(
                            nullptr, "rol", ActionTags::EXECUTE_TAG));   
            inst_handlers.emplace(
                "ror", atlas::Action::createAction<&RvzbbInsts::rorHandler<RV32>, RvzbbInsts>(
                            nullptr, "ror", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rori", atlas::Action::createAction<&RvzbbInsts::roriHandler<RV32>, RvzbbInsts>(
                            nullptr, "rori", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "sext.b", atlas::Action::createAction<&RvzbbInsts::sext_bHandler<RV32>, RvzbbInsts>(
                            nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", atlas::Action::createAction<&RvzbbInsts::sext_hHandler<RV32>, RvzbbInsts>(
                            nullptr, "sext.h", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "xnor", atlas::Action::createAction<&RvzbbInsts::xnorHandler<RV32>, RvzbbInsts>(
                            nullptr, "xnor", ActionTags::EXECUTE_TAG));          
            inst_handlers.emplace(
                "zext.h", atlas::Action::createAction<&RvzbbInsts::zext_hHandler<RV32>, RvzbbInsts>(
                            nullptr, "zext.h", ActionTags::EXECUTE_TAG));            
        }
    }
    
    template void RvzbbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::andnHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = rs1_val & (~rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::clzHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        
        const XLEN rd_val = std::countl_zero(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::clzwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
         const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint32_t>(state, inst->getRs1());
        
        const uint32_t rd_val = std::countl_zero(rs1_val);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
    
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::cpopHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        const XLEN rd_val = std::popcount(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::cpopwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint32_t>(state, inst->getRs1());

        const uint32_t rd_val = std::popcount(rs1_val);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::ctzHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        
        const XLEN rd_val = std::countr_zero(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::ctzwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint32_t>(state, inst->getRs1());

        const uint32_t rd_val = std::countr_zero(rs1_val);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Should be optimized
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::maxHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        // Cast to signed value
        using S_XLEN = std::conditional_t<std::is_same_v<XLEN, RV64>, int64_t, int32_t>;
        const S_XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const S_XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        
        const XLEN rd_val = std::max(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::maxuHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        
       const XLEN rd_val = std::max(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Should be optimized
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::minHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        // Cast to signed value
        using S_XLEN = std::conditional_t<std::is_same_v<XLEN, RV64>, int64_t, int32_t>;
        const S_XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const S_XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        
        const XLEN rd_val = std::min(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::minuHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        
        const XLEN rd_val = std::min(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Not properly implemented yet
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::orc_bHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        XLEN output = 0;
        for (uint32_t i = 0; i < sizeof(XLEN); i++) {
            output = ((rs1_val & (0xFFull << i * 8)) == 0) ? output : output | (0xFFull << i * 8);
        }

        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::ornHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = rs1_val | (~rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Not properly implemented yet
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::rev8Handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        const XLEN rd_val = sparta::byte_swap(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }


    // Should be optimized
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::rolHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        // Chooses appropriate bitmask
        const uint32_t shamt = rs2_val & (sizeof(XLEN) * 8 - 1);

        const XLEN rd_val = std::rotl(rs1_val, shamt);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::rolwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        uint32_t shamt = rs2_val & 0x3F;

        const int64_t rd_val = static_cast<int64_t>(static_cast<int32_t>(std::rotl(rs1_val, shamt)));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Should be optimized
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::rorHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        // Chooses appropriate bitmask
        const uint32_t shamt = rs2_val & (sizeof(XLEN) * 8 - 1);

        const XLEN rd_val = std::rotr(rs1_val, shamt);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    // Should be optimized
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::roriHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        // Chooses appropriate bitmask
        const uint32_t shamt = imm_val & (sizeof(XLEN) * 8 - 1);

        const XLEN rd_val = std::rotr(rs1_val, shamt);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::roriwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t imm_val = inst->getImmediate();

        uint32_t shamt = imm_val & 0x3F;

        const int64_t rd_val = static_cast<int64_t>(static_cast<int32_t>(std::rotr(rs1_val, shamt)));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvzbbInsts::rorwHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1()) & 0xFFFFFFFFull;
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        uint32_t shamt = rs2_val & 0x3F;

        const int64_t rd_val = static_cast<int64_t>(static_cast<int32_t>(std::rotr(rs1_val, shamt)));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::sext_bHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

         const XLEN rd_val =  sext(rs1_val, 8);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        
        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::sext_hHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

         const XLEN rd_val =  sext(rs1_val, 16);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        
        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::xnorHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = ~(rs1_val ^ rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> 
    Action::ItrType RvzbbInsts::zext_hHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

         const XLEN rd_val =  zext(rs1_val, 16);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        
        return ++action_it;
    }
}