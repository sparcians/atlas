#pragma once

#include <cinttypes>
#include <string>

#define ONE_BIT_MASK 0x1
#define TWO_BIT_MASK 0x3
#define TEN_BIT_MASK 0x3FF    // used for PPN0 for SV32
#define TWELVE_BIT_MASK 0xFFF // used for PPN1 for SV32

#define PTE_D_MASK 0x80
#define PTE_A_MASK 0x40
#define PTE_G_MASK 0x20
#define PTE_U_MASK 0x10
#define PTE_X_MASK 0x8
#define PTE_W_MASK 0x4
#define PTE_R_MASK 0x2
#define PTE_V_MASK 0x1

#define PT_ENTRIES_SV32 1024 // Sv32 has 1,024 entries in a page table
#define PT_ENTRIES_SV39 512  // Sv39 has 512 entries in a page table

namespace atlas
{
    using Addr = uint64_t;
    using Opcode = uint32_t;
    using OpcodeSize = size_t;
    using HartId = uint32_t;
    using ExcpCode = uint64_t;

    using ActionTagType = uint32_t;

    using RV32 = uint32_t;
    using RV64 = uint64_t;

    using SP = uint32_t; // Single Precision
    using DP = uint64_t; // Double Precision

    enum class InstType
    {
        SCALAR,         //! Scalar (integer)
        FLOATING_POINT, //! Floating point
        VECTOR,         //! Vector
        BRANCH,         //! Branch
        LOAD,           //! Load
        STORE,          //! Store
        STORECOND,      //! Atomic store conditional
        MISC,           //! Miscellaneous (csr, ecall, wfi, etc.)
        INVALID         //! Invalid
    };

    enum class RegType
    {
        INTEGER,
        FLOATING_POINT,
        VECTOR,
        CSR,
        INVALID
    };

    enum class MemAccessSource
    {
        FETCH,       //! Memory access from fetching an instruction
        INSTRUCTION, //! Memory access made by an instruction (load or store)
        HARDWARE     //! Memory access made by hardware (e.g. page table walk)
    };

    enum class PrivMode
    {
        USER,
        SUPERVISOR,
        // HYPERVISOR,
        MACHINE = 3,
        // VIRTUAL_USER,
        // VIRTUAL_SUPERVISOR
        INVALID
    };

    enum class ExcpType
    {
        FAULT,       //! Trap caused by unexpected program behavior
        SYSTEM_CALL, //! Trap caused by an ecall instruction
        INTERRUPT,   //! Asynchronous exception
        INVALID
    };

    struct RegId
    {
        RegType reg_type;
        uint32_t reg_num;
        std::string reg_name;
    };

    enum class MMUMode
    {
        SV32,
        SV39,
        SV48
    };

    // Common opcodes
    constexpr uint64_t WFI_OPCODE = 0x10500073;
    constexpr uint64_t NOP_OPCODE = 0x00000013;
} // namespace atlas
