#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvmInsts::getInstHandlers(Execute::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "div", atlas::Action::createAction<&RvmInsts::div_64_handler<RV64>, RvmInsts>(
                           nullptr, "div", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "divu", atlas::Action::createAction<&RvmInsts::divu_64_handler<RV64>, RvmInsts>(
                            nullptr, "divu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "divuw", atlas::Action::createAction<&RvmInsts::divuw_64_handler, RvmInsts>(
                             nullptr, "divuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("divw",
                                  atlas::Action::createAction<&RvmInsts::divw_64_handler, RvmInsts>(
                                      nullptr, "divw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mul", atlas::Action::createAction<&RvmInsts::mul_64_handler<RV64>, RvmInsts>(
                           nullptr, "mul", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulh", atlas::Action::createAction<&RvmInsts::mulh_64_handler<RV64>, RvmInsts>(
                            nullptr, "mulh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhsu", atlas::Action::createAction<&RvmInsts::mulhsu_64_handler<RV64>, RvmInsts>(
                              nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhu", atlas::Action::createAction<&RvmInsts::mulhu_64_handler<RV64>, RvmInsts>(
                             nullptr, "mulhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mulw",
                                  atlas::Action::createAction<&RvmInsts::mulw_64_handler, RvmInsts>(
                                      nullptr, "mulw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rem", atlas::Action::createAction<&RvmInsts::rem_64_handler<RV64>, RvmInsts>(
                           nullptr, "rem", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "remu", atlas::Action::createAction<&RvmInsts::remu_64_handler<RV64>, RvmInsts>(
                            nullptr, "remu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "remuw", atlas::Action::createAction<&RvmInsts::remuw_64_handler, RvmInsts>(
                             nullptr, "remuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("remw",
                                  atlas::Action::createAction<&RvmInsts::remw_64_handler, RvmInsts>(
                                      nullptr, "remw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "div", atlas::Action::createAction<&RvmInsts::div_64_handler<RV32>, RvmInsts>(
                           nullptr, "div", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "divu", atlas::Action::createAction<&RvmInsts::divu_64_handler<RV32>, RvmInsts>(
                            nullptr, "divu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mul", atlas::Action::createAction<&RvmInsts::mul_64_handler<RV32>, RvmInsts>(
                           nullptr, "mul", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulh", atlas::Action::createAction<&RvmInsts::mulh_64_handler<RV32>, RvmInsts>(
                            nullptr, "mulh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhsu", atlas::Action::createAction<&RvmInsts::mulhsu_64_handler<RV32>, RvmInsts>(
                              nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhu", atlas::Action::createAction<&RvmInsts::mulhu_64_handler<RV32>, RvmInsts>(
                             nullptr, "mulhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rem", atlas::Action::createAction<&RvmInsts::rem_64_handler<RV32>, RvmInsts>(
                           nullptr, "rem", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "remu", atlas::Action::createAction<&RvmInsts::remu_64_handler<RV32>, RvmInsts>(
                            nullptr, "remu", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvmInsts::getInstHandlers<RV32>(Execute::InstHandlersMap &);
    template void RvmInsts::getInstHandlers<RV64>(Execute::InstHandlersMap &);

    template <typename XLEN> ActionGroup* RvmInsts::div_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        sreg_t lhs = sext(rs1_val, state->getXlen());
        sreg_t rhs = sext(rs2_val, state->getXlen());

        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), UINT64_MAX);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), lhs);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext((lhs / rhs), state->getXlen()));
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::divu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        reg_t lhs = zext(rs1_val, state->getXlen());
        reg_t rhs = zext(rs2_val, state->getXlen());
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext((lhs / rhs), state->getXlen()));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::divuw_64_handler(atlas::AtlasState* state)
    {
        using XLEN = uint64_t;
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(lhs / rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::divw_64_handler(atlas::AtlasState* state)
    {
        using XLEN = uint64_t;
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(lhs / rhs));
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::mul_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());
        const XLEN rd_val = rs1_val * rs2_val;
        WRITE_INT_REG<XLEN>(state, insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::mulh_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), mulh(rs1_val, rs2_val));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(),
                                sext32((sext32(rs1_val) * sext32(rs2_val)) >> 32));
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::mulhsu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), mulhsu(rs1_val, rs2_val));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(),
                                sext32((sext32(rs1_val) * reg_t((uint32_t)rs2_val)) >> 32));
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::mulhu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), mulhu(rs1_val, rs2_val));
        }
        else
        {
            WRITE_INT_REG<XLEN>(
                state, insn->getRd(),
                sext32(((uint64_t)(uint32_t)rs1_val * (uint64_t)(uint32_t)rs2_val) >> 32));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::mulw_64_handler(atlas::AtlasState* state)
    {
        using XLEN = uint64_t;
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());
        WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(rs1_val * rs2_val));

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::rem_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        sreg_t lhs = sext(rs1_val, state->getXlen());
        sreg_t rhs = sext(rs2_val, state->getXlen());

        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), lhs);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), 0);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext((lhs % rhs), state->getXlen()));
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvmInsts::remu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        reg_t lhs = zext(rs1_val, state->getXlen());
        reg_t rhs = zext(rs2_val, state->getXlen());
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext(lhs, state->getXlen()));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext((lhs % rhs), state->getXlen()));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::remuw_64_handler(atlas::AtlasState* state)
    {
        using XLEN = uint64_t;
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(lhs));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(lhs % rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::remw_64_handler(atlas::AtlasState* state)
    {
        using XLEN = uint64_t;
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, insn->getRs2());

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), lhs);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, insn->getRd(), sext32(lhs % rhs));
        }

        return nullptr;
    }
} // namespace atlas
