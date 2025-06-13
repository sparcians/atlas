#pragma once

#include "core/observers/Observer.hpp"
#include "stf_lib/stf-inc/stf_record_types.hpp"
#include "stf_lib/stf-inc/stf_writer.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{

    class STFLogger : public Observer
    {
      public:
        STFLogger(const uint32_t width, uint64_t pc, const std::string & filename);

        void initialize(const bool stf_enable, const uint32_t width, uint64_t pc);

        void writeInstruction(const AtlasInst* inst);


      private:
        std::unique_ptr<stf::STFWriter> stf_writer_;
        void postExecute_(AtlasState* state) override;
    };
} // namespace atlas