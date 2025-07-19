#pragma once

#include "core/ActionGroup.hpp"
#include "core/Trap.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{

    class PegasusState;

    /// This class handles exceptions that occur during the fetch-translate-decode-execute cycle,
    /// e.g. for handling illegal instructions.
    class Exception : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Exception";
        using base_type = Exception;

        class ExceptionParameters : public sparta::ParameterSet
        {
          public:
            ExceptionParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
        };

        Exception(sparta::TreeNode* exception_node, const ExceptionParameters* p);

        void setUnhandledException(const FaultCause cause) { fault_cause_ = cause; }

        void setUnhandledException(const InterruptCause cause) { interrupt_cause_ = cause; }

        const sparta::utils::ValidValue<FaultCause> & getUnhandledFault() const
        {
            return fault_cause_;
        }

        const sparta::utils::ValidValue<InterruptCause> & getUnhandledInterrupt() const
        {
            return interrupt_cause_;
        }

        ActionGroup* getActionGroup() { return &exception_action_group_; }

      private:
        void onBindTreeEarly_() override;

        template <typename XLEN>
        Action::ItrType handleException_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup exception_action_group_{"Exception"};

        Action rv32_exception_action_;
        Action rv64_exception_action_;

        sparta::utils::ValidValue<FaultCause> fault_cause_;
        sparta::utils::ValidValue<InterruptCause> interrupt_cause_;

        uint64_t determineTrapValue_(const FaultCause & trap_cause, PegasusState* state);
        uint64_t determineTrapValue_(const InterruptCause & trap_cause, PegasusState* state);
    };

} // namespace pegasus
