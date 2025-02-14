#pragma once

#include "core/inst_handlers/inst_helpers.hpp"
#include "include/CSRNums.hpp"
#include "core/IntNums.hpp"
#include "core/FpNums.hpp"
#include "core/VecNums.hpp"
#include "include/CSRHelpers.hpp"
#include <utility>
#include <vector>
#include <unordered_map>
#include <string>

#define READ_INT_REG32(reg_ident)                                                                  \
    (reg_ident == 0) ? 0 : state->getIntRegister(reg_ident)->dmiRead<uint32_t>()

#define READ_INT_REG64(reg_ident)                                                                  \
    (reg_ident == 0) ? 0 : state->getIntRegister(reg_ident)->dmiRead<uint64_t>()

#define READ_INT_REG(reg_ident)                                                                    \
    (reg_ident == 0) ? 0 : state->getIntRegister(reg_ident)->dmiRead<XLEN>()

#define WRITE_INT_REG32(reg_ident, reg_value)                                                      \
    if (reg_ident != 0)                                                                            \
        state->getIntRegister(reg_ident)->dmiWrite<uint32_t>(reg_value);

#define WRITE_INT_REG64(reg_ident, reg_value)                                                      \
    if (reg_ident != 0)                                                                            \
        state->getIntRegister(reg_ident)->dmiWrite<uint64_t>(reg_value);

#define WRITE_INT_REG(reg_ident, reg_value)                                                        \
    if (reg_ident != 0)                                                                            \
        state->getIntRegister(reg_ident)->dmiWrite<XLEN>(reg_value);

#define READ_FP_REG32(reg_ident) state->getFpRegister(reg_ident)->dmiRead<uint32_t>()

#define READ_FP_REG64(reg_ident) state->getFpRegister(reg_ident)->dmiRead<uint64_t>()

#define READ_FP_REG(reg_ident) state->getFpRegister(reg_ident)->dmiRead<XLEN>()

#define WRITE_FP_REG32(reg_ident, reg_value)                                                       \
    state->getFpRegister(reg_ident)->dmiWrite<uint32_t>(reg_value);

#define WRITE_FP_REG64(reg_ident, reg_value)                                                       \
    state->getFpRegister(reg_ident)->dmiWrite<uint64_t>(reg_value);

#define WRITE_FP_REG(reg_ident, reg_value)                                                         \
    state->getFpRegister(reg_ident)->dmiWrite<XLEN>(reg_value);

#define READ_VEC_REG(reg_ident) state->getVecRegister(reg_ident)->dmiRead<uint64_t>()

#define WRITE_VEC_REG(reg_ident, reg_value) state->getVecRegister(reg_ident)->dmiWrite(reg_value);

#define READ_CSR_REG(reg_ident) state->getCsrRegister(reg_ident)->dmiRead<uint64_t>()

#define WRITE_CSR_REG(reg_ident, reg_value)                                                        \
    if (atlas::getCsrBitMask(reg_ident) != 0xffffffffffffffff)                                     \
    {                                                                                              \
        auto reg = state->getCsrRegister(reg_ident);                                               \
        const auto old_value = reg->dmiRead<uint64_t>();                                           \
        const auto mask = atlas::getCsrBitMask(reg_ident);                                         \
        const auto write_val = (old_value & ~mask) | (reg_value & mask);                           \
        reg->dmiWrite(write_val);                                                                  \
    }                                                                                              \
    else                                                                                           \
        state->getCsrRegister(reg_ident)->dmiWrite(reg_value);

#define READ_CSR_FIELD(reg_ident, field_name)                                                      \
    ((state->getCsrRegister(reg_ident)->dmiRead<uint64_t>()                                        \
      >> atlas::getCsrBitRange(reg_ident, #field_name).first)                                      \
     & ((1ULL << (atlas::getCsrBitRange(reg_ident, #field_name).second                             \
                  - atlas::getCsrBitRange(reg_ident, #field_name).first + 1))                      \
        - 1))

#define WRITE_CSR_FIELD(reg_ident, field_name, field_value)                                        \
    {                                                                                              \
        auto csr_value = state->getCsrRegister(reg_ident)->dmiRead<uint64_t>();                    \
                                                                                                   \
        const auto low_bit = atlas::getCsrBitRange(reg_ident, #field_name).first;                  \
        const auto high_bit = atlas::getCsrBitRange(reg_ident, #field_name).second;                \
        const auto mask = ((1ULL << (high_bit - low_bit + 1)) - 1) << low_bit;                     \
        csr_value &= ~mask;                                                                        \
                                                                                                   \
        const uint64_t new_field_value = field_value << low_bit;                                   \
        csr_value |= new_field_value;                                                              \
                                                                                                   \
        state->getCsrRegister(reg_ident)->write(csr_value);                                        \
    }

#define PEEK_CSR_REG(reg_ident) READ_CSR_REG(reg_ident)

#define POKE_CSR_REG(reg_ident, reg_value) state->getCsrRegister(reg_ident)->dmiWrite(reg_value);

#define POKE_CSR_FIELD(reg_ident, field_name, field_value)                                         \
    {                                                                                              \
        auto csr_value = state->getCsrRegister(reg_ident)->dmiRead<uint64_t>();                    \
                                                                                                   \
        const auto low_bit = atlas::getCsrBitRange(reg_ident, #field_name).first;                  \
        const auto high_bit = atlas::getCsrBitRange(reg_ident, #field_name).second;                \
        const auto mask = ((1ULL << (high_bit - low_bit + 1)) - 1) << low_bit;                     \
        csr_value &= ~mask;                                                                        \
                                                                                                   \
        const uint64_t new_field_value = (uint64_t)field_value << low_bit;                         \
        csr_value |= new_field_value;                                                              \
                                                                                                   \
        state->getCsrRegister(reg_ident)->dmiWrite(csr_value);                                     \
    }
