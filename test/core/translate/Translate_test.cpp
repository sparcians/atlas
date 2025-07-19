#include "sim/PegasusSim.hpp"

#include "core/PegasusState.hpp"
#include "core/translate/PageTable.hpp"

#include "include/PegasusTypes.hpp"
#include "include/CSRNums.hpp"

#include <bitset>
#include "sparta/utils/SpartaTester.hpp"

class PegasusTranslateTester
{

  public:
    PegasusTranslateTester()
    {
        // Create the simulator
        const uint64_t ilimit = 0;
        pegasus_sim_.reset(new pegasus::PegasusSim(&scheduler_, {}, {}, ilimit));

        pegasus_sim_->buildTree();
        pegasus_sim_->configureTree();
        pegasus_sim_->finalizeTree();

        state_ = pegasus_sim_->getPegasusState();
        translate_unit_ = state_->getTranslateUnit();
    }

    void testPegasusTranslationStateBasic()
    {
        std::cout << "Testing PegasusTranslationState class -- basic" << std::endl;
        const pegasus::Addr vaddr = 0x1000;
        const size_t access_size = 4;

        pegasus::PegasusTranslationState* translation_state = state_->getFetchTranslationState();

        // Requests and results should be invalid and throw an exception
        EXPECT_THROW(translation_state->getRequest());
        EXPECT_THROW(translation_state->getResult());

        // Make a request
        translation_state->makeRequest(vaddr, access_size);

        // Get number of requests and results
        EXPECT_EQUAL(translation_state->getNumRequests(), 1);
        EXPECT_EQUAL(translation_state->getNumResults(), 0);

        // Request size must be non-zero
        EXPECT_THROW(translation_state->makeRequest(vaddr, 0));

        // Trying to get a result before it is ready will fail
        EXPECT_THROW(translation_state->getResult());

        // Set result
        translation_state->setResult(vaddr, vaddr | 0x80000000, access_size);

        // Get number of requests and results
        EXPECT_EQUAL(translation_state->getNumRequests(), 1);
        EXPECT_EQUAL(translation_state->getNumResults(), 1);

        // Cannot make a new request if there are any results
        EXPECT_THROW(translation_state->makeRequest(0, 0));

        // Clear request
        translation_state->popRequest();

        // Get number of requests and results
        EXPECT_EQUAL(translation_state->getNumRequests(), 0);
        EXPECT_EQUAL(translation_state->getNumResults(), 1);

        // Get result
        const pegasus::PegasusTranslationState::TranslationResult & result =
            translation_state->getResult();
        EXPECT_TRUE(result.isValid());
        EXPECT_EQUAL(result.getPAddr(), vaddr | 0x80000000);
        EXPECT_EQUAL(result.getSize(), access_size);

        // Still cannot make any new requests until results have been cleared
        EXPECT_THROW(translation_state->makeRequest(0, 0));

        // Clear result
        translation_state->popResult();

        // Get number of requests and results
        EXPECT_EQUAL(translation_state->getNumRequests(), 0);
        EXPECT_EQUAL(translation_state->getNumResults(), 0);

        // No more requests or results that can be cleared
        EXPECT_THROW(translation_state->popRequest());
        EXPECT_THROW(translation_state->popResult());
    }

    void testPegasusTranslationStateMisaligned()
    {
        std::cout << "Testing PegasusTranslationState class -- misaligned" << std::endl;
        pegasus::PegasusTranslationState* translation_state = state_->getFetchTranslationState();

        // Make a request and mark it as misaligned
        const pegasus::Addr vaddr = 0xffe;
        const size_t access_size = 4;
        translation_state->makeRequest(vaddr, access_size);
        pegasus::PegasusTranslationState::TranslationRequest & request =
            translation_state->getRequest();
        request.setMisaligned(2);

        // Number of misaligned bytes must be valid
        EXPECT_THROW(request.setMisaligned(4));
        EXPECT_THROW(request.setMisaligned(0));

        // Set results
        translation_state->setResult(vaddr, vaddr | 0x80000000,
                                     access_size - request.getMisalignedBytes());
        translation_state->setResult(vaddr, (vaddr + request.getMisalignedBytes()) | 0x80000000,
                                     request.getMisalignedBytes());

        // Get number of requests and results
        EXPECT_EQUAL(translation_state->getNumRequests(), 1);
        EXPECT_EQUAL(translation_state->getNumResults(), 2);

        translation_state->popRequest();
        translation_state->popResult();
        translation_state->popResult();
    }

    void testPegasusTranslationStateMultiple()
    {
        std::cout << "Testing PegasusTranslationState class -- multiple" << std::endl;
        pegasus::PegasusTranslationState* translation_state = state_->getFetchTranslationState();

        // Make multiple requests
        const uint64_t base_vaddr = 0x1000;
        uint64_t offset = 4;
        size_t access_size = 4;
        uint64_t num_requests = 8;
        for (uint64_t num = 0; num < num_requests; num++)
        {
            const uint64_t vaddr = base_vaddr + (num * offset);
            std::cout << "Making request: 0x" << std::hex << vaddr << std::endl;
            translation_state->makeRequest(vaddr, access_size);
        }

        EXPECT_EQUAL(translation_state->getNumRequests(), num_requests);

        // Handle requests
        for (uint64_t num = 0; num < num_requests; num++)
        {
            const pegasus::PegasusTranslationState::TranslationRequest & request =
                translation_state->getRequest();
            const uint64_t paddr = request.getVAddr() | 0x80000000;
            translation_state->setResult(request.getVAddr(), paddr, request.getSize());
            translation_state->popRequest();

            // Can't make any new requests
            EXPECT_THROW(translation_state->makeRequest(0, 0));
        }

        EXPECT_EQUAL(translation_state->getNumRequests(), 0);
        EXPECT_EQUAL(translation_state->getNumResults(), num_requests);

        // Get Results
        for (uint64_t num = 0; num < num_requests; num++)
        {
            const pegasus::PegasusTranslationState::TranslationResult & result =
                translation_state->getResult();
            std::cout << "Getting result: 0x" << std::hex << result.getPAddr() << std::endl;
            translation_state->popResult();
        }

        EXPECT_EQUAL(translation_state->getNumResults(), 0);
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
            const uint32_t pte_val =
                (ppn1 << pegasus::translate_types::Sv32::PteFields::ppn1.lsb)
                | (ppn0 << pegasus::translate_types::Sv32::PteFields::ppn0.lsb)
                | (rsw << pegasus::translate_types::Sv32::PteFields::rsw.lsb)
                | (d << pegasus::translate_types::Sv32::PteFields::dirty.lsb)
                | (a << pegasus::translate_types::Sv32::PteFields::accessed.lsb)
                | (g << pegasus::translate_types::Sv32::PteFields::global.lsb)
                | (u << pegasus::translate_types::Sv32::PteFields::user.lsb)
                | (x << pegasus::translate_types::Sv32::PteFields::execute.lsb)
                | (w << pegasus::translate_types::Sv32::PteFields::write.lsb)
                | (r << pegasus::translate_types::Sv32::PteFields::read.lsb) | (v);

            std::cout << "Creating sv32 PTE: " << HEX8(pte_val) << std::endl;
            pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> sv32_pte(pte_val);

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
            const uint64_t pte_val =
                (ppn2 << pegasus::translate_types::Sv39::PteFields::ppn2.lsb)
                | (ppn1 << pegasus::translate_types::Sv39::PteFields::ppn1.lsb)
                | (ppn0 << pegasus::translate_types::Sv39::PteFields::ppn0.lsb)
                | (rsw << pegasus::translate_types::Sv39::PteFields::rsw.lsb)
                | (d << pegasus::translate_types::Sv39::PteFields::dirty.lsb)
                | (a << pegasus::translate_types::Sv39::PteFields::accessed.lsb)
                | (g << pegasus::translate_types::Sv39::PteFields::global.lsb)
                | (u << pegasus::translate_types::Sv39::PteFields::user.lsb)
                | (x << pegasus::translate_types::Sv39::PteFields::execute.lsb)
                | (w << pegasus::translate_types::Sv39::PteFields::write.lsb)
                | (r << pegasus::translate_types::Sv39::PteFields::read.lsb) | (v);

            std::cout << "Creating sv64 PTE: " << HEX16(pte_val) << std::endl;
            pegasus::PageTableEntry<pegasus::RV64, pegasus::MMUMode::SV39> sv39_pte(pte_val);

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
        constexpr uint32_t PTE_SIZE = sizeof(pegasus::RV32);
        const uint32_t base_addr = 0x1000;
        pegasus::PageTable<pegasus::RV32, pegasus::MMUMode::SV32> pt(base_addr);

        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> sv32_pte1(0x7B1EEFF);
        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> sv32_pte2(0xABC12FF);
        // Valid, Read-only (0111 1111 0000 0011 1101 0100 1100 0011)
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> sv32_pte3(0x7F03D4C3);
        // Valid,
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> sv32_pte4(0xABC12FF);

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
        pegasus::PegasusTranslationState* translation_state = state_->getFetchTranslationState();
        const pegasus::Addr vaddr = 0x1000;
        const uint32_t access_size = 4;
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;
        translation_state->makeRequest(vaddr, access_size);

        // Execute translation
        pegasus::Action::ItrType dummy_action_it;
        dummy_action_it = translate_unit_->translate_<pegasus::RV64, pegasus::MMUMode::BAREMETAL,
                                                      pegasus::Translate::AccessType::INSTRUCTION>(
            state_, dummy_action_it);

        // Get translation result
        const pegasus::PegasusTranslationState::TranslationResult & result =
            translation_state->getResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.getPAddr();
        std::cout << ", Access size: " << std::dec << result.getSize() << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.getPAddr(), vaddr);
        EXPECT_EQUAL(result.getSize(), access_size);

        translation_state->reset();
    }

    void testSv32Translation()
    {
        std::cout << "Testing sv32 translation\n" << std::endl;

        state_->setPrivMode(pegasus::PrivMode::SUPERVISOR, true);

        const uint32_t vaddr = 0x143FFABC;
        const uint32_t page_offset = vaddr & 0xFFF;
        const uint64_t expected_paddr = 0x200000000 + page_offset;

        // First-level page table
        uint32_t satp_ppn = 0x10;
        const uint32_t lvl1_base_paddr = satp_ppn << 12; // 0x10000
        pegasus::PageTable<pegasus::RV32, pegasus::MMUMode::SV32> lvl1_pagetable(lvl1_base_paddr);
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> lvl1_pte{0x8031};
        const uint32_t lvl1_index = (vaddr & 0xFFC00000) >> 22;
        const uint32_t lvl1_paddr = lvl1_pagetable.getAddrOfIndex(lvl1_index);
        std::cout << "Loading Level 1 PTE at address 0x" << std::hex << lvl1_paddr
                  << " (idx: " << std::dec << lvl1_index << ")" << std::endl;
        lvl1_pagetable.addEntry(lvl1_index, lvl1_pte);
        std::cout << "    " << lvl1_pte << std::endl;

        // Second-level page table
        const uint32_t lvl2_base_paddr = 0x20000;
        pegasus::PageTable<pegasus::RV32, pegasus::MMUMode::SV32> lvl2_pagetable(lvl2_base_paddr);
        pegasus::PageTableEntry<pegasus::RV32, pegasus::MMUMode::SV32> lvl2_pte{0x8000002f};
        lvl2_pte.setAccessed();
        const uint32_t lvl2_index = (vaddr & 0x3FF000) >> 12;
        const uint32_t lvl2_paddr = lvl2_pagetable.getAddrOfIndex(lvl2_index);
        std::cout << "Loading Level 2 PTE at address 0x" << std::hex << lvl2_paddr
                  << " (idx: " << std::dec << lvl2_index << ")" << std::endl;
        lvl2_pagetable.addEntry(lvl2_index, lvl2_pte);
        std::cout << "    " << lvl2_pte << std::endl;
        std::cout << std::endl;

        // Set SATP value
        state_->getCsrRegister(pegasus::CSR::SATP::reg_num)->dmiWrite<uint64_t>(satp_ppn);

        // Write PTEs to memory
        state_->writeMemory<uint32_t>(lvl1_paddr, lvl1_pte.getPte());
        state_->writeMemory<uint32_t>(lvl2_paddr, lvl2_pte.getPte());

        // Make translation request
        pegasus::PegasusTranslationState* translation_state = state_->getFetchTranslationState();
        const size_t access_size = 4;
        translation_state->makeRequest(vaddr, access_size);
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;

        // Translate!
        pegasus::Action::ItrType dummy_action_it;
        dummy_action_it = translate_unit_->translate_<pegasus::RV32, pegasus::MMUMode::SV32,
                                                      pegasus::Translate::AccessType::INSTRUCTION>(
            state_, dummy_action_it);

        // Get translation result
        const pegasus::PegasusTranslationState::TranslationResult & result =
            translation_state->getResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.getPAddr();
        std::cout << ", Access size: " << std::dec << result.getSize() << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.getPAddr(), expected_paddr);
        EXPECT_EQUAL(result.getSize(), access_size);

        translation_state->reset();
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<pegasus::PegasusSim> pegasus_sim_;

    pegasus::PegasusState* state_ = nullptr;
    pegasus::Translate* translate_unit_ = nullptr;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    PegasusTranslateTester translate_tester;

    translate_tester.testPegasusTranslationStateBasic();
    translate_tester.testPegasusTranslationStateMisaligned();
    translate_tester.testPegasusTranslationStateMultiple();
    // translate_tester.testPageTableEntry();
    // translate_tester.testPageTable();

    // translate_tester.testBaremetalTranslation();
    // translate_tester.testSv32Translation();

    REPORT_ERROR;
    return ERROR_CODE;
}
