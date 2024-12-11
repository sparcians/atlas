#pragma once

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvmInsts
    {
      public:
        using base_type = RvmInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        ActionGroup* div_64_handler(atlas::AtlasState* state);
        ActionGroup* divu_64_handler(atlas::AtlasState* state);
        ActionGroup* divuw_64_handler(atlas::AtlasState* state);
        ActionGroup* divw_64_handler(atlas::AtlasState* state);
        ActionGroup* mul_64_handler(atlas::AtlasState* state);
        ActionGroup* mulh_64_handler(atlas::AtlasState* state);
        ActionGroup* mulhsu_64_handler(atlas::AtlasState* state);
        ActionGroup* mulhu_64_handler(atlas::AtlasState* state);
        ActionGroup* mulw_64_handler(atlas::AtlasState* state);
        ActionGroup* rem_64_handler(atlas::AtlasState* state);
        ActionGroup* remu_64_handler(atlas::AtlasState* state);
        ActionGroup* remuw_64_handler(atlas::AtlasState* state);
        ActionGroup* remw_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
