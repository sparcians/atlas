#include "stf-inc/stf_record_types.hpp"
#include "stf-inc/stf_writer.hpp"

// This test is taken directly from the sparcians/stf_lib repo's unit tests.
// It is really only here to show some example usage of the stf_lib library, which is
// a submodule of Pegasus.

int main()
{
    uint64_t pc = 0x1000;

    stf::STFWriter stf_writer;

    stf_writer.open("stf_write_test.zstf");
    stf_writer.addTraceInfo(
        stf::TraceInfoRecord(stf::STF_GEN::STF_GEN_DROMAJO, 1, 2, 0, "Trace from Dromajo"));
    stf_writer.setISA(stf::ISA::RISCV);
    stf_writer.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
    stf_writer.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
    stf_writer.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_PHYSICAL_ADDRESS);
    stf_writer.setHeaderPC(pc);
    stf_writer.finalizeHeader();
    stf_writer << stf::InstOpcode32Record(0x00b60733);
    stf_writer.close();

    return 0;
}
