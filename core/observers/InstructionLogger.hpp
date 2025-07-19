#pragma once

#include "core/observers/Observer.hpp"
#include "sparta/functional/Register.hpp"

namespace sparta::log
{
    class MessageSource;
}

namespace pegasus
{
    class InstLogWriterBase;

    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(sparta::log::MessageSource & inst_logger, const ObserverMode arch);

        void useSpikeFormatting();

      private:
        void postExecute_(PegasusState*) override;

        sparta::log::MessageSource & inst_logger_;
        std::shared_ptr<InstLogWriterBase> inst_log_writer_;
    };
} // namespace pegasus
