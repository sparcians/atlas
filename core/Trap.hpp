#pragma once

#include "ActionGroup.hpp"

#include <cstdint>
#include <iostream>

namespace pegasus
{
    enum class FaultCause : uint64_t
    {
        INST_ADDR_MISALIGNED = 0x0,
        INST_ACCESS = 0x1,
        ILLEGAL_INST = 0x2,
        BREAKPOINT = 0x3,
        LOAD_ADDR_MISALIGNED = 0x4,
        LOAD_ACCESS = 0x5,
        STORE_AMO_ADDR_MISALIGNED = 0x6,
        STORE_AMO_ACCESS = 0x7,
        USER_ECALL = 0x8,
        SUPERVISOR_ECALL = 0x9,
        MACHINE_ECALL = 0xb,
        INST_PAGE_FAULT = 0xc,
        LOAD_PAGE_FAULT = 0xd,
        STORE_AMO_PAGE_FAULT = 0xf,
        SOFTWARE_CHECK = 0x12,
        HARDWARE_ERROR = 0x13,
    };

    enum class InterruptCause : uint64_t
    {
        SUPERVISOR_SOFTWARE = 0x1,
        MACHINE_SOFTWARE = 0x3,
        SUPERVISOR_TIMER = 0x5,
        MACHINE_TIMER = 0x7,
        SUPERVISOR_EXTERNAL = 0x9,
        MACHINE_EXTERNAL = 0xb,
        COUNTER_OVERFLOW = 0xd,
    };

    // Defined in Exception.cpp
    std::ostream & operator<<(std::ostream & os, const FaultCause & cause);

} // namespace pegasus

#include "core/Exception.hpp"

#define TRAP_IMPL(cause)                                                                           \
    {                                                                                              \
        auto exception_unit = state->getExceptionUnit();                                           \
        exception_unit->setUnhandledException(cause);                                              \
        throw ActionException(exception_unit->getActionGroup());                                   \
    }

#define THROW_MISALIGNED_FETCH TRAP_IMPL(FaultCause::INST_ADDR_MISALIGNED)
#define THROW_FETCH_ACCESS TRAP_IMPL(FaultCause::INST_ACCESS)
#define THROW_ILLEGAL_INST TRAP_IMPL(FaultCause::ILLEGAL_INST)
#define THROW_BREAKPOINT TRAP_IMPL(FaultCause::BREAKPOINT)
#define THROW_MISALIGNED_LOAD TRAP_IMPL(FaultCause::LOAD_ADDR_MISALIGNED)
#define THROW_LOAD_ACCESS TRAP_IMPL(FaultCause::LOAD_ACCESS)
#define THROW_MISALIGNED_STORE_AMO TRAP_IMPL(FaultCause::STORE_AMO_ADDR_MISALIGNED)
#define THROW_STORE_AMO_ACCESS TRAP_IMPL(FaultCause::STORE_AMO_ACCESS)
#define THROW_USER_ECALL TRAP_IMPL(FaultCause::USER_ECALL)
#define THROW_SUPERVISOR_ECALL TRAP_IMPL(FaultCause::SUPERVISOR_ECALL)
#define THROW_MACHINE_ECALL TRAP_IMPL(FaultCause::MACHINE_ECALL)
#define THROW_FETCH_PAGE_FAULT TRAP_IMPL(FaultCause::INST_PAGE_FAULT)
#define THROW_LOAD_PAGE_FAULT TRAP_IMPL(FaultCause::LOAD_PAGE_FAULT)
#define THROW_STORE_AMO_PAGE_FAULT TRAP_IMPL(FaultCause::STORE_AMO_PAGE_FAULT)
#define THROW_SOFTWARE_CHECK_FAULT TRAP_IMPL(FaultCause::SOFTWARE_CHECK)
#define THROW_HARDWARE_ERROR_FAULT TRAP_IMPL(FaultCause::HARDWARE_ERROR)
