#include "core/inst_handlers/b/RvzbbInsts.hpp"
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
    void RvzbbInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "andn", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::andn<uint64_t>>, RvzbbInsts>(
                            nullptr, "andn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::countl_zero<uint64_t>>, RvzbbInsts>(
                            nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clzw", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::countl_zero<uint32_t>>, RvzbbInsts>(
                            nullptr, "clzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::popcount<uint64_t>>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));     
             inst_handlers.emplace(
                "cpopw", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::popcount<uint32_t>>, RvzbbInsts>(
                            nullptr, "cpopw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "ctz", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::countr_zero<uint64_t>>, RvzbbInsts>(
                            nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctzw", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::countr_zero<uint32_t>>, RvzbbInsts>(
                            nullptr, "ctzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::max<int64_t>>, RvzbbInsts>(
                            nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::max<uint64_t>>, RvzbbInsts>(
                            nullptr, "maxu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::min<int64_t>>, RvzbbInsts>(
                            nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::min<uint64_t>>, RvzbbInsts>(
                            nullptr, "minu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b", atlas::Action::createAction<&RvzbbInsts::orc_bHandler<RV64>, RvzbbInsts>(
                            nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::orn<uint64_t>>, RvzbbInsts>(
                            nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::byte_swap<uint64_t>>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::rol<uint64_t>>, RvzbbInsts>(
                            nullptr, "rol", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rolw", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::rolw<int64_t>>, RvzbbInsts>(
                            nullptr, "rolw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "ror", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::ror<uint64_t>>, RvzbbInsts>(
                            nullptr, "ror", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rori", atlas::Action::createAction<&RvzbbInsts::immOpHandler<RV64, RvzbbInsts::ror<uint64_t>>, RvzbbInsts>(
                            nullptr, "rori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "roriw", atlas::Action::createAction<&RvzbbInsts::immOpHandler<RV64, RvzbbInsts::rorw<int64_t>>, RvzbbInsts>(
                            nullptr, "roriw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rorw", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::rorw<int64_t>>, RvzbbInsts>(
                            nullptr, "rorw", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "sext.b", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::sext_x<uint64_t, 8>>, RvzbbInsts>(
                            nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::sext_x<uint64_t, 16>>, RvzbbInsts>(
                            nullptr, "sext.h", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xnor", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV64, RvzbbInsts::xnor<uint64_t>>, RvzbbInsts>(
                            nullptr, "xnor", ActionTags::EXECUTE_TAG));  
            inst_handlers.emplace(
                "zext.h", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV64, RvzbbInsts::zext_h<uint64_t>>, RvzbbInsts>(
                            nullptr, "zext.h", ActionTags::EXECUTE_TAG));         
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "andn", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::andn<uint32_t>>, RvzbbInsts>(
                            nullptr, "andn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::countl_zero<uint32_t>>, RvzbbInsts>(
                            nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::popcount<uint32_t>>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctz", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::countr_zero<uint32_t>>, RvzbbInsts>(
                            nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::max<int32_t>>, RvzbbInsts>(
                            nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::max<uint32_t>>, RvzbbInsts>(
                            nullptr, "maxu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::min<int32_t>>, RvzbbInsts>(
                            nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::min<uint32_t>>, RvzbbInsts>(
                            nullptr, "minu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b", atlas::Action::createAction<&RvzbbInsts::orc_bHandler<RV32>, RvzbbInsts>(
                            nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::orn<uint32_t>>, RvzbbInsts>(
                            nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::byte_swap<uint32_t>>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::rol<uint32_t>>, RvzbbInsts>(
                            nullptr, "rol", ActionTags::EXECUTE_TAG));   
            inst_handlers.emplace(
                "ror", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::ror<uint32_t>>, RvzbbInsts>(
                            nullptr, "ror", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "rori", atlas::Action::createAction<&RvzbbInsts::immOpHandler<RV32, RvzbbInsts::ror<uint32_t>>, RvzbbInsts>(
                            nullptr, "rori", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "sext.b", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::sext_x<uint32_t, 8>>, RvzbbInsts>(
                            nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::sext_x<uint32_t, 16>>, RvzbbInsts>(
                            nullptr, "sext.h", ActionTags::EXECUTE_TAG)); 
            inst_handlers.emplace(
                "xnor", atlas::Action::createAction<&RvzbbInsts::binaryOpHandler<RV32, RvzbbInsts::xnor<uint32_t>>, RvzbbInsts>(
                            nullptr, "xnor", ActionTags::EXECUTE_TAG));          
            inst_handlers.emplace(
                "zext.h", atlas::Action::createAction<&RvzbbInsts::unaryOpHandler<RV32, RvzbbInsts::zext_h<uint32_t>>, RvzbbInsts>(
                            nullptr, "zext.h", ActionTags::EXECUTE_TAG));            
        }
    }
    
    template void RvzbbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    
    template <typename XLEN, typename OP> 
    Action::ItrType RvzbbInsts::unaryOpHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        
        const XLEN rd_val = OP()(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it; 
    }

    template <typename XLEN, typename OP> 
    Action::ItrType RvzbbInsts::binaryOpHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        
        const XLEN rd_val = OP()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it; 
    }

    template <typename XLEN, typename OP> 
    Action::ItrType RvzbbInsts::immOpHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        
        const XLEN rd_val = OP()(rs1_val, imm_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it; 
    }
    
    template <typename XLEN> 
    Action::ItrType RvzbbInsts::orc_bHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        XLEN output = 0;
        for (uint32_t i = 0; i < sizeof(XLEN); i++) {
            uint64_t byte_mask = 0xFFull << (i * 8);
            output |= byte_mask & -!!(rs1_val & byte_mask);
        }

        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }
}