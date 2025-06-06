#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RvzbsInsts
    {
        public:
            using base_type = RvzbsInsts;

            template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);
        
        private:
            template <typename XLEN>
            Action::ItrType bclrHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType bclriHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType bextHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType bextiHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType binvHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType binviHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType bsetHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType bsetiHandler(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas