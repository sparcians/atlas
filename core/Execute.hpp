#pragma once

#include "core/ActionGroup.hpp"
#include "include/PegasusTypes.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace pegasus
{
    class PegasusState;

    class Execute : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Execute";
        using base_type = Execute;

        class ExecuteParameters : public sparta::ParameterSet
        {
          public:
            ExecuteParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(bool, enable_syscall_emulation, false, "ecalls will be emulated");
        };

        Execute(sparta::TreeNode* execute_node, const ExecuteParameters* p);

        ActionGroup* getActionGroup() { return &execute_action_group_; }

        bool getSystemCallEmulation() const { return enable_syscall_emulation_; }

        using InstHandlersMap = std::map<std::string, Action>;
        using CsrUpdateActionsMap = std::map<uint32_t, Action>;

        template <typename XLEN> const InstHandlersMap* getInstHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_actions_;
            }
            else
            {
                return &rv32_inst_actions_;
            }
        }

        template <typename XLEN> const InstHandlersMap* getInstComputeAddressHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_compute_address_actions_;
            }
            else
            {
                return &rv32_inst_compute_address_actions_;
            }
        }

        template <typename XLEN> const CsrUpdateActionsMap* getCsrUpdateActionsMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_csr_update_actions_;
            }
            else
            {
                return &rv32_csr_update_actions_;
            }
        }

      private:
        const bool enable_syscall_emulation_;

        Action::ItrType execute_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup execute_action_group_{"Execute"};

        // Instruction handlers
        InstHandlersMap rv64_inst_actions_;
        InstHandlersMap rv32_inst_actions_;

        // Instruction handlers for computing the address of load/store instructions
        InstHandlersMap rv64_inst_compute_address_actions_;
        InstHandlersMap rv32_inst_compute_address_actions_;

        // CSR update Actions for executing write side effects
        CsrUpdateActionsMap rv64_csr_update_actions_;
        CsrUpdateActionsMap rv32_csr_update_actions_;
    };
} // namespace pegasus
