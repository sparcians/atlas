#pragma once

#include "core/observers/Observer.hpp"
#include "stf-inc/stf_record_types.hpp"
#include "stf-inc/stf_writer.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
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
        * \param state AtlasState used to populate initial register values
        */
        STFLogger(const uint32_t reg_width, uint64_t initial_pc, const std::string & filename,
                    AtlasState* state);

      private:
        stf::STFWriter stf_writer_;
        void postExecute_(AtlasState* state) override;
        // TODO: Add exception support
        void preException_(AtlasState* state) override;
        void preExecute_(AtlasState* state) override;
        void recordRegState_(AtlasState* state);
        void writeInstruction_(const AtlasInst* inst);
    };
} // namespace atlas