#pragma once

#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"
#include "mavis/OpcodeInfo.h"

namespace atlas
{
<<<<<<< HEAD
    constexpr uint64_t rv64_l32_mask = 0x0000FFFF;
    
    inline uint_fast8_t getRM(const AtlasInstPtr & inst)
    {
        const uint64_t static_rm = inst->getRM();
=======
    uint_fast8_t getRM(const AtlasInstPtr & inst)
    {
        uint64_t static_rm = inst->getRM();
>>>>>>> ba8f11c (add f extension)
        if (static_rm == 7) // RM field "DYN"
        {
            // TODO: use fcsr::frm
            return 0;
        }
<<<<<<< HEAD
        return static_rm;
=======
        else
        {
            return static_rm;
        }
>>>>>>> ba8f11c (add f extension)
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

<<<<<<< HEAD
    template <typename RV, typename SIZE, bool LOAD>
    ActionGroup* float_ls_handler(atlas::AtlasState* state)
    {
        static_assert(std::is_same<RV, RV64>::value || std::is_same<RV, RV32>::value);
        static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);
=======
    using S = uint32_t;
    using D = uint64_t;

    template <typename RV, typename SIZE>
    ActionGroup* float_ls_handler(atlas::AtlasState* state, bool load)
    {
        static_assert(std::is_same<RV, RV64>::value || std::is_same<RV, RV32>::value);
        static_assert(std::is_same<SIZE, S>::value || std::is_same<SIZE, D>::value);
>>>>>>> ba8f11c (add f extension)
        static_assert(sizeof(RV) >= sizeof(SIZE));

        const AtlasInstPtr & inst = state->getCurrentInst();
        const RV paddr = state->getTranslationState()->getTranslationResult().getPaddr();

<<<<<<< HEAD
        if constexpr (LOAD)
=======
        if (load)
>>>>>>> ba8f11c (add f extension)
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