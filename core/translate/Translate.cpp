
#include <vector>

#include "core/translate/Translate.hpp"
#include "core/translate/PageTableEntry.hpp"
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

        // Baremetal (translation disabled)
        {
            registerAction_<RV32, MMUMode::BAREMETAL, INST_TRANSLATION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_);

            registerAction_<RV32, MMUMode::BAREMETAL, DATA_TRANSLATION>(
                "Data Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv32_data_translation_actions_);

            // RV64
            registerAction_<RV64, MMUMode::BAREMETAL, INST_TRANSLATION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);

            registerAction_<RV64, MMUMode::BAREMETAL, DATA_TRANSLATION>(
                "Data Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_data_translation_actions_);
        }

        // Sv32
        {
            registerAction_<RV32, MMUMode::SV32, INST_TRANSLATION>("Inst Translate (Sv32)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv32_inst_translation_actions_);
            registerAction_<RV32, MMUMode::SV32, DATA_TRANSLATION>("Data Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv32_data_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, INST_TRANSLATION>("Inst Translate (Sv32)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, DATA_TRANSLATION>("Data Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }
        // Sv39
        {
            registerAction_<RV64, MMUMode::SV39, INST_TRANSLATION>("Inst Translate (Sv39)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV39, DATA_TRANSLATION>("Data Translate (Sv39)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }
        // Sv48
        {
            registerAction_<RV64, MMUMode::SV48, INST_TRANSLATION>("Inst Translate (Sv48)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV48, DATA_TRANSLATION>("Data Translate (Sv48)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }
        // Sv57
        {
            registerAction_<RV64, MMUMode::SV57, INST_TRANSLATION>("Inst Translate (Sv57)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV57, DATA_TRANSLATION>("Data Translate (Sv57)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }

        // Assume we are booting in RV64 Machine mode with translation disabled
        inst_translate_action_group_.addAction(
            rv64_inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        data_translate_action_group_.addAction(
            rv64_data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
    }

    void Translate::changeMMUMode(uint64_t xlen, uint32_t satp_mode)
    {
        static const std::vector<MMUMode> satp_mmu_mode_map = {
            MMUMode::BAREMETAL, // mode == 0
            MMUMode::SV32,      // mode == 1 xlen==32
            MMUMode::INVALID,   // mode == 2 - 7 -> reserved
            MMUMode::INVALID,   MMUMode::INVALID, MMUMode::INVALID, MMUMode::INVALID,
            MMUMode::INVALID, // mode ==  7
            MMUMode::SV39,    // mode ==  8, xlen==64
            MMUMode::SV48,    // mode ==  9, xlen==64
            MMUMode::SV57     // mode == 10, xlen==64
        };

        sparta_assert(satp_mode < satp_mmu_mode_map.size());

        const MMUMode mode = satp_mmu_mode_map[satp_mode];
        sparta_assert(mode != MMUMode::INVALID);

        if (xlen == 64)
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            data_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv64_data_translation_actions_.at(static_cast<uint32_t>(mode)));
        }
        else
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            data_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv32_data_translation_actions_.at(static_cast<uint32_t>(mode)));
        }
    }

    template <typename XLEN, MMUMode MODE, bool TRANSLATION>
    ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = nullptr;
        if constexpr (TRANSLATION == INST_TRANSLATION)
        {
            // Translation reqest is from fetch
            translation_state = state->getFetchTranslationState();
        }
        else
        {
            const auto & inst = state->getCurrentInst();
            translation_state = inst->getTranslationState();
        }

        const AtlasTranslationState::TranslationRequest request = translation_state->getRequest();
        const XLEN vaddr = request.getVaddr();

        uint32_t level = getNumPageWalkLevels_<MODE>();

        const auto priv_mode = state->getPrivMode();

        // See if xlation is disable -- no level walks
        if (level == 0 || (priv_mode == PrivMode::MACHINE))
        {
            translation_state->setResult(vaddr, request.getSize());
            // Keep going
            return nullptr;
        }

        // TODO: TRANSLATION should be an enum for inst, load or store
        const bool is_store =
            (TRANSLATION == DATA_TRANSLATION) && state->getCurrentInst()->isStoreType();

        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;
        ILOG("Translating " << HEX(vaddr, width));

        // Page size is 4K for both RV32 and RV64
        constexpr uint64_t PAGESHIFT = 12; // 4096
        uint64_t ppn = READ_CSR_FIELD<XLEN>(state, SATP, "ppn") << PAGESHIFT;
        while (level > 0)
        {
            const auto indexed_level = level - 1;
            const auto & vpn_field = extractVpnField_<MODE>(indexed_level);
            const uint64_t pte_paddr = ppn + vpn_field.calcPTEOffset(vaddr) * sizeof(XLEN);
            PageTableEntry<XLEN, MODE> pte = state->readMemory<XLEN>(pte_paddr);
            DLOG_CODE_BLOCK(DLOG_OUTPUT("Level " << indexed_level << " Page Walk");
                            DLOG_OUTPUT("    Addr: " << HEX(pte_paddr, width));
                            DLOG_OUTPUT("     PTE: " << pte););

            // If accessing pte violates a PMA or PMP check, raise an
            // access-fault exception corresponding to the original
            // access type
            if (false == pte.isValid())
            {
                if constexpr (TRANSLATION == INST_TRANSLATION)
                {
                    translation_state->clearRequest();
                }

                break;
            }

            // If PTE is a leaf, perform address translation
            if (pte.isLeaf())
            {
                if ((false == pte.isUserMode()) && (priv_mode != PrivMode::SUPERVISOR))
                {
                    // Must throw
                    break;
                }

                // TODO: Check access permissions more better...
                if (TRANSLATION == DATA_TRANSLATION)
                {
                    if (is_store && (false == pte.canWrite()))
                    {
                        THROW_STORE_AMO_PAGE_FAULT;
                    }
                    else if (false == pte.canRead())
                    {
                        THROW_LOAD_PAGE_FAULT;
                    }
                }

                if (false == pte.isAccessable(is_store))
                {
                    // See if we're required to update access bits in the PTE
                    if (READ_CSR_FIELD<XLEN>(state, MENVCFG, "adue"))
                    {
                        if (is_store)
                        {
                            pte.setDirty();
                        }
                        pte.setAccessed();
                        state->writeMemory<XLEN>(pte_paddr, pte.getPte());
                    }
                    else
                    {
                        // Take exception -- no access allowed or not dirty
                        break;
                    }
                }
                const auto index_bits = (vpn_field.msb - vpn_field.lsb + 1) * indexed_level;
                const auto virt_base = vaddr >> PAGESHIFT;
                Addr paddr = (Addr(pte.getPpn()) | (virt_base & ((0b1 << index_bits) - 1)))
                             << PAGESHIFT;
                paddr |= extractPageOffset_(vaddr); // Add the page offset

                translation_state->setResult(paddr, request.getSize());
                ILOG("  Result: " << HEX(paddr, width));

                // Keep going
                return nullptr;
            }
            // Read next level PTE
            else
            {
                ppn = pte.getPpn() << PAGESHIFT;
            }
            --level;
        }

        if constexpr (TRANSLATION == INST_TRANSLATION)
        {
            translation_state->clearRequest();
            THROW_FETCH_PAGE_FAULT;
        }
        else if (is_store)
        {
            THROW_STORE_AMO_PAGE_FAULT;
        }
        else
        {
            THROW_LOAD_PAGE_FAULT;
        }
        return nullptr;
    }

    // Being pedantic
    template ActionGroup* Translate::translate_<RV32, MMUMode::BAREMETAL, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::BAREMETAL, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV32, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV32, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV39, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV39, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV48, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV48, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV57, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV57, false>(AtlasState*);

    template ActionGroup* Translate::translate_<RV64, MMUMode::BAREMETAL, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::BAREMETAL, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV32, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV32, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV39, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV39, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV48, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV48, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV57, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV57, false>(AtlasState*);

} // namespace atlas
