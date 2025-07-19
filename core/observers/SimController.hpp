#pragma once

#include "core/observers/Observer.hpp"
#include <memory>

namespace pegasus
{

    class PegasusState;

    class SimController : public Observer
    {
      public:
        using base_type = SimController;

        SimController();

        void postInit(PegasusState*);
        void onSimulationFinished(PegasusState* state);

      private:
        void preExecute_(PegasusState*) override;
        void postExecute_(PegasusState*) override;
        void preException_(PegasusState*) override;

        class SimEndpoint;
        std::shared_ptr<SimEndpoint> endpoint_;
    };

} // namespace pegasus
