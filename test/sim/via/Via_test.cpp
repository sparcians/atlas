#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class ViaInstructionTester : public PegasusInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    ViaInstructionTester() = default;

    void testVaddvv1()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 3, vs1 = 1, vs2 = 2;
        uint32_t opcode;

        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);   // avl = 8
        state->getVectorConfig()->setLMUL(8); // vlmul = 1
        state->getVectorConfig()->setSEW(8);  // sew = 8

        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {1, 3, 5, 7, 9, 11, 13, 15};
        WRITE_VEC_REG<VLEN>(state, vs1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val);
        opcode = vaddvvOp(vd, vs1, vs2, 1); // unmasked
        injectInstruction(pc, opcode);

        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            EXPECT_EQUAL(vd_val[i], vs3_val[i]);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVaddvv2()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        uint32_t opcode;

        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(16);   // avl = 16
        state->getVectorConfig()->setLMUL(16); // vlmul = 2
        state->getVectorConfig()->setSEW(8);   // sew = 8

        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {9, 10, 11, 12, 13, 14, 15, 16};
        VLEN vs4_val = {17, 18, 19, 20, 21, 22, 23, 24};
        VLEN vs5_val = {9, 11, 13, 15, 17, 19, 21, 23};
        VLEN vs6_val = {18, 20, 22, 24, 26, 28, 30, 32};
        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, 3, vs3_val);
        WRITE_VEC_REG<VLEN>(state, 4, vs4_val);
        opcode = vaddvvOp(5, 1, 3, 1); // unmasked
        injectInstruction(pc, opcode);

        auto vd_val1 = READ_VEC_REG<VLEN>(state, 5);
        for (size_t i = 0; i < vd_val1.size(); ++i)
        {
            EXPECT_EQUAL(vd_val1[i], vs5_val[i]);
        }
        auto vd_val2 = READ_VEC_REG<VLEN>(state, 6);
        for (size_t i = 0; i < vd_val2.size(); ++i)
        {
            EXPECT_EQUAL(vd_val2[i], vs6_val[i]);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVaddvv3()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        uint32_t opcode;

        state->getVectorConfig()->setVSTART(2); // vstart = 2
        state->getVectorConfig()->setVL(14);    // avl = 14
        state->getVectorConfig()->setLMUL(16);  // vlmul = 2
        state->getVectorConfig()->setSEW(8);    // sew = 8

        VLEN rst_val = {0, 0, 0, 0, 0, 0, 0, 0};
        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {9, 10, 11, 12, 13, 14, 15, 16};
        VLEN vs4_val = {17, 18, 19, 20, 21, 22, 23, 24};
        VLEN vs5_val = {9, 11, 13, 15, 17, 19, 21, 23};
        VLEN vs6_val = {18, 20, 22, 24, 26, 28, 30, 32};

        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, 3, vs3_val);
        WRITE_VEC_REG<VLEN>(state, 4, vs4_val);
        WRITE_VEC_REG<VLEN>(state, 5, rst_val);
        WRITE_VEC_REG<VLEN>(state, 6, rst_val);
        opcode = vaddvvOp(5, 1, 3, 1); // unmasked
        injectInstruction(pc, opcode);

        auto vd_val1 = READ_VEC_REG<VLEN>(state, 5);
        for (size_t i = 0; i < 2; ++i)
        {
            EXPECT_EQUAL(vd_val1[i], 0);
        }
        for (size_t i = 2; i < vd_val1.size(); ++i)
        {
            EXPECT_EQUAL(vd_val1[i], vs5_val[i]);
        }

        auto vd_val2 = READ_VEC_REG<VLEN>(state, 6);
        for (size_t i = 0; i < 6; ++i)
        {
            EXPECT_EQUAL(vd_val2[i], vs6_val[i]);
        }
        for (size_t i = 6; i < vd_val2.size(); ++i)
        {
            EXPECT_EQUAL(vd_val2[i], 0);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3);
    }

    void testVaddvv4()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        uint32_t opcode;

        state->getVectorConfig()->setVSTART(2); // vstart = 2
        state->getVectorConfig()->setVL(14);    // avl = 14
        state->getVectorConfig()->setLMUL(16);  // vlmul = 2
        state->getVectorConfig()->setSEW(8);    // sew = 8

        VLEN rst_val = {0, 0, 0, 0, 0, 0, 0, 0};
        VLEN vs0_val = {0xF0, 0x0F, 0, 0, 0, 0, 0, 0}; // mask first and last 2
        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {9, 10, 11, 12, 13, 14, 15, 16};
        VLEN vs4_val = {17, 18, 19, 20, 21, 22, 23, 24};
        VLEN vs5_val = {9, 11, 13, 15, 17, 19, 21, 23};
        VLEN vs6_val = {18, 20, 22, 24, 26, 28, 30, 32};

        WRITE_VEC_REG<VLEN>(state, 0, vs0_val);
        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        WRITE_VEC_REG<VLEN>(state, 3, vs3_val);
        WRITE_VEC_REG<VLEN>(state, 4, vs4_val);
        WRITE_VEC_REG<VLEN>(state, 5, rst_val);
        WRITE_VEC_REG<VLEN>(state, 6, rst_val);
        opcode = vaddvvOp(5, 1, 3, 0); // masked
        injectInstruction(pc, opcode);

        auto vd_val1 = READ_VEC_REG<VLEN>(state, 5);
        for (size_t i = 0; i < 4; ++i)
        {
            EXPECT_EQUAL(vd_val1[i], 0);
        }
        for (size_t i = 4; i < vd_val1.size(); ++i)
        {
            EXPECT_EQUAL(vd_val1[i], vs5_val[i]);
        }

        auto vd_val2 = READ_VEC_REG<VLEN>(state, 6);
        for (size_t i = 0; i < 4; ++i)
        {
            EXPECT_EQUAL(vd_val2[i], vs6_val[i]);
        }
        for (size_t i = 4; i < vd_val2.size(); ++i)
        {
            EXPECT_EQUAL(vd_val2[i], 0);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 4);
    }

    uint32_t vaddvvOp(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset; // opcode
        offset += 7;
        opcode |= rd << offset; // rd
        offset += 5;
        opcode |= 0 << offset;
        offset += 3;
        opcode |= rs1 << offset; // rs1
        offset += 5;
        opcode |= rs2 << offset; // rs2
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        opcode |= 0 << offset;
        offset += 6;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    ViaInstructionTester Via_tester;

    Via_tester.testVaddvv1();
    Via_tester.testVaddvv2();
    Via_tester.testVaddvv3();
    Via_tester.testVaddvv4();

    REPORT_ERROR;
    return ERROR_CODE;
}
