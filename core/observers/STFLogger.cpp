#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"

namespace pegasus
{
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         PegasusState* state) :
        Observer((reg_width == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        // set up version and stf generation type
        stf_writer_.open(filename);
        stf_writer_.addTraceInfo(stf::TraceInfoRecord(stf::STF_GEN::STF_TRANSACTION_EXAMPLE, 0, 0,
                                                      0, "Trace from Pegasus"));

        if (reg_width == 64)
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
            stf_writer_.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }
        else
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }

        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
        (void)state; // Unused parameter, but will be implemented when recording inital state of
                     // registers
        // recordRegState_(state); record inital state of registers
    }

    void STFLogger::writeInstruction_(const PegasusInst* inst)
    {
        if (fault_cause_.isValid() || interrupt_cause_.isValid())
        {
            return;
        }

        if (inst->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(inst->getOpcode());
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(inst->getOpcode());
        }
    }

    void STFLogger::postExecute_(PegasusState* state)
    {
        // write opcode record
        writeInstruction_(state->getCurrentInst().get());
    }

    void STFLogger::preExecute_(PegasusState* state) { (void)state; }

    void STFLogger::preException_(PegasusState* state) { (void)state; }

    void STFLogger::recordRegState_(PegasusState* state) { (void)state; }
} // namespace pegasus