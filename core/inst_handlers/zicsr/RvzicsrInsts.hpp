#pragma once

#include "core/Execute.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvzicsrInsts
    {
      public:
        using base_type = RvzicsrInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

        template <typename XLEN> static void getCsrUpdateActions(Execute::CsrUpdateActionsMap &);

      private:
        template <typename XLEN> ActionGroup* csrrc_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrci_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrs_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrsi_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrw_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrwi_handler(atlas::AtlasState* state);

        // CSR update Actions for executing write side effects
        template <typename XLEN> ActionGroup* mstatus_update_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sstatus_update_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcsr_update_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fflags_update_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* frm_update_handler(atlas::AtlasState* state);
    };
} // namespace atlas
