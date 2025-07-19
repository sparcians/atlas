#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class VredInstructionTester : public PegasusInstructionTester
{
  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;
    using WLEN = int16_t; // 16-bit widened accumulator/result

    VredInstructionTester() = default;

    void testVredsumvs1()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10; // scalar destination (reduction result)
        const uint32_t vs1 = 1; // reduction initial value (accumulator)
        const uint32_t vs2 = 2; // vector source
        uint32_t opcode;

        // Configure vector unit
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);   // VL = 8 elements
        state->getVectorConfig()->setLMUL(1); // LMUL = 1
        state->getVectorConfig()->setSEW(8);  // SEW = 8 bits

        // Initial values
        VLEN vs1_val = {10, 0, 0, 0, 0, 0, 0, 0}; // Start with scalar accumulator = 10
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        XLEN expected_sum = vs1_val[0];
        for (auto val : vs2_val)
            expected_sum += val;

        // Write input values
        WRITE_VEC_REG<VLEN>(state, vs1, vs1_val); // vector register
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val); // vector register

        // Encode and inject vredsum.vs
        opcode = vredsumvsOp(vd, vs1, vs2, 1, false); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Read result
        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVwredsumvs1()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t vd = 10; // destination scalar (wide result)
        const uint32_t vs1 = 1; // wide scalar accumulator
        const uint32_t vs2 = 2; // vector input (narrow)
        uint32_t opcode;

        // Configure vector unit
        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);   // VL = 8 elements
        state->getVectorConfig()->setLMUL(1); // LMUL = 1
        state->getVectorConfig()->setSEW(8);  // SEW = 8-bit

        // Set input values
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};  // vector of int8_t
        VLEN vs1_val = {10, 0, 0, 0, 0, 0, 0, 0}; // initial accumulator
        WLEN expected_sum = vs1_val[0];
        for (auto val : vs2_val)
            expected_sum += static_cast<WLEN>(val);

        // Write inputs
        WRITE_VEC_REG<VLEN>(state, vs1, vs1_val); // scalar accumulator
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val); // vector input

        // Encode and inject instruction
        opcode = vredsumvsOp(vd, vs1, vs2, 1, true); // vm = 1 (unmasked)
        injectInstruction(pc, opcode);

        // Validate result
        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        EXPECT_EQUAL(vd_val[0], expected_sum);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    // vredsum.vs encoding helper
    uint32_t vredsumvsOp(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm, bool isWideningEnabled)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset; // opcode = 0x57 (vector)
        offset += 7;
        opcode |= rd << offset; // rd (scalar result)
        offset += 5;
        opcode |= 0 << offset; // funct3 = 000
        offset += 3;
        opcode |= rs1 << offset; // rs1 (accumulator)
        offset += 5;
        opcode |= rs2 << offset; // rs2 (vector source)
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        if (isWideningEnabled)
            opcode |= 0x26 << offset; // funct6 = 100110 for vwredsum.vs
        else
            opcode |= 0x06 << offset; // funct6 = 000110 for vredsum.vs
        offset += 6;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VredInstructionTester tester;
    tester.testVredsumvs1();
    tester.testVwredsumvs1();

    REPORT_ERROR;
    return ERROR_CODE;
}
