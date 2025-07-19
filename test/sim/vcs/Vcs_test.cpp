#include "test/sim/InstructionTester.hpp"
#include "core/VecElements.hpp"
#include "sparta/utils/SpartaTester.hpp"

class VcsInstructionTester : public PegasusInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    VcsInstructionTester() = default;

    void testVsetvl()
    {
        const uint32_t rs1 = 1, rs2 = 2, rd = 3;
        pegasus::PegasusState* state = getPegasusState();

        WRITE_INT_REG<XLEN>(state, rs1, 8);            // avl = 8
        XLEN vtype = setVtypeCSR<XLEN>(0, 0, 0, 0, 0); // vlmul = 1, sew = 8
        WRITE_INT_REG<XLEN>(state, rs2, vtype);
        instPtr_ = makePegasusInst(std::string{"vsetvl"}, mavis::ExtractorIF::RegListType{rs1, rs2},
                                   mavis::ExtractorIF::RegListType{rd});
        executeInstruction(instPtr_);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 8);
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVsetvli()
    {
        const uint32_t rs1 = 1, rd = 3;
        pegasus::PegasusState* state = getPegasusState();

        WRITE_INT_REG<XLEN>(state, rs1, 8);            // avl = 8
        XLEN vtype = setVtypeCSR<XLEN>(0, 0, 0, 0, 0); // vlmul = 1, sew = 8
        instPtr_ = makePegasusInst(std::string{"vsetvli"}, mavis::ExtractorIF::RegListType{rs1},
                                   mavis::ExtractorIF::RegListType{rd}, vtype);
        executeInstruction(instPtr_);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 8);
        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

    void testVsetivli()
    {
        pegasus::PegasusState* state = getPegasusState();
        const pegasus::Addr pc = 0x1000;
        const uint32_t rd = 3;
        uint32_t opcode;

        opcode = vsetivliOp(rd, 8, 0, 0, 0, 0); // vl = 8, vlmul = 1, sew = 8
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 8);

        opcode = vsetivliOp(rd, 16, 1, 0, 0, 0); // vl = 16, vlmul = 2, sew = 8
        injectInstruction(pc, opcode);

        EXPECT_EQUAL(READ_INT_REG<XLEN>(state, rd), 16);

        const pegasus::PegasusState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 4);
    }

  private:
    uint32_t vsetivliOp(uint8_t rd, uint8_t avl, uint8_t vlmul, uint8_t sew, uint8_t vta,
                        uint8_t vma)
    {
        uint32_t opcode = 0;
        uint8_t offset = 0;
        opcode |= 0x57 << offset;
        offset += 7;
        opcode |= rd << offset;
        offset += 5;
        opcode |= 7 << offset;
        offset += 3;
        opcode |= avl << offset;
        offset += 5;
        opcode |= vlmul << offset;
        offset += 3;
        opcode |= sew << offset;
        offset += 3;
        opcode |= vta << offset;
        offset += 1;
        opcode |= vma << offset;
        offset += 1;
        offset += 2;
        opcode |= 1 << offset;
        offset += 1;
        opcode |= 1 << offset;
        offset += 1;
        EXPECT_EQUAL(offset, 32);
        return opcode;
    }

    pegasus::PegasusInst::PtrType instPtr_ = nullptr;
};

int main()
{
    VcsInstructionTester Vcs_tester;

    Vcs_tester.testVsetvl();
    Vcs_tester.testVsetvli();
    Vcs_tester.testVsetivli();

    REPORT_ERROR;
    return ERROR_CODE;
}
