#include "sim/AtlasSim.hpp"
#include "core/AtlasState.hpp"
#include "include/AtlasTypes.hpp"
#include "core/PageTableEntry.hpp"
#include "core/PageTable.hpp"
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
        const atlas::Addr va = 0x1000;
        const uint32_t access_size = 4;
        // TODO: Add test for AtlasTranslationState state machine

        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        EXPECT_THROW(
            translation_state
                ->getTranslationRequest()); // request should be invalid hence expedct throw;

        EXPECT_THROW(
            translate_unit_->translate_(state_)); // this will call setTransaltionRequest which
                                                  // should fail as transalationRequest is invalid

        //    - Make a second request without resolving the first request
        translation_state->makeTranslationRequest(va, access_size);                // 1st request
        EXPECT_THROW(translation_state->makeTranslationRequest(va, access_size);); // 2nd request

        //    - Try to get a result before it is ready
        EXPECT_THROW(translation_state->getTranslationResult()); // getting the result before it is
                                                                 // ready, hence expect failure

        //    - More?
        //
        // TODO: Add tests for AtlasTranslationState request/result validation
        //    - Make a misaligned request (is it supported?)
        //    - Make a request with an invalid size (too large, too small, not power of 2, etc.)
        //    - Set the result with the wrong size
        //    - More?
    }

    void testBaremetalTranslation()
    {
        std::cout << "Testing baremetal translation\n" << std::endl;

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        const atlas::Addr va = 0x1000;
        const uint32_t access_size = 4;
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << va;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;
        translation_state->makeTranslationRequest(va, access_size);

        // Execute translation
        auto next_action_group = translate_unit_->translate_(state_);
        EXPECT_EQUAL(next_action_group, nullptr);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result =
            translation_state->getTranslationResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.physical_addr;
        std::cout << ", Access size: " << std::dec << result.size << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.physical_addr, va);
        EXPECT_EQUAL(result.size, access_size);
    }

    void testPageTableEntryCreation() // unit test for PageTableEntry.cpp
    {
        const bool v = true;
        const bool r = true;
        const bool w = true;
        const bool x = true;
        const bool u = true;
        const bool g = true;
        const bool a = true;
        const bool d = true;
        uint8_t rsw = 2;
        uint16_t ppn0 = 123;
        uint16_t ppn1 = 123;
        uint32_t examplePTEValue;
        examplePTEValue = (ppn1 << 20) | (ppn0 << 10) | ((rsw & TWO_BIT_MASK) << 8) | (d << 7)
                          | (a << 6) | (g << 5) | (u << 4) | (x << 3) | (w << 2) | (r << 1) | (v);
        atlas::PageTableEntry<atlas::MMUMode::SV32> sv32PageTableEntry(examplePTEValue);
        EXPECT_TRUE(sv32PageTableEntry.isValid());
        EXPECT_TRUE(sv32PageTableEntry.canRead());
        EXPECT_TRUE(sv32PageTableEntry.canWrite());
        EXPECT_TRUE(sv32PageTableEntry.canExecute());
        EXPECT_TRUE(sv32PageTableEntry.isUserMode());
        EXPECT_TRUE(sv32PageTableEntry.isGlobal());
        EXPECT_TRUE(sv32PageTableEntry.isAccessed());
        EXPECT_TRUE(sv32PageTableEntry.isDirty());
        EXPECT_EQUAL(sv32PageTableEntry.getRSW(), rsw);
        EXPECT_EQUAL(sv32PageTableEntry.getPPN0(), ppn0);
        EXPECT_EQUAL(sv32PageTableEntry.getPPN1(), ppn1);
    }

    void testPageTable() // unit test for PageTable.cpp
    {
        uint32_t pa = 0x7B1EEFF;
        uint32_t baseAddrOfPT = 0xFFFF0000;
        atlas::PageTable<atlas::MMUMode::SV32> pt(baseAddrOfPT);
        atlas::PageTableEntry<atlas::MMUMode::SV32> sv32PTE1(
            pa); // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111
                 // 0000 1111)
        atlas::PageTableEntry<atlas::MMUMode::SV32> sv32PTE2(
            0xABC12FF); // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010
                        // 1111 0000 1111)
        atlas::PageTableEntry<atlas::MMUMode::SV32> sv32PTE3(
            0x7F03D4C3); // Valid, Read-only (0111 1111 0000 0011 1101 0100 1100 0011)
        atlas::PageTableEntry<atlas::MMUMode::SV32> sv32PTE4(
            0xABC12FF); // Invalid PTE (0000 0000 0000 0000 0000 0000 0000 0000)

        pt.addEntry(baseAddrOfPT + 1, sv32PTE1);
        pt.addEntry(baseAddrOfPT + 10, sv32PTE2);
        pt.addEntry(baseAddrOfPT + 100, sv32PTE3);
        pt.addEntry(baseAddrOfPT + 1024, sv32PTE4);

        EXPECT_EQUAL(pt.getEntry(baseAddrOfPT + 1).getPPN(), sv32PTE1.getPPN());
        EXPECT_EQUAL(pt.getEntry(baseAddrOfPT + 10).getPPN(), sv32PTE2.getPPN());
        EXPECT_EQUAL(pt.getEntry(baseAddrOfPT + 100).getPPN(), sv32PTE3.getPPN());
        EXPECT_EQUAL(pt.getEntry(baseAddrOfPT + 1024).getPPN(), sv32PTE4.getPPN());

        EXPECT_THROW(pt.addEntry(
            baseAddrOfPT + 1025,
            sv32PTE4)); // Page table has reached its maximum capacity/PageTable index out of bound!
        EXPECT_THROW(pt.addEntry(
            baseAddrOfPT,
            sv32PTE4)); // Page table has reached its maximum capacity/PageTable index out of bound!

        EXPECT_THROW(
            pt.getEntry(baseAddrOfPT + 1023).getPPN()); // entry not present at the provided index

        EXPECT_THROW(pt.removeEntry(baseAddrOfPT + 1044)); // Index Invalid

        EXPECT_TRUE(pt.contains(baseAddrOfPT + 1024));
        pt.removeEntry(baseAddrOfPT + 1024);
        EXPECT_FALSE(pt.contains(baseAddrOfPT + 1024));

        EXPECT_TRUE(pt.contains(baseAddrOfPT + 100));
        pt.removeEntry(baseAddrOfPT + 100);
        EXPECT_FALSE(pt.contains(baseAddrOfPT + 100));

        EXPECT_TRUE(pt.contains(baseAddrOfPT + 10));
        pt.removeEntry(baseAddrOfPT + 10);
        EXPECT_FALSE(pt.contains(baseAddrOfPT + 10));

        EXPECT_TRUE(pt.contains(baseAddrOfPT + 1));
        pt.removeEntry(baseAddrOfPT + 1);
        EXPECT_FALSE(pt.contains(baseAddrOfPT + 1));
    }

    void testSv32Translation()
    {
        // const atlas::Addr pa = 0x1000;
        // const uint64_t value = 0xdeadbeef;
        // state_->writeMemory<uint64_t>(pa, value);

        // presetup fopr the test, install all the addresses in pageTable Setups

        // uint32_t va = 0xABCD;
        // uint64_t satpBaseAddress = 0x0000;

        // call ptw with the above fields

        // indexFromVa  = extract index from VA
        // pte          = use that index to get PTE from the PageTable Map
        // PA           = use the pte to get PA from it, pass the PA to next level page for the
        // further walk
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
    translate_tester.testBaremetalTranslation();
    translate_tester.testPageTableEntryCreation();
    translate_tester.testAtlasTranslationState();
    translate_tester.testPageTable();

    REPORT_ERROR;
    return (int)ERROR_CODE;
}
