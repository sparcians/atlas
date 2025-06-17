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
        STFLogger(const uint32_t width, uint64_t pc, const std::string & filename, AtlasState* state);

        void initialize(const bool stf_enable, const uint32_t width, uint64_t pc);

        void writeInstruction(const AtlasInst* inst);


      private:
        std::unique_ptr<stf::STFWriter> stf_writer_;
        void postExecute_(AtlasState* state) override;
        // TODO: Add exception support
        void preException_(AtlasState* state) override;
        void preExecute_(AtlasState* state) override;
        void recordRegState_(AtlasState* state);
    };
} // namespace atlas