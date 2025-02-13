#include "sim/AtlasSim.hpp"
#include "core/AtlasState.hpp"
#include "include/AtlasTypes.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "arch/register_macros.hpp"
#include <bitset>
#include <random>

class RegisterTester
{
  public:
    RegisterTester()
    {
        atlas_sim_.buildTree();
        atlas_sim_.configureTree();
        atlas_sim_.finalizeTree();
        state_ = atlas_sim_.getAtlasState();
    }

    atlas::AtlasState* getAtlasState() { return state_; }

  private:
    sparta::Scheduler scheduler_;
    atlas::AtlasSim atlas_sim_{&scheduler_, "", 0};
    atlas::AtlasState* state_ = nullptr;
};

TEST_INIT

// Create a random device to generate a seed
std::random_device rd;

// Create a Mersenne Twister engine seeded with the random device
std::mt19937_64 gen(rd());

// Define the range for the random number (0 to UINT64_MAX)
std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

void testIntRegs()
{
    RegisterTester tester;
    atlas::AtlasState* state = tester.getAtlasState();

    // Verify the zero register is zero
    auto x0_reg_val = READ_INT_REG64(atlas::X0);
    EXPECT_EQUAL(x0_reg_val, 0);

    // Verify that writing to the zero register is ignored
    WRITE_INT_REG64(atlas::X0, 1);
    x0_reg_val = READ_INT_REG64(atlas::X0);
    EXPECT_EQUAL(x0_reg_val, 0);

    // Verify that aliases are working
    auto zero_reg_val = READ_INT_REG64(atlas::X0);
    EXPECT_EQUAL(zero_reg_val, x0_reg_val);

    // Verify writable registers
    WRITE_INT_REG64(atlas::X1, 0xdeadbeef);
    auto x1_reg_val = READ_INT_REG64(atlas::X1);
    EXPECT_EQUAL(x1_reg_val, 0xdeadbeef);
}

/*
void testFpRegs()
{
    RegisterTester tester;
    atlas::AtlasState* state = tester.getAtlasState();

    // Generate a random uint64_t
    uint64_t rand_val = dis(gen);

    // Verify the sp field for the f0 register.
    WRITE_FP_FIELD(atlas::F0, sp, rand_val);
    auto f0_sp_val = READ_FP_FIELD(atlas::F0, sp);
    EXPECT_EQUAL(f0_sp_val, rand_val & 0x00000000ffffffff);

    // Verify the dp field for the f1 register.
    WRITE_FP_FIELD(atlas::F1, dp, rand_val);
    auto f1_dp_val = READ_FP_FIELD(atlas::F1, dp);
    EXPECT_EQUAL(f1_dp_val, rand_val);
}
*/

void testVecRegs()
{
    RegisterTester tester;
    atlas::AtlasState* state = tester.getAtlasState();

    // Generate a random uint64_t
    uint64_t rand_val = dis(gen);

    // Verify the v0 register
    WRITE_VEC_REG(atlas::V0, rand_val);
    auto v0_reg_val = READ_VEC_REG(atlas::V0);
    EXPECT_EQUAL(v0_reg_val, rand_val);
}

void testCsrRegs()
{
    RegisterTester tester;
    atlas::AtlasState* state = tester.getAtlasState();

    // Generate a random uint64_t
    uint64_t rand_val = dis(gen);

    // Verify the ustatus register (all fields writable)
    POKE_CSR_REG(atlas::USTATUS, rand_val);
    auto ustatus_reg_val = READ_CSR_REG(atlas::USTATUS);
    EXPECT_EQUAL(ustatus_reg_val, rand_val);

    // Verify the dmcontrol register. The hasel field is read-only
    // at bit 26 and the hartreset field is writable at bit 29.
    std::bitset<64> reg_val_bits(0);
    reg_val_bits.set(26);
    reg_val_bits.set(29);

    POKE_CSR_REG(atlas::DMCONTROL, reg_val_bits.to_ullong());
    auto dmcontrol_reg_val = READ_CSR_REG(atlas::DMCONTROL);
    EXPECT_EQUAL(dmcontrol_reg_val, reg_val_bits.to_ullong());

    // Try to overwrite both the read-only and writable fields
    reg_val_bits.reset(26);
    reg_val_bits.reset(29);
    WRITE_CSR_REG(atlas::DMCONTROL, reg_val_bits.to_ullong());

    // We expect the read-only field to be unchanged (bit 26) and
    // the writable field to be changed (bit 29)
    dmcontrol_reg_val = READ_CSR_REG(atlas::DMCONTROL);
    std::bitset<64> new_dmcontrol_reg_val_bits(dmcontrol_reg_val);
    EXPECT_TRUE(new_dmcontrol_reg_val_bits.test(26));
    EXPECT_FALSE(new_dmcontrol_reg_val_bits.test(29));

    // Now we will write to the hartsello field, which differs from
    // the fields used above in that it is a >1 bit field. It spans
    // bits 16-25.
    //
    // We need a separate test for multi-bit fields since the mask
    // impl is different for single- vs. multi-bit fields.
    reg_val_bits.reset();
    for (int i = 16; i <= 25; i++)
    {
        reg_val_bits.set(i);
    }

    POKE_CSR_REG(atlas::DMCONTROL, reg_val_bits.to_ullong());
    dmcontrol_reg_val = READ_CSR_REG(atlas::DMCONTROL);
    EXPECT_EQUAL(dmcontrol_reg_val, reg_val_bits.to_ullong());

    reg_val_bits.reset();
    WRITE_CSR_REG(atlas::DMCONTROL, 0);

    dmcontrol_reg_val = READ_CSR_REG(atlas::DMCONTROL);
    new_dmcontrol_reg_val_bits = std::bitset<64>(dmcontrol_reg_val);
    for (int i = 16; i <= 25; i++)
    {
        EXPECT_FALSE(new_dmcontrol_reg_val_bits.test(i));
    }

    // Now let's try to write to a read-only, multi-bit field. The
    // DMSTATUS register's "version" field is read-only from bit
    // positions 0-3.
    reg_val_bits.reset();
    for (int i = 0; i <= 3; i++)
    {
        reg_val_bits.set(i);
    }

    POKE_CSR_REG(atlas::DMSTATUS, reg_val_bits.to_ullong());
    auto dmstatus_reg_val = READ_CSR_REG(atlas::DMSTATUS);
    EXPECT_EQUAL(dmstatus_reg_val, reg_val_bits.to_ullong());

    reg_val_bits.reset();
    WRITE_CSR_REG(atlas::DMSTATUS, 0);

    dmstatus_reg_val = READ_CSR_REG(atlas::DMSTATUS);
    new_dmcontrol_reg_val_bits = std::bitset<64>(dmstatus_reg_val);
    for (int i = 0; i <= 3; i++)
    {
        EXPECT_TRUE(new_dmcontrol_reg_val_bits.test(i));
    }

    // Test the WRITE_CSR_FIELD and READ_CSR_FIELD macros.
    // Case 1: Writable single-bit field (DMCONTROL.hartreset)
    POKE_CSR_REG(atlas::DMCONTROL, 0);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::DMCONTROL, hartreset), 0);

    WRITE_CSR_FIELD(atlas::DMCONTROL, hartreset, 1);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::DMCONTROL, hartreset), 1);

    // Case 2: Read-only single-bit field (DMCONTROL.hasel)
    WRITE_CSR_FIELD(atlas::DMCONTROL, hasel, 1);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::DMCONTROL, hasel), 0);

    // Case 3: Writable multi-bit field (SSTATUS.XS, bits 15-16)
    reg_val_bits.reset();
    reg_val_bits.set(15);
    reg_val_bits.set(16);
    POKE_CSR_REG(atlas::SSTATUS, reg_val_bits.to_ullong());
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::SSTATUS, XS), 3); // Two bits set, 0x11 = 3

    // Case 4: Read-only multi-bit field (SSTATUS.WPRI, bits 2-3)
    reg_val_bits.reset();
    reg_val_bits.set(2);
    reg_val_bits.set(3);
    POKE_CSR_REG(atlas::SSTATUS, reg_val_bits.to_ullong());
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::SSTATUS, WPRI), 3); // Two bits set, 0x11 = 3
    reg_val_bits.reset();
    WRITE_CSR_REG(atlas::SSTATUS, reg_val_bits.to_ullong());
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::SSTATUS, WPRI), 3); // Field is read-only, should not change
    WRITE_CSR_FIELD(atlas::SSTATUS, WPRI, 2);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::SSTATUS, WPRI), 3); // Field is read-only, should not change

    // Case 5: Write a combination of read-only and writable fields all at
    // once using the WRITE_CSR_REG macro. We will do this for the DMSTATUS
    // register focusing on these fields:
    //
    //     authbusy (writable, bit position 6)
    //     authenticated (read-only, bit position 7)
    reg_val_bits.reset();
    reg_val_bits.set(6);
    reg_val_bits.set(7);
    POKE_CSR_REG(atlas::DMSTATUS, reg_val_bits.to_ullong());

    // We will try to overwrite both 1 bits to 0.
    reg_val_bits.reset();
    WRITE_CSR_REG(atlas::DMSTATUS, reg_val_bits.to_ullong());

    // Verify that the write operation only changed the authbusy field
    // at bit position 6 and left the authenticated field at bit position 7
    // unchanged.
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::DMSTATUS, authbusy), 0);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::DMSTATUS, authenticated), 1);
    EXPECT_EQUAL(READ_CSR_REG(atlas::DMSTATUS), 0x2 << 6);

    // Case 6: Write a combination of read-only and writable fields,
    // where the fields are both multiple bits. We will do this for the
    // MVENDORID register focusing on these fields:
    //
    //     Bank (writable, bits 7-31)       --> start with 0xf
    //     Offset (read-only, bits 0-6)     --> start with 0xf
    POKE_CSR_REG(atlas::MVENDORID, 0xf << 7 | 0xf);

    // To to overwrite both fields with 0xe
    WRITE_CSR_REG(atlas::MVENDORID, 0xe << 7 | 0xe);

    // Verify that only the Bank field was changed and the Offset field
    // was left unchanged.
    auto expected_mvendid_val = 0xe << 7 | 0xf;
    auto mvendid_val = READ_CSR_REG(atlas::MVENDORID);
    EXPECT_EQUAL(mvendid_val, expected_mvendid_val);

    EXPECT_EQUAL(READ_CSR_FIELD(atlas::MVENDORID, Bank), 0xe);
    EXPECT_EQUAL(READ_CSR_FIELD(atlas::MVENDORID, Offset), 0xf);
}

int main()
{
    testIntRegs();
    // testFpRegs();
    testVecRegs();
    testCsrRegs();

    REPORT_ERROR;
    return ERROR_CODE;
}
