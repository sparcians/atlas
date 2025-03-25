#pragma once

#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"
#include "include/CSRBitMasks64.hpp"
#include "mavis/OpcodeInfo.h"

extern "C"
{
#include "source/include/softfloat.h"
}

namespace atlas
{
    class RvfInstsBase
    {

      protected:
        using SP = uint32_t;
        using DP = uint64_t;

        template <typename XLEN> inline uint_fast8_t getRM(AtlasState* state)
        {
            auto inst = state->getCurrentInst();
            uint64_t static_rm = inst->getRM();
            if (static_rm == 7) // RM field "DYN"
            {
                static_rm = READ_CSR_REG<XLEN>(state, FRM);
            }
            return static_rm;
        }

        // From RISCV manual:
        // the value −0.0 is considered to be less than the value +0.0. If both inputs are NaNs, the
        // result is the canonical NaN. If only one operand is a NaN, the result is the non-NaN
        // operand.
        template <typename SIZE>
        static void fmaxFminNanZeroCheck(SIZE rs1_val, SIZE rs2_val, SIZE & rd_val, bool max)
        {
            static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);

            const Constants<SIZE> & cons = getConst<SIZE>();

            bool rs1_nan =
                ((rs1_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs1_val & cons.SIG_MASK);
            bool rs2_nan =
                ((rs2_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs2_val & cons.SIG_MASK);
            if (rs1_nan && rs2_nan)
            {
                rd_val = cons.CAN_NAN;
            }
            else if (rs1_nan)
            {
                rd_val = rs2_val;
            }
            else if (rs2_nan)
            {
                rd_val = rs1_val;
            }

            if (((rs1_val == cons.NEG_ZERO) && (rs2_val == cons.POS_ZERO))
                || ((rs2_val == cons.NEG_ZERO) && (rs1_val == cons.POS_ZERO)))
            {
                rd_val = max ? cons.POS_ZERO : cons.NEG_ZERO;
            }
        }

        template <typename XLEN> static ActionGroup* updateCsr(AtlasState* state)
        {
            // TODO: it would be nice to have field shift, then a single combined CSR write will
            // suffice.

            // FFLAGS
            WRITE_CSR_FIELD<XLEN>(
                state, FFLAGS, "NX",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_inexact) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FFLAGS, "UF",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_underflow) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FFLAGS, "OF",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_overflow) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FFLAGS, "DZ",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_infinite) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FFLAGS, "NV",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_invalid) != 0));

            // FCSR
            WRITE_CSR_FIELD<XLEN>(
                state, FCSR, "NX",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_inexact) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FCSR, "UF",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_underflow) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FCSR, "OF",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_overflow) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FCSR, "DZ",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_infinite) != 0));
            WRITE_CSR_FIELD<XLEN>(
                state, FCSR, "NV",
                static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_invalid) != 0));

            return nullptr;
        }

        template <typename XLEN> ActionGroup* computeAddressHandler(AtlasState* state)
        {
            static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);

            const AtlasInstPtr & inst = state->getCurrentInst();
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
            constexpr uint32_t IMM_SIZE = 12;
            const XLEN imm =
                inst->hasImmediate() ? inst->getSignExtendedImmediate<XLEN, IMM_SIZE>() : 0;
            const XLEN vaddr = rs1_val + imm;
            inst->getTranslationState()->makeRequest(vaddr, sizeof(XLEN));
            return nullptr;
        }

        // Check and convert a narrower SIZE floating point value from wider floating point
        // register.
        template <typename XLEN, typename SIZE> SIZE checkNanBoxing(XLEN num)
        {
            static_assert(sizeof(XLEN) > sizeof(SIZE));

            const Constants<SIZE> & cons = getConst<SIZE>();
            constexpr XLEN mask = SIZE(-1);
            SIZE value = cons.CAN_NAN;
            if ((num & ~mask) == ~mask) // upper bits all 1's
            {
                value = num; // truncated
            }
            return value;
        }

        // NaN-boxing a narrower SIZE floating point value for wider floating point register.
        template <typename XLEN, typename SIZE> inline XLEN nanBoxing(XLEN num)
        {
            static_assert(sizeof(XLEN) > sizeof(SIZE));
            constexpr XLEN mask = SIZE(-1);
            return ~mask | (num & mask);
        }

        template <typename XLEN, typename SIZE, bool LOAD>
        ActionGroup* floatLsHandler(AtlasState* state)
        {
            static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);
            static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);
            static_assert(sizeof(XLEN) >= sizeof(SIZE));

            const AtlasInstPtr & inst = state->getCurrentInst();
            const XLEN paddr = inst->getTranslationState()->getResult().getPaddr();

            if constexpr (LOAD)
            {
                XLEN value = state->readMemory<XLEN>(paddr);
                if constexpr (sizeof(XLEN) > sizeof(SIZE))
                {
                    value = nanBoxing<XLEN, SIZE>(value);
                }
                WRITE_FP_REG<XLEN>(state, inst->getRd(), value);
            }
            else
            {
                state->writeMemory<SIZE>(paddr, READ_FP_REG<XLEN>(state, inst->getRs2()));
            }
            return nullptr;
        }

      private:
        template <typename SIZE> struct Constants
        {
            static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);

            static constexpr uint8_t SGN_BIT = sizeof(SIZE) * 8 - 1;
            static constexpr uint8_t EXP_MSB = SGN_BIT - 1;
            static constexpr uint8_t EXP_LSB = std::is_same<SIZE, DP>::value ? 52 : 23;
            static constexpr uint8_t SIG_MSB = EXP_LSB - 1;

            static constexpr SIZE EXP_MASK = (((SIZE)1 << (EXP_MSB - EXP_LSB + 1)) - 1) << EXP_LSB;
            static constexpr SIZE SIG_MASK = ((SIZE)1 << (SIG_MSB + 1)) - 1;

            static constexpr SIZE CAN_NAN = EXP_MASK | (SIZE)1 << SIG_MSB;
            static constexpr SIZE NEG_ZERO = (SIZE)1 << SGN_BIT;
            static constexpr SIZE POS_ZERO = 0;
        }; // struct Constants

        static constexpr Constants<SP> const_sp{};
        static constexpr Constants<DP> const_dp{};

        template <typename SIZE> static constexpr Constants<SIZE> getConst()
        {
            static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);
            if constexpr (std::is_same<SIZE, SP>::value)
            {
                return const_sp;
            }
            else
            {
                return const_dp;
            }
        }

    }; // class RvfInstsBase

} // namespace atlas
