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
        ActionGroup* csrrc_64_handler(atlas::AtlasState* state);
        ActionGroup* csrrci_64_handler(atlas::AtlasState* state);
        ActionGroup* csrrs_64_handler(atlas::AtlasState* state);
        ActionGroup* csrrsi_64_handler(atlas::AtlasState* state);
        ActionGroup* csrrw_64_handler(atlas::AtlasState* state);
        ActionGroup* csrrwi_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
