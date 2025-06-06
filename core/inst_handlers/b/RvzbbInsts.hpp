#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RvzbbInsts
    {
        public:
           using base_type = RvzbbInsts;

            template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

        private:
         
            template <typename XLEN>
            Action::ItrType andnHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType clzHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType clzwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType cpopHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType cpopwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType ctzHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType ctzwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType maxHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType maxuHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType minHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType minuHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType orc_bHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType ornHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType rev8Handler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType rolHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType rolwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType rorHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType roriHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType roriwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            Action::ItrType rorwHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType sext_bHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType sext_hHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType xnorHandler(atlas::AtlasState* state, Action::ItrType action_it);
            template <typename XLEN>
            Action::ItrType zext_hHandler(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas