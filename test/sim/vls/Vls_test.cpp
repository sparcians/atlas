#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "mavis/Mavis.h"

class VlsInstructionTester : public PegasusInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    VlsInstructionTester() = default;

    void testVle8()
    {
        pegasus::PegasusState* state = getPegasusState();
        const uint64_t pc = 0x1000;
        const pegasus::Addr addr = 0x2000;
        const uint64_t v1_val = 0x0807060504030201;
        const uint32_t vd = 1, rs1 = 1;

        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8); // avl = 8
        state->writeMemory<uint64_t>(addr, v1_val);
        WRITE_INT_REG<XLEN>(state, rs1, addr);

        uint32_t opcode = vle8Op(vd, rs1, 1); // vm = 1 unmasked
        injectInstruction(pc, opcode);

        auto vd_val = READ_VEC_REG<VLEN>(state, vd);
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            EXPECT_EQUAL(vd_val[i], reinterpret_cast<const uint8_t*>(&v1_val)[i]);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVlse8()
    {
        pegasus::PegasusState* state = getPegasusState();
        const uint64_t pc = 0x1000;
        const pegasus::Addr addr = 0x2000;
        const pegasus::Addr stride = 0x10; // 16-byte stride
        const uint32_t vd = 1, rs1 = 1, rs2 = 2;
        VLEN vd_val;

        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8); // avl = 8
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            state->writeMemory<uint8_t>(addr + i * stride, i);
        }
        WRITE_INT_REG<XLEN>(state, rs1, addr);
        WRITE_INT_REG<XLEN>(state, rs2, stride);

        uint32_t opcode = vlse8Op(vd, rs1, rs2, 1); // vm = 1 unmasked
        injectInstruction(pc, opcode);

        vd_val = READ_VEC_REG<VLEN>(state, vd);
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            EXPECT_EQUAL(vd_val[i], i);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVloxei8()
    {
        pegasus::PegasusState* state = getPegasusState();
        const uint64_t pc = 0x1000;
        const pegasus::Addr addr = 0x2000;
        const uint32_t vd = 1, rs1 = 1, vs2 = 3;
        const VLEN vs2_val = {0, 2, 4, 6, 8, 10, 12, 14};
        VLEN vd_val1, vd_val2;

        state->getVectorConfig()->setVSTART(0);
        state->getVectorConfig()->setVL(8);    // avl = 8
        state->getVectorConfig()->setSEW(16);  // sew = 16
        state->getVectorConfig()->setLMUL(16); // lmul = 2
        for (size_t i = 0; i < vs2_val.size(); ++i)
        {
            state->writeMemory<uint16_t>(addr + vs2_val[i], i);
        }
        WRITE_INT_REG<XLEN>(state, rs1, addr);
        WRITE_VEC_REG<VLEN>(state, vs2, vs2_val);

        uint32_t opcode = vloxei8Op(vd, rs1, vs2, 1); // vm = 1 unmasked
        injectInstruction(pc, opcode);

        vd_val1 = READ_VEC_REG<VLEN>(state, vd);
        for (size_t i = 0; i < vd_val1.size() / 2; ++i)
        {
            EXPECT_EQUAL(reinterpret_cast<uint16_t*>(&vd_val1)[i], i);
        }
        vd_val2 = READ_VEC_REG<VLEN>(state, vd + 1);
        for (size_t i = 0; i < vd_val1.size() / 2; ++i)
        {
            EXPECT_EQUAL(reinterpret_cast<uint16_t*>(&vd_val2)[i], i + 4);
        }
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 3);
    }

    uint32_t vle8Op(uint8_t rd, uint8_t rs1, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x7 << offset; // opcode
        offset += 7;
        opcode |= rd << offset; // rd
        offset += 5;
        opcode |= 0 << offset; // width
        offset += 3;
        opcode |= rs1 << offset; // rs1
        offset += 5;
        opcode |= 0 << offset; // rs2
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        opcode |= 0 << offset; // newop
        offset += 3;
        opcode |= 0 << offset; // nf
        offset += 3;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

    uint32_t vlse8Op(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x7 << offset; // opcode
        offset += 7;
        opcode |= rd << offset; // rd
        offset += 5;
        opcode |= 0 << offset; // width
        offset += 3;
        opcode |= rs1 << offset; // rs1
        offset += 5;
        opcode |= rs2 << offset; // rs2
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        opcode |= 2 << offset; // newop
        offset += 3;
        opcode |= 0 << offset; // nf
        offset += 3;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

    uint32_t vloxei8Op(uint8_t rd, uint8_t rs1, uint8_t vs2, uint8_t vm)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x7 << offset; // opcode
        offset += 7;
        opcode |= rd << offset; // rd
        offset += 5;
        opcode |= 0 << offset; // width
        offset += 3;
        opcode |= rs1 << offset; // rs1
        offset += 5;
        opcode |= vs2 << offset; // rs2
        offset += 5;
        opcode |= vm << offset; // vm
        offset += 1;
        opcode |= 3 << offset; // newop
        offset += 3;
        opcode |= 0 << offset; // nf
        offset += 3;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

  private:
    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VlsInstructionTester Vls_tester;

    Vls_tester.testVle8();
    Vls_tester.testVlse8();
    Vls_tester.testVloxei8();

    REPORT_ERROR;
    return ERROR_CODE;
}
