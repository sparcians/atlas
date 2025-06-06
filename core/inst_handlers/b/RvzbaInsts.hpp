#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RvzbaInsts
    {
      public:
        using base_type = RvzbaInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        Action::ItrType add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
        
        //sh1add and sh1add.uw
        template <typename XLEN>
        Action::ItrType sh1add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh1add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
        
        template <typename XLEN>
        Action::ItrType sh2add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh2add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType sh3add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh3add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);

        Action::ItrType slli_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas