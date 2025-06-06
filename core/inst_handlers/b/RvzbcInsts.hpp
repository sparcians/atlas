#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RvzbcInsts
    {
        public:
            using base_type = RvzbcInsts;

            template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);
        
        private:

            template <typename XLEN>
            Action::ItrType clmulHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType clmulhHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType clmulrHandler(atlas::AtlasState* state, Action::ItrType action_it);
        
    };
} // namespace atlas