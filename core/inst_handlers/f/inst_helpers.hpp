#pragma once

#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"
#include "mavis/OpcodeInfo.h"

namespace atlas
{
    inline uint_fast8_t getRM(const AtlasInstPtr & inst)
    {
        uint64_t static_rm = inst->getRM();
        if (static_rm == 7) // RM field "DYN"
        {
            // TODO: use fcsr::frm
            return 0;
        }
        else
        {
            return static_rm;
        }
    }

    template <typename T> ActionGroup* compute_address_handler(AtlasState* state)
    {
        static_assert(std::is_same<T, RV64>::value || std::is_same<T, RV32>::value);

        const AtlasInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1()->dmiRead<uint64_t>();
        constexpr uint32_t IMM_SIZE = 12;
        const T imm = inst->hasImmediate() ? inst->getSignExtendedImmediate<T, IMM_SIZE>() : 0;
        const T vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(T));
        return nullptr;
    }

    using S = uint32_t;
    using D = uint64_t;

    template <typename RV, typename SIZE>
    ActionGroup* float_ls_handler(atlas::AtlasState* state, bool load)
    {
        static_assert(std::is_same<RV, RV64>::value || std::is_same<RV, RV32>::value);
        static_assert(std::is_same<SIZE, S>::value || std::is_same<SIZE, D>::value);
        static_assert(sizeof(RV) >= sizeof(SIZE));

        const AtlasInstPtr & inst = state->getCurrentInst();
        const RV paddr = state->getTranslationState()->getTranslationResult().getPaddr();

        if (load)
        {
            inst->getRd()->write(state->readMemory<SIZE>(paddr));
        }
        else
        {
            state->writeMemory<SIZE>(paddr, inst->getRs2()->dmiRead<uint64_t>());
        }
        return nullptr;
    }
} // namespace atlas