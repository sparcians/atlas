
#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    // CONSTRUCTORS

    // stf_enable -> user can enable and disable STF Trace Generation
    // width -> register width (32 or 64)
    // pc -> initial program counter
    // filename -> name of the file the trace will be written to
    STFLogger::STFLogger(const uint32_t width, uint64_t pc, const std::string & filename, AtlasState* state) :
        Observer((width == 32) ? ObserverMode::RV32 : ObserverMode::RV64),
        stf_writer_(std::make_unique<stf::STFWriter>())
    {
        // set up version and stf generation type
        stf_writer_->open(filename);
        stf_writer_->addTraceInfo(
            stf::TraceInfoRecord(stf::STF_GEN::STF_TRANSACTION_EXAMPLE, 0, 0, 0, "Trace from Atlas"));

        if (width == 32)
        {

            stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }
        else
        {
            stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
            stf_writer_->setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }

        stf_writer_->setISA(stf::ISA::RISCV);
        stf_writer_->setHeaderPC(pc);
        stf_writer_->finalizeHeader();
        (void) state; // Unused parameter, but will be implemented when recording inital state of registers
        //recordRegState_(state); record inital state of registers
    }

    // METHODS
    void STFLogger::writeInstruction(const AtlasInst* inst)
    {
        if (inst->getOpcodeSize() == 2)
        {
            *stf_writer_ << stf::InstOpcode16Record(inst->getOpcode());
        }
        else if (inst->getOpcodeSize() == 4)
        {
            *stf_writer_ << stf::InstOpcode32Record(inst->getOpcode());
        }
    }

    // state -> current AtlasState to write instruction record
    void STFLogger::postExecute_(AtlasState* state) 
    {
        //write opcode record
        writeInstruction(state->getCurrentInst().get());
    }

    void STFLogger::preExecute_(AtlasState* state) 
    {
        (void)state;
    }

   void STFLogger::preException_(AtlasState* state)
    {
        (void)state;
    }

    void STFLogger::recordRegState_(AtlasState* state){
        (void)state;
    }
} // namespace atlas