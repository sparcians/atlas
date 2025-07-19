#pragma once

#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class ActionTags
    {
      public:
        // Core Actions
        static const ActionTagType FETCH_TAG;
        static const ActionTagType INST_TRANSLATE_TAG;
        static const ActionTagType DECODE_TAG;
        static const ActionTagType EXECUTE_TAG;
        static const ActionTagType COMPUTE_ADDR_TAG;
        static const ActionTagType DATA_TRANSLATE_TAG;
        static const ActionTagType EXCEPTION_TAG;

        // Stop Simulation
        static const ActionTagType STOP_SIM_TAG;
    };
} // namespace pegasus
