#include "test/sim/InstructionTester.hpp"
#include "sparta/utils/SpartaTester.hpp"

class VsetInstructionTester : AtlasInstructionTester
{

  public:
    VsetInstructionTester() = default;

    void testVsetvl()
    {
        const uint64_t pc = 0x1000;
        const uint32_t opcode = 0x00000013;
        injectInstruction(pc, opcode);

	const atlas::AtlasState* state = getAtlasState();
	const atlas::AtlasState::SimState* sim_state = state->getSimState();

	std::cout << sim_state->current_inst << std::endl;
	EXPECT_EQUAL(sim_state->inst_count, 1);
    }

  private:
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    VsetInstructionTester vset_tester;

    vset_tester.testVsetvl();

    REPORT_ERROR;
    return (int)ERROR_CODE;
}
