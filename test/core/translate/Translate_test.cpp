#include "sim/AtlasSim.hpp"

#include "core/AtlasState.hpp"
#include "core/translate/PageTable.hpp"

#include "include/AtlasTypes.hpp"
#include "include/CSRNums.hpp"

#include <bitset>
#include "sparta/utils/SpartaTester.hpp"

class AtlasTranslateTester
{

  public:
    AtlasTranslateTester()
    {
        // Create the simulator
        const std::string workload = "";
        const uint64_t ilimit = 0;
        atlas_sim_.reset(new atlas::AtlasSim(&scheduler_, workload, ilimit));

        atlas_sim_->buildTree();
        atlas_sim_->configureTree();
        atlas_sim_->finalizeTree();

        state_ = atlas_sim_->getAtlasState();
        translate_unit_ = state_->getTranslateUnit();
    }

    void testAtlasTranslationState()
    {
        std::cout << "Testing AtlasTranslationState class" << std::endl;
        std::cout << std::endl;
        const atlas::Addr vaddr = 0x1000;
        const uint32_t access_size = 4;
        // TODO: Add test for AtlasTranslationState state machine

        atlas::AtlasTranslationState* translation_state = state_->getFetchTranslationState();

        // Request should be invalid and throw an exception
        EXPECT_THROW(translation_state->getRequest());

        // This will call setTransaltionRequest which should fail as transalationRequest is invalid
        // FIXME: Sparta macro doesn't understand templated methods
        // EXPECT_THROW(translate_unit_->translate_<atlas::RV64,
        // atlas::MMUMode::BAREMETAL>(state_));

        // Make a second request without resolving the first request
        translation_state->makeRequest(vaddr, access_size);
        EXPECT_THROW(translation_state->makeRequest(vaddr, access_size););

        // Try to get a result before it is ready will fail
        EXPECT_THROW(translation_state->getResult());

        // Clear current request so subsequent tests will pass
        translation_state->setResult(vaddr, access_size);

        // TODO: Add tests for AtlasTranslationState request/result validation
        //    - Make a misaligned request (is it supported?)
        //    - Make a request with an invalid size (too large, too small, not power of 2, etc.)
        //    - Set the result with the wrong size
        //    - More?
    }

    void testPageTableEntry() // unit test for PageTableEntry.cpp
    {
        std::cout << "Testing PageTableEntry class" << std::endl;

        // sv32
        {
            const bool v = true;
            const bool r = true;
            const bool w = true;
            const bool x = true;
            const bool u = true;
            const bool g = true;
            const bool a = true;
            const bool d = true;
            const uint32_t rsw = 2;
            const uint32_t ppn0 = 123;
            const uint32_t ppn1 = 456;
            const uint32_t pte_val = (ppn1 << atlas::translate_types::Sv32::PteFields::ppn1.lsb)
                                     | (ppn0 << atlas::translate_types::Sv32::PteFields::ppn0.lsb)
                                     | (rsw << atlas::translate_types::Sv32::PteFields::rsw.lsb)
                                     | (d << atlas::translate_types::Sv32::PteFields::dirty.lsb)
                                     | (a << atlas::translate_types::Sv32::PteFields::accessed.lsb)
                                     | (g << atlas::translate_types::Sv32::PteFields::global.lsb)
                                     | (u << atlas::translate_types::Sv32::PteFields::user.lsb)
                                     | (x << atlas::translate_types::Sv32::PteFields::execute.lsb)
                                     | (w << atlas::translate_types::Sv32::PteFields::write.lsb)
                                     | (r << atlas::translate_types::Sv32::PteFields::read.lsb)
                                     | (v);

            std::cout << "Creating sv32 PTE: " << HEX8(pte_val) << std::endl;
            atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte(pte_val);

            EXPECT_TRUE(sv32_pte.isValid());
            EXPECT_TRUE(sv32_pte.canRead());
            EXPECT_TRUE(sv32_pte.canWrite());
            EXPECT_TRUE(sv32_pte.canExecute());
            EXPECT_TRUE(sv32_pte.isUserMode());
            EXPECT_TRUE(sv32_pte.isGlobal());
            EXPECT_TRUE(sv32_pte.isAccessed());
            EXPECT_TRUE(sv32_pte.isDirty());
            EXPECT_EQUAL(sv32_pte.getRsw(), rsw);
            EXPECT_EQUAL(sv32_pte.getPpnField(0), ppn0);
            EXPECT_EQUAL(sv32_pte.getPpnField(1), ppn1);
        }
        // sv39
        {
            const bool v = true;
            const bool r = true;
            const bool w = false;
            const bool x = false;
            const bool u = true;
            const bool g = true;
            const bool a = true;
            const bool d = true;
            const uint32_t rsw = 2;
            const uint64_t ppn0 = 123;
            const uint64_t ppn1 = 456;
            const uint64_t ppn2 = 789;
            const uint64_t pte_val = (ppn2 << atlas::translate_types::Sv39::PteFields::ppn2.lsb)
                                     | (ppn1 << atlas::translate_types::Sv39::PteFields::ppn1.lsb)
                                     | (ppn0 << atlas::translate_types::Sv39::PteFields::ppn0.lsb)
                                     | (rsw << atlas::translate_types::Sv39::PteFields::rsw.lsb)
                                     | (d << atlas::translate_types::Sv39::PteFields::dirty.lsb)
                                     | (a << atlas::translate_types::Sv39::PteFields::accessed.lsb)
                                     | (g << atlas::translate_types::Sv39::PteFields::global.lsb)
                                     | (u << atlas::translate_types::Sv39::PteFields::user.lsb)
                                     | (x << atlas::translate_types::Sv39::PteFields::execute.lsb)
                                     | (w << atlas::translate_types::Sv39::PteFields::write.lsb)
                                     | (r << atlas::translate_types::Sv39::PteFields::read.lsb)
                                     | (v);

            std::cout << "Creating sv64 PTE: " << HEX16(pte_val) << std::endl;
            atlas::PageTableEntry<atlas::RV64, atlas::MMUMode::SV39> sv39_pte(pte_val);

            EXPECT_TRUE(sv39_pte.isValid());
            EXPECT_TRUE(sv39_pte.canRead());
            EXPECT_FALSE(sv39_pte.canWrite());
            EXPECT_FALSE(sv39_pte.canExecute());
            EXPECT_TRUE(sv39_pte.isUserMode());
            EXPECT_TRUE(sv39_pte.isGlobal());
            EXPECT_TRUE(sv39_pte.isAccessed());
            EXPECT_TRUE(sv39_pte.isDirty());
            EXPECT_EQUAL(sv39_pte.getRsw(), rsw);
            EXPECT_EQUAL(sv39_pte.getPpnField(0), ppn0);
            EXPECT_EQUAL(sv39_pte.getPpnField(1), ppn1);
            EXPECT_EQUAL(sv39_pte.getPpnField(2), ppn2);
        }
    }

    void testPageTable() // unit test for PageTable.cpp
    {
        std::cout << "\nTesting PageTable class" << std::endl;
        constexpr uint32_t PTE_SIZE = sizeof(atlas::RV32);
        const uint32_t base_addr = 0x1000;
        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> pt(base_addr);

        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte1(0x7B1EEFF);
        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte2(0xABC12FF);
        // Valid, Read-only (0111 1111 0000 0011 1101 0100 1100 0011)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte3(0x7F03D4C3);
        // Valid,
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte4(0xABC12FF);

        std::cout << "Creating page table with 4 PTEs:" << std::endl;
        std::cout << "    PTE1: " << sv32_pte1 << std::endl;
        std::cout << "    PTE2: " << sv32_pte2 << std::endl;
        std::cout << "    PTE3: " << sv32_pte3 << std::endl;
        std::cout << "    PTE4: " << sv32_pte4 << std::endl;
        std::cout << std::endl;

        // Add entries
        pt.addEntry(0, sv32_pte1); // First entry
        pt.addEntry(10, sv32_pte2);
        pt.addEntry(100, sv32_pte3);
        pt.addEntry(1023, sv32_pte4); // Last entry

        // Cannot add an entry out of range
        EXPECT_THROW(pt.addEntry(1024, sv32_pte4));

        // Get entries by index
        EXPECT_EQUAL(pt.getEntry(0).getPpn(), sv32_pte1.getPpn());
        EXPECT_EQUAL(pt.getEntry(10).getPpn(), sv32_pte2.getPpn());
        EXPECT_EQUAL(pt.getEntry(100).getPpn(), sv32_pte3.getPpn());
        EXPECT_EQUAL(pt.getEntry(1023).getPpn(), sv32_pte4.getPpn());

        // Invalid indexes are not allowed (out of range, not initialized)
        EXPECT_THROW(pt.getEntry(10000));
        EXPECT_THROW(pt.getEntry(11));

        // Get entries by address
        EXPECT_EQUAL(pt.getEntryAtAddr(base_addr).getPpn(), sv32_pte1.getPpn());
        EXPECT_EQUAL(pt.getEntryAtAddr(base_addr + 10 * PTE_SIZE).getPpn(), sv32_pte2.getPpn());
        EXPECT_EQUAL(pt.getEntryAtAddr(base_addr + 100 * PTE_SIZE).getPpn(), sv32_pte3.getPpn());
        EXPECT_EQUAL(pt.getEntryAtAddr(base_addr + 1023 * PTE_SIZE).getPpn(), sv32_pte4.getPpn());

        // Invalid address are not allowed (out of range, not initalized, misaligned addresses)
        EXPECT_THROW(pt.getEntryAtAddr(0xFFFF))
        EXPECT_THROW(pt.getEntryAtAddr(0x1004)); // idx 1
        EXPECT_THROW(pt.getEntryAtAddr(0x1001)); // Middle of idx 0 PTE

        // Get address of indexes
        EXPECT_EQUAL(pt.getAddrOfIndex(0), base_addr);
        EXPECT_EQUAL(pt.getAddrOfIndex(10), base_addr + 10 * PTE_SIZE);
        EXPECT_EQUAL(pt.getAddrOfIndex(100), base_addr + 100 * PTE_SIZE);
        EXPECT_EQUAL(pt.getAddrOfIndex(1023), base_addr + 1023 * PTE_SIZE);

        // Remove entries
        EXPECT_TRUE(pt.contains(1023));
        pt.removeEntry(1023);
        EXPECT_FALSE(pt.contains(1023));

        // Cannot remove entry that is not initialized
        EXPECT_THROW(pt.removeEntry(1022));
    }

    void testBaremetalTranslation()
    {
        std::cout << "Testing baremetal translation\n" << std::endl;

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getFetchTranslationState();
        const atlas::Addr vaddr = 0x1000;
        const uint32_t access_size = 4;
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;
        translation_state->makeRequest(vaddr, access_size);

        // Execute translation
        auto next_action_group =
            translate_unit_->translate_<atlas::RV64, atlas::MMUMode::BAREMETAL, true>(state_);
        EXPECT_EQUAL(next_action_group, nullptr);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result =
            translation_state->getResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.getPaddr();
        std::cout << ", Access size: " << std::dec << result.getSize() << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.getPaddr(), vaddr);
        EXPECT_EQUAL(result.getSize(), access_size);
    }

    void testSv32Translation()
    {
        std::cout << "Testing sv32 translation\n" << std::endl;

        state_->setPrivMode(atlas::PrivMode::SUPERVISOR, true);

        const uint32_t vaddr = 0x143FFABC;
        const uint32_t page_offset = vaddr & 0xFFF;
        const uint64_t expected_paddr = 0x200000000 + page_offset;

        // First-level page table
        uint32_t satp_ppn = 0x10;
        const uint32_t lvl1_base_paddr = satp_ppn << 12; // 0x10000
        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> lvl1_pagetable(lvl1_base_paddr);
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> lvl1_pte{0x8031};
        const uint32_t lvl1_index = (vaddr & 0xFFC00000) >> 22;
        const uint32_t lvl1_paddr = lvl1_pagetable.getAddrOfIndex(lvl1_index);
        std::cout << "Loading Level 1 PTE at address 0x" << std::hex << lvl1_paddr
                  << " (idx: " << std::dec << lvl1_index << ")" << std::endl;
        lvl1_pagetable.addEntry(lvl1_index, lvl1_pte);
        std::cout << "    " << lvl1_pte << std::endl;

        // Second-level page table
        const uint32_t lvl2_base_paddr = 0x20000;
        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> lvl2_pagetable(lvl2_base_paddr);
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> lvl2_pte{0x8000002f};
        lvl2_pte.setAccessed();
        const uint32_t lvl2_index = (vaddr & 0x3FF000) >> 12;
        const uint32_t lvl2_paddr = lvl2_pagetable.getAddrOfIndex(lvl2_index);
        std::cout << "Loading Level 2 PTE at address 0x" << std::hex << lvl2_paddr
                  << " (idx: " << std::dec << lvl2_index << ")" << std::endl;
        lvl2_pagetable.addEntry(lvl2_index, lvl2_pte);
        std::cout << "    " << lvl2_pte << std::endl;
        std::cout << std::endl;

        // Set SATP value
        state_->getCsrRegister(atlas::CSR::SATP::reg_num)->dmiWrite<uint64_t>(satp_ppn);

        // Write PTEs to memory
        state_->writeMemory<uint32_t>(lvl1_paddr, lvl1_pte.getPte());
        state_->writeMemory<uint32_t>(lvl2_paddr, lvl2_pte.getPte());

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getFetchTranslationState();
        const size_t access_size = 4;
        translation_state->makeRequest(vaddr, access_size);
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;

        // Translate!
        translate_unit_->translate_<atlas::RV32, atlas::MMUMode::SV32, true>(state_);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result =
            translation_state->getResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.getPaddr();
        std::cout << ", Access size: " << std::dec << result.getSize() << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.getPaddr(), expected_paddr);
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<atlas::AtlasSim> atlas_sim_;

    atlas::AtlasState* state_ = nullptr;
    atlas::Translate* translate_unit_ = nullptr;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    AtlasTranslateTester translate_tester;

    translate_tester.testAtlasTranslationState();
    translate_tester.testPageTableEntry();
    translate_tester.testPageTable();

    translate_tester.testBaremetalTranslation();
    translate_tester.testSv32Translation();

    REPORT_ERROR;
    return (int)ERROR_CODE;
}
