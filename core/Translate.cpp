#include "core/Translate.hpp"
#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
    Translate::Translate(sparta::TreeNode* translate_node, const TranslateParameters* p) :
        sparta::Unit(translate_node)
    {
        (void)p;

        Action inst_translate_action =
            atlas::Action::createAction<&Translate::translate_>(this, "Inst Translate");
        inst_translate_action.addTag(ActionTags::INST_TRANSLATE_TAG);
        inst_translate_action_group_.addAction(inst_translate_action);

        Action data_translate_action =
            atlas::Action::createAction<&Translate::translate_>(this, "Data Translate");
        data_translate_action.addTag(ActionTags::DATA_TRANSLATE_TAG);
        data_translate_action_group_.addAction(data_translate_action);
    }

    ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = state->getTranslationState();
        const AtlasTranslationState::TranslationRequest request =
            translation_state->getTranslationRequest();

        ILOG("Translating 0x" << std::hex << request.virtual_addr);

        // Translation currently not supported, assume VA = PA
        // TODO: PageTableWalker.pageTableWalk(request.virtual_addr, state);
        translation_state->setTranslationResult(request.virtual_addr, request.size);

        // Keep going
        return nullptr;
    }
} // namespace atlas
