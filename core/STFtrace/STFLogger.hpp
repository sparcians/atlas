#pragma once

#include "sparta/utils/SpartaAssert.hpp"
#include "sparta/utils/SpartaException.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "stf_lib/stf-inc/stf_record_types.hpp"
#include "stf_lib/stf-inc/stf_writer.hpp"
#include "core/AtlasInst.hpp"

namespace atlas{

    class STFLogger{
        public:
            /*class STFLoggerParameters : public sparta::ParameterSet{
            public:
                STFLoggerParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
            // stf_enable -> user can enable and disable STF Trace Generation
                PARAMETER(bool, stf_enable, false, "Do you want to enable STF Trace generation?")
            };*/
            stf::STFWriter* stf_writer_;
            STFLogger();
            ~STFLogger();

            void initialize(const bool stf_enable, const uint32_t width, uint64_t pc);

            void writeInstruction(const AtlasInst* inst);


        private:
            bool enable;
    };
}