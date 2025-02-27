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
    using SP = uint32_t;
    using DP = uint64_t;

    template <typename XLEN> inline uint_fast8_t getRM(const AtlasInstPtr & inst, AtlasState* state)
    {
        uint64_t static_rm = inst->getRM();
        if (static_rm == 7) // RM field "DYN"
        {
            static_rm = READ_CSR_REG<XLEN>(state, FRM);
        }
        return static_rm;
    }

    template <typename SIZE>
    static void fmax_fmin_nan_zero_check(SIZE rs1_val, SIZE rs2_val, SIZE & rd_val, bool max)
    {
        static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);

        constexpr uint8_t SGN_BIT = sizeof(SIZE) * 8 - 1;
        constexpr uint8_t EXP_MSB = SGN_BIT - 1;
        constexpr uint8_t EXP_LSB = std::is_same<SIZE, DP>::value ? 52 : 23;
        constexpr uint8_t SIG_MSB = EXP_LSB - 1;

        constexpr SIZE EXP_MASK = (((SIZE)1 << (EXP_MSB - EXP_LSB + 1)) - 1) << EXP_LSB;
        constexpr SIZE SIG_MASK = ((SIZE)1 << (SIG_MSB + 1)) - 1;

        constexpr SIZE CAN_NAN = EXP_MASK | (SIZE)1 << SIG_MSB;
        constexpr SIZE NEG_ZERO = (SIZE)1 << SGN_BIT;
        constexpr SIZE POS_ZERO = 0;

        bool rs1_nan = ((rs1_val & EXP_MASK) == EXP_MASK) && (rs1_val & SIG_MASK);
        bool rs2_nan = ((rs2_val & EXP_MASK) == EXP_MASK) && (rs2_val & SIG_MASK);
        if (rs1_nan && rs2_nan)
        {
            rd_val = CAN_NAN;
        }
        else if (rs1_nan)
        {
            rd_val = rs2_val;
        }
        else if (rs2_nan)
        {
            rd_val = rs1_val;
        }

        if (((rs1_val == NEG_ZERO) && (rs2_val == POS_ZERO))
            || ((rs2_val == NEG_ZERO) && (rs1_val == POS_ZERO)))
        {
            rd_val = max ? POS_ZERO : NEG_ZERO;
        }
    }

    template <typename XLEN> static ActionGroup* update_csr(AtlasState* state)
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

    template <typename T> ActionGroup* compute_address_handler(AtlasState* state)
    {
        static_assert(std::is_same<T, RV64>::value || std::is_same<T, RV32>::value);

        const AtlasInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1Reg()->dmiRead<uint64_t>();
        constexpr uint32_t IMM_SIZE = 12;
        const T imm = inst->hasImmediate() ? inst->getSignExtendedImmediate<T, IMM_SIZE>() : 0;
        const T vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(T));
        return nullptr;
    }

    template <typename XLEN, typename SIZE, bool LOAD>
    ActionGroup* float_ls_handler(atlas::AtlasState* state)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);
        static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);
        static_assert(sizeof(XLEN) >= sizeof(SIZE));

        const AtlasInstPtr & inst = state->getCurrentInst();
        const XLEN paddr = state->getTranslationState()->getTranslationResult().getPaddr();

        if constexpr (LOAD)
        {
            WRITE_FP_REG<XLEN>(state, inst->getRd(), state->readMemory<SIZE>(paddr));
        }
        else
        {
            state->writeMemory<SIZE>(paddr, READ_FP_REG<XLEN>(state, inst->getRs2()));
        }
        return nullptr;
    }
} // namespace atlas