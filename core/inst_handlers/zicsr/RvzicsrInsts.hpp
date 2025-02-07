#pragma once

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvzicsrInsts
    {
      public:
        using base_type = RvzicsrInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN> ActionGroup* csrrc_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrci_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrs_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrsi_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrw_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrwi_handler(atlas::AtlasState* state);
    };
} // namespace atlas
