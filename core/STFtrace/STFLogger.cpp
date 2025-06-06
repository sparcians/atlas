
#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/AtlasState.hpp"

namespace atlas{
// CONSTRUCTORS 
    STFLogger::STFLogger(){
        stf_writer_ = new stf::STFWriter();
    } 

// DESTRUCTORS
    STFLogger::~STFLogger(){
        if(enable)  stf_writer_->close();
        delete stf_writer_;
        stf_writer_ = nullptr;
        enable = false;
    }

//METHODS
    //stf_enable -> user can enable and disable STF Trace Generation
    //width -> register width (32 or 64)
    //pc -> initial program counter
    void STFLogger::initialize(const bool stf_enable, const uint32_t width, uint64_t pc){
        enable = stf_enable;
        if (!enable) return;
        stf_writer_->open("STFTrace.stf");

        //set up version and stf generation type
        stf_writer_->addTraceInfo(stf::TraceInfoRecord(stf::STF_GEN::STF_GEN_IMPERAS,
                                                        1, 2, 0, "Trace from Imperas")); 

        if(width == 32)  stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        else{
            stf_writer_->setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64); 
            stf_writer_->setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }

        stf_writer_->setISA(stf::ISA::RISCV);

        std::cout << "STFLogger fully init" << std::endl;
        stf_writer_->setHeaderPC(pc);
        stf_writer_->finalizeHeader();
    }

    //inst -> current instruction being executed (will add register records)
    void STFLogger::writeInstruction(const AtlasInst* inst){
        if (!enable) return;
        if (inst->getOpcodeSize() == 2 ) *stf_writer_ << stf::InstOpcode16Record(inst->getOpcode());
        else if (inst->getOpcodeSize() == 4)  *stf_writer_ << stf::InstOpcode32Record(inst->getOpcode());
    }

}