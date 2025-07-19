#pragma once

#include <cinttypes>
#include <string>
#include <ostream>
#include <array>

namespace pegasus
{
    using Addr = uint64_t;
    using Opcode = uint32_t;
    using OpcodeSize = size_t;
    using HartId = uint32_t;
    using ExcpCode = uint64_t;

    using ActionTagType = uint32_t;

    using RV32 = uint32_t;
    using RV64 = uint64_t;

    using W = uint32_t;
    using D = uint64_t;

    using SP = uint32_t;
    using DP = uint64_t;

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

    enum class PageSize : uint32_t
    {
        SIZE_4K,
        SIZE_2M,   // Megapage (Sv39, Sv48, Sv57)
        SIZE_4M,   // Megapage (Sv32 only)
        SIZE_1G,   // Gigapage (Sv39, Sv48, Sv57)
        SIZE_512G, // Terapage (Sv48, Sv57)
        SIZE_256T, // Petapage (Sv57)
        INVALID
    };

    static constexpr uint32_t N_MMU_MODES = static_cast<uint32_t>(MMUMode::INVALID);

    inline std::ostream & operator<<(std::ostream & os, const MMUMode mode)
    {
        switch (mode)
        {
            case MMUMode::BAREMETAL:
                os << "BAREMETAL";
                break;
            case MMUMode::SV32:
                os << "SV32";
                break;
            case MMUMode::SV39:
                os << "SV39";
                break;
            case MMUMode::SV48:
                os << "SV48";
                break;
            case MMUMode::SV57:
                os << "SV57";
                break;
            case MMUMode::INVALID:
                os << "INVALID";
                break;
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const PageSize page_size)
    {
        switch (page_size)
        {
            case PageSize::SIZE_4K:
                os << "4K";
                break;
            case PageSize::SIZE_2M:
                os << "2M";
                break;
            case PageSize::SIZE_4M:
                os << "4M";
                break;
            case PageSize::SIZE_1G:
                os << "1G";
                break;
            case PageSize::SIZE_512G:
                os << "512G";
                break;
            case PageSize::SIZE_256T:
                os << "256T";
                break;
            case PageSize::INVALID:
                os << "INVALID";
                break;
        }
        return os;
    }

    // Common opcodes
    constexpr uint64_t WFI_OPCODE = 0x10500073;
    constexpr uint64_t NOP_OPCODE = 0x00000013;

    // System Call emulation
    using SystemCallStack = std::array<uint64_t, 8>;

} // namespace pegasus
