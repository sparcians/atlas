#pragma once

#include <cinttypes>
#include <string>

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
        USER = 0,
        SUPERVISOR = 1,
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

    enum class MMUMode : uint32_t
    {
        BAREMETAL,
        SV32,
        SV39,
        SV48,
        SV57,
        INVALID
    };

    static constexpr uint32_t N_MMU_MODES = static_cast<uint32_t>(MMUMode::INVALID);

    // Common opcodes
    constexpr uint64_t WFI_OPCODE = 0x10500073;
    constexpr uint64_t NOP_OPCODE = 0x00000013;
} // namespace atlas
