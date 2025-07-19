#pragma once

#include "core/observers/Observer.hpp"
#include "stf-inc/stf_record_types.hpp"
#include "stf-inc/stf_writer.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    class STFLogger : public Observer
    {
      public:
        /*!
         * \class STFLogger
         * \brief Description...
         * \param reg_width Register width (32 or 64)
         * \param initial_pc Initial program counter
         * \param filename Name of the file the trace will be written to
         * \param state PegasusState used to populate initial register values
         */
        STFLogger(const uint32_t reg_width, uint64_t initial_pc, const std::string & filename,
                  PegasusState* state);

      private:
        stf::STFWriter stf_writer_;
        void postExecute_(PegasusState* state) override;
        // TODO: Add exception support
        void preException_(PegasusState* state) override;
        void preExecute_(PegasusState* state) override;
        void recordRegState_(PegasusState* state);
        void writeInstruction_(const PegasusInst* inst);
    };
} // namespace pegasus