#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>
#include <bit>

namespace pegasus
{
    class PegasusState;

    class RvzbbInsts
    {
      public:
        using base_type = RvzbbInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        template <typename XLEN> struct andn
        {
            XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val & (~rs2_val); }
        };

        template <typename XLEN> struct countl_zero
        {
            XLEN operator()(XLEN rs1_val) const { return std::countl_zero(rs1_val); }
        };

        template <typename XLEN> struct popcount
        {
            XLEN operator()(XLEN rs1_val) const { return std::popcount(rs1_val); }
        };

        template <typename XLEN> struct countr_zero
        {
            XLEN operator()(XLEN rs1_val) const { return std::countr_zero(rs1_val); }
        };

        template <typename XLEN> struct byte_swap
        {
            XLEN operator()(XLEN rs1_val) const
            {
                if constexpr (std::is_same_v<XLEN, RV64>)
                {
                    return __builtin_bswap64(rs1_val);
                }
                else
                    return __builtin_bswap32(rs1_val);
            }
        };

        template <typename S_XLEN> struct max
        {
            S_XLEN operator()(S_XLEN rs1_val, S_XLEN rs2_val) const
            {
                return std::max(rs1_val, rs2_val);
            }
        };

        template <typename S_XLEN> struct min
        {
            S_XLEN operator()(S_XLEN rs1_val, S_XLEN rs2_val) const
            {
                return std::min(rs1_val, rs2_val);
            }
        };

        template <typename XLEN> struct orn
        {
            XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val | (~rs2_val); }
        };

        template <typename XLEN> struct rol
        {
            XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
            {
                return std::rotl(rs1_val, rs2_val & (sizeof(XLEN) * 8 - 1));
            }
        };

        template <typename S_XLEN> struct rolw
        {
            S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
            {
                return static_cast<int32_t>(std::rotl(rs1_val, rs2_val & 0x1Full));
            }
        };

        template <typename XLEN> struct ror
        {
            XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
            {
                return std::rotr(rs1_val, rs2_val & (sizeof(XLEN) * 8 - 1));
            }
        };

        template <typename S_XLEN> struct rorw
        {
            S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
            {
                return static_cast<int32_t>(std::rotr(rs1_val, rs2_val & 0x1Full));
            }
        };

        template <typename XLEN, uint32_t x> struct sext_x
        {
            XLEN operator()(XLEN rs1_val) const
            {
                return (((int64_t)(rs1_val) << (64 - (x))) >> (64 - (x)));
            }
        };

        template <typename XLEN> struct xnor
        {
            XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return ~(rs1_val ^ rs2_val); }
        };

        template <typename XLEN> struct zext_h
        {
            XLEN operator()(XLEN rs1_val) const
            {
                return (((uint64_t)(rs1_val) << (64 - (16))) >> (64 - (16)));
            }
        };

        template <typename XLEN, typename OP>
        Action::ItrType unaryOpHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType binaryOpHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType immOpHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType orc_bHandler(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus