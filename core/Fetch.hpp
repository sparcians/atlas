#pragma once

#include "core/ActionGroup.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace pegasus
{
    class PegasusState;

    class Fetch : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Fetch";
        using base_type = Fetch;

        class FetchParameters : public sparta::ParameterSet
        {
          public:
            FetchParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
        };

        Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p);

        ActionGroup* getActionGroup() { return &fetch_action_group_; }

      private:
        PegasusState* state_ = nullptr;

        void onBindTreeEarly_() override;

        Action::ItrType fetch_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup fetch_action_group_{"Fetch"};

        Action::ItrType decode_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup decode_action_group_{"Decode"};

        void advanceSim_();
    };
} // namespace pegasus
