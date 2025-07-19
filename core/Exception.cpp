#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/PegasusState.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace pegasus
{

    Exception::Exception(sparta::TreeNode* exception_node, const ExceptionParameters*) :
        sparta::Unit(exception_node)
    {
        rv32_exception_action_ = pegasus::Action::createAction<&Exception::handleException_<RV32>>(
            this, "exception", ActionTags::EXCEPTION_TAG);
        rv64_exception_action_ = pegasus::Action::createAction<&Exception::handleException_<RV64>>(
            this, "exception", ActionTags::EXCEPTION_TAG);
    }

    void Exception::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        PegasusState* state = core_tn->getResourceAs<PegasusState>();

        const auto xlen = state->getXlen();
        if (xlen == 64)
        {
            exception_action_group_.addAction(rv64_exception_action_);
        }
        else
        {
            exception_action_group_.addAction(rv32_exception_action_);
        }

        // Connect exception ActionGroup to instruction finish ActionGroup
        exception_action_group_.setNextActionGroup(state->getFinishActionGroup());
    }

    template <typename XLEN>
    Action::ItrType Exception::handleException_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        sparta_assert(fault_cause_.isValid() || interrupt_cause_.isValid(),
                      "Exception cause is not valid!");
        sparta_assert(fault_cause_.isValid() != interrupt_cause_.isValid(),
                      "Fault and interrupt cause cannot both be valid!");

        const bool is_interrupt = interrupt_cause_.isValid();
        const uint64_t excp_code = is_interrupt ? static_cast<XLEN>(interrupt_cause_.getValue())
                                                : static_cast<XLEN>(fault_cause_.getValue());

        // Determine which privilege mode to handle the trap in
        const uint32_t trap_deleg_csr = is_interrupt ? MIDELEG : MEDELEG;
        const XLEN trap_deleg_val = READ_CSR_REG<XLEN>(state, trap_deleg_csr);
        const PrivMode priv_mode =
            ((1ull << (excp_code)) & trap_deleg_val) ? PrivMode::SUPERVISOR : PrivMode::MACHINE;

        const bool prev_virt_mode = state->getVirtualMode();

        if (priv_mode == PrivMode::SUPERVISOR)
        {
            // Set next PC
            const XLEN trap_handler_address = (READ_CSR_REG<XLEN>(state, STVEC) & ~(XLEN)1);
            state->setNextPc(trap_handler_address);

            // Get PC that caused the exception
            const XLEN epc_val = state->getPc();
            WRITE_CSR_REG<XLEN>(state, SEPC, epc_val);

            // Get the exception code, set upper bit for interrupts
            const XLEN interrupt_bit = 1 << ((sizeof(XLEN) * 4) - 1);
            const XLEN cause_val = is_interrupt ? (excp_code & interrupt_bit) : excp_code;
            WRITE_CSR_REG<XLEN>(state, SCAUSE, cause_val);

            // Depending on the exception type, get the trap value
            const uint64_t trap_val = is_interrupt
                                          ? determineTrapValue_(interrupt_cause_.getValue(), state)
                                          : determineTrapValue_(fault_cause_.getValue(), state);
            WRITE_CSR_REG<XLEN>(state, STVAL, trap_val);

            // Update MSTATUS
            const auto mstatus_sie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "sie");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spie", mstatus_sie);

            const auto spp_val = static_cast<XLEN>(state->getPrivMode());
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spp", spp_val);

            const uint64_t sie_val = 0;
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sie", sie_val);
        }
        else if (priv_mode == PrivMode::MACHINE)
        {
            // Set next PC
            const XLEN trap_handler_address = (READ_CSR_REG<XLEN>(state, MTVEC) & ~(XLEN)1);
            state->setNextPc(trap_handler_address);

            // Get PC that caused the exception
            const XLEN epc_val = state->getPc();
            WRITE_CSR_REG<XLEN>(state, MEPC, epc_val);

            // Get the exception code, set upper bit for interrupts
            const XLEN interrupt_bit = 1 << ((sizeof(XLEN) * 4) - 1);
            const XLEN cause_val = is_interrupt ? (excp_code & interrupt_bit) : excp_code;
            WRITE_CSR_REG<XLEN>(state, MCAUSE, cause_val);

            // Depending on the exception type, get the trap value
            const uint64_t trap_val = is_interrupt
                                          ? determineTrapValue_(interrupt_cause_.getValue(), state)
                                          : determineTrapValue_(fault_cause_.getValue(), state);
            WRITE_CSR_REG<XLEN>(state, MTVAL, trap_val);

            // Update MSTATUS
            const auto mstatus_mie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "mie");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpie", mstatus_mie);

            const auto mpp_val = static_cast<XLEN>(state->getPrivMode());
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpp", mpp_val);

            const uint64_t mie_val = 0;
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mie", mie_val);

            const uint64_t mpv_val = 0;
            const uint64_t gva_val = 0;
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpv", mpv_val);
                WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "gva", gva_val);
            }
            else
            {
                WRITE_CSR_FIELD<XLEN>(state, MSTATUSH, "mpv", mpv_val);
                WRITE_CSR_FIELD<XLEN>(state, MSTATUSH, "gva", gva_val);
            }
        }
        state->setPrivMode(priv_mode, prev_virt_mode);
        state->changeMMUMode<XLEN>();

        fault_cause_.clearValid();
        interrupt_cause_.clearValid();
        return ++action_it;
    }

    uint64_t Exception::determineTrapValue_(const FaultCause & cause, PegasusState* state)
    {
        switch (cause)
        {
            case FaultCause::INST_ADDR_MISALIGNED:
            case FaultCause::INST_ACCESS:
            case FaultCause::INST_PAGE_FAULT:
                {
                    const auto request = state->getFetchTranslationState()->getRequest();
                    return (request.getVAddr() + request.getMisalignedBytes());
                }
            case FaultCause::LOAD_ADDR_MISALIGNED:
            case FaultCause::LOAD_ACCESS:
            case FaultCause::STORE_AMO_ADDR_MISALIGNED:
            case FaultCause::STORE_AMO_ACCESS:
            case FaultCause::LOAD_PAGE_FAULT:
            case FaultCause::STORE_AMO_PAGE_FAULT:
                {
                    const auto vaddr_val =
                        state->getCurrentInst()->getTranslationState()->getRequest().getVAddr();
                    return vaddr_val;
                }
            case FaultCause::ILLEGAL_INST:
                return state->getSimState()->current_opcode;
            case FaultCause::BREAKPOINT:
            case FaultCause::USER_ECALL:
            case FaultCause::SUPERVISOR_ECALL:
            case FaultCause::MACHINE_ECALL:
            case FaultCause::SOFTWARE_CHECK:
            case FaultCause::HARDWARE_ERROR:
                return 0;
        }
        return 0;
    }

    std::ostream & operator<<(std::ostream & os, const FaultCause & cause)
    {
        switch (cause)
        {
            case FaultCause::INST_ADDR_MISALIGNED:
                os << "INST_ADDR_MISALIGNED";
                break;
            case FaultCause::INST_ACCESS:
                os << "INST_ACCESS";
                break;
            case FaultCause::INST_PAGE_FAULT:
                os << "INST_PAGE_FAULT";
                break;
            case FaultCause::LOAD_ADDR_MISALIGNED:
                os << "LOAD_ADDR_MISALIGNED";
                break;
            case FaultCause::LOAD_ACCESS:
                os << "LOAD_ACCESS";
                break;
            case FaultCause::STORE_AMO_ADDR_MISALIGNED:
                os << "STORE_AMO_ADDR_MISALIGNED";
                break;
            case FaultCause::STORE_AMO_ACCESS:
                os << "STORE_AMO_ACCESS";
                break;
            case FaultCause::LOAD_PAGE_FAULT:
                os << "LOAD_PAGE_FAULT";
                break;
            case FaultCause::STORE_AMO_PAGE_FAULT:
                os << "STORE_AMO_PAGE_FAULT";
                break;
            case FaultCause::ILLEGAL_INST:
                os << "ILLEGAL_INST";
                break;
            case FaultCause::BREAKPOINT:
                os << "BREAKPOINT";
                break;
            case FaultCause::USER_ECALL:
                os << "USER_ECALL";
                break;
            case FaultCause::SUPERVISOR_ECALL:
                os << "SUPERVISOR_ECALL";
                break;
            case FaultCause::MACHINE_ECALL:
                os << "MACHINE_ECALL";
                break;
            case FaultCause::SOFTWARE_CHECK:
                os << "SOFTWARE_CHECK";
                break;
            case FaultCause::HARDWARE_ERROR:
                os << "HARDWARE_ERROR";
                break;
        }
        return os;
    }

    uint64_t Exception::determineTrapValue_(const InterruptCause & cause, PegasusState*)
    {
        switch (cause)
        {
            case InterruptCause::SUPERVISOR_SOFTWARE:
            case InterruptCause::MACHINE_SOFTWARE:
            case InterruptCause::SUPERVISOR_TIMER:
            case InterruptCause::MACHINE_TIMER:
            case InterruptCause::SUPERVISOR_EXTERNAL:
            case InterruptCause::MACHINE_EXTERNAL:
            case InterruptCause::COUNTER_OVERFLOW:
                return 0;
        }
        return 0;
    }
} // namespace pegasus
