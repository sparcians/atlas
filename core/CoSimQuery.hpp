#pragma once

namespace pegasus
{

    // This base class is used to query the expected register values from the
    // attached co-simulation e.g. Imperas, Spike, etc.
    //
    // It is used to optionally add expected input/output values to each instruction
    // for Python implementations' quick evaluation in the Pegasus IDE.
    //
    // Subclasses could be found external to the core Pegasus codebase, such as
    // a separate repo of yours that has a license to Imperas.
    class CoSimQuery
    {
      public:
        virtual ~CoSimQuery() = default;

        virtual uint32_t getNumIntRegisters() const = 0;
        virtual uint64_t getIntRegValue(uint64_t hart, int reg_id) const = 0;

        virtual uint32_t getNumFpRegisters() const = 0;
        virtual uint64_t getFpRegValue(uint64_t hart, int reg_id) const = 0;

        virtual uint32_t getNumVecRegisters() const = 0;
        virtual uint64_t getVecRegValue(uint64_t hart, int reg_id) const = 0;

        virtual bool isCsrImplemented(const std::string & csr_name) const = 0;
        virtual uint64_t getCsrRegValue(uint64_t hart, const std::string & csr_name) const = 0;

        virtual uint64_t getExpectedPC(uint64_t hart) const = 0;
    };

} // namespace pegasus
