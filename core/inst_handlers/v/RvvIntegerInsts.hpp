#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        // Ingeter Arithmetic / Bitwise Logical
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType viablHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Result for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp,
                  template <typename> typename FunctorTemp>
        Action::ItrType viacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Carry/borrow for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFuc>
        Action::ItrType vmiacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Ingeter Compare
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vmicHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
