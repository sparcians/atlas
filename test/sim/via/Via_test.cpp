#include "test/sim/InstructionTester.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "core/VectorState.hpp"
#include "mavis/Mavis.h"

class ViaInstructionTester : public AtlasInstructionTester
{

  public:
    using VLEN = std::array<uint8_t, 8>;
    using XLEN = uint64_t;

    ViaInstructionTester() = default;

    void testVaddvv1()
    {
        atlas::AtlasState* state = getAtlasState();

        state->getVectorState()->setVSTART(0);
        state->getVectorState()->setVL(8);   // avl = 8
        state->getVectorState()->setLMUL(8); // vlmul = 1
        state->getVectorState()->setSEW(8);  // sew = 8

        VLEN vs1_val = {0, 1, 2, 3, 4, 5, 6, 7};
        VLEN vs2_val = {1, 2, 3, 4, 5, 6, 7, 8};
        VLEN vs3_val = {1, 3, 5, 7, 9, 11, 13, 15};
        WRITE_VEC_REG<VLEN>(state, 1, vs1_val);
        WRITE_VEC_REG<VLEN>(state, 2, vs2_val);
        instPtr_ = makeAtlasInst(std::string{"vadd.vv"}, mavis::ExtractorIF::RegListType{1, 2},
                                 mavis::ExtractorIF::RegListType{3}); // vd = 3, vs1 = 1, vs2 = 2
        executeInstruction(instPtr_);

        auto vd_val = READ_VEC_REG<VLEN>(state, 3);
        for (size_t i = 0; i < vd_val.size(); ++i)
        {
            EXPECT_EQUAL(vd_val[i], vs3_val[i]);
        }
        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 1);
    }

    void testVaddvv2()
    {
        atlas::AtlasState* state = getAtlasState();

        state->getVectorState()->setVSTART(0);
        state->getVectorState()->setVL(16);   // avl = 16
        state->getVectorState()->setLMUL(16); // vlmul = 2
        state->getVectorState()->setSEW(8);   // sew = 8

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
        instPtr_ = makeAtlasInst(std::string{"vadd.vv"}, mavis::ExtractorIF::RegListType{1, 3},
                                 mavis::ExtractorIF::RegListType{5}); // vd = 5, vs1 = 1, vs2 = 3
        executeInstruction(instPtr_);

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
        const atlas::AtlasState::SimState* sim_state = state->getSimState();
        std::cout << sim_state->current_inst << std::endl;
        EXPECT_EQUAL(sim_state->inst_count, 2);
    }

  private:
    atlas::AtlasInst::PtrType instPtr_ = nullptr;
};

int main()
{
    ViaInstructionTester Via_tester;

    Via_tester.testVaddvv1();
    Via_tester.testVaddvv2();

    REPORT_ERROR;
    return ERROR_CODE;
}
