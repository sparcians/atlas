#include "cosim/AtlasCoSim.hpp"
#include "sparta/utils/SpartaTester.hpp"

void testSingleStepNop()
{
    const uint64_t ilimit = 0;
    sparta::Scheduler scheduler;
    atlas::AtlasCoSim cosim(&scheduler, ilimit);
    cosim.enableLogger();

    atlas::AtlasState* state = cosim.getAtlasState();
    const atlas::AtlasState::SimState* sim_state = state->getSimState();

    // Load a nop instruction into memory
    const atlas::HartId hart_id = 0;
    const atlas::Addr paddr = 0x1000;
    uint64_t opcode = 0x13; // nop
    std::vector<uint8_t> buffer(sizeof(opcode));
    std::memcpy(buffer.data(), &opcode, sizeof(opcode));
    const bool success = cosim.getMemoryInterface()->poke(hart_id, paddr, buffer);
    EXPECT_EQUAL(success, true);

    // Set starting PC to address of nop instruction
    cosim.setPc(hart_id, paddr);

    // Validate AtlasState before stepping
    EXPECT_EQUAL(state->getPc(), 0x1000);
    EXPECT_EQUAL(sim_state->inst_count, 0);

    // Step Atlas
    const atlas::cosim::Event event = cosim.step(hart_id);

    // Validate Event
    EXPECT_EQUAL(event.getEuid(), 1);
    EXPECT_EQUAL(event.getEventType(), atlas::cosim::Event::Type::INSTRUCTION);
    EXPECT_EQUAL(event.getHartId(), hart_id);
    EXPECT_EQUAL(event.isDone(), true);
    // EXPECT_EQUAL(event.isLastEvent(), false);
    // EXPECT_EQUAL(event.isEventInRoi(), false);
    // EXPECT_EQUAL(event.isEventEnteringRoi(), false);
    // EXPECT_EQUAL(event.isEventExitingRoi(), false);
    EXPECT_EQUAL(event.getArchId(), 1);
    EXPECT_EQUAL(event.getOpcode(), opcode);
    EXPECT_EQUAL(event.getOpcodeSize(), 4);
    // EXPECT_EQUAL(event.getInstType(), atlas::InstType);
    // EXPECT_EQUAL(event.isChangeOfFlowEvent(), false);
    EXPECT_EQUAL(event.getPc(), paddr);
    EXPECT_EQUAL(event.getNextPc(), 0x1004);
    // EXPECT_EQUAL(event.getAltNextPc(), 0);
    // EXPECT_EQUAL(event.getPrivilegeMode(), atlas::PrivMode::MACHINE);
    // EXPECT_EQUAL(event.getNextPrivilegeMode(), atlas::PrivMode::MACHINE);
    // EXPECT_EQUAL(event.getExceptionType(), atlas::ExcpType::NONE);
    // EXPECT_EQUAL(event.getExceptionCode(), 0);

    // Validate CoSim
    EXPECT_EQUAL(cosim.getNumCommittedEvents(hart_id), 0);
    EXPECT_EQUAL(cosim.getLastCommittedEvent(hart_id).getEventType(),
                 atlas::cosim::Event::Type::INVALID);
    EXPECT_EQUAL(cosim.getNumUncommittedEvents(hart_id), 1);

    // Validate AtlasState
    EXPECT_EQUAL(state->getPc(), 0x1004);
    EXPECT_EQUAL(sim_state->inst_count, 1);

    // Commit Event
    cosim.commit(hart_id);

    // Validate CoSim
    EXPECT_EQUAL(cosim.getNumCommittedEvents(hart_id), 1);
    EXPECT_EQUAL(cosim.getLastCommittedEvent(hart_id).getEuid(), event.getEuid());
    EXPECT_EQUAL(cosim.getNumUncommittedEvents(hart_id), 0);
}

void testSingleStepAdd()
{
    const uint64_t ilimit = 0;
    sparta::Scheduler scheduler;
    atlas::AtlasCoSim cosim(&scheduler, ilimit);
    cosim.enableLogger();

    atlas::AtlasState* state = cosim.getAtlasState();
    const atlas::AtlasState::SimState* sim_state = state->getSimState();

    // Load an add instruction into memory
    const atlas::HartId hart_id = 0;
    const atlas::Addr paddr = 0x1000;
    uint64_t opcode = 0x002081b3; // add x3, x1, x2
    std::vector<uint8_t> buffer(sizeof(opcode));
    std::memcpy(buffer.data(), &opcode, sizeof(opcode));
    const bool success = cosim.getMemoryInterface()->poke(hart_id, paddr, buffer);
    EXPECT_EQUAL(success, true);

    // TODO: Initialize register values

    // Set starting PC to address of nop instruction
    cosim.setPc(hart_id, paddr);

    // Validate AtlasState before stepping
    EXPECT_EQUAL(state->getPc(), 0x1000);
    EXPECT_EQUAL(sim_state->inst_count, 0);

    // Validate AtlasState before stepping
    EXPECT_EQUAL(state->getPc(), 0x1000);
    EXPECT_EQUAL(sim_state->inst_count, 0);

    // Step Atlas
    const atlas::cosim::Event event = cosim.step(hart_id);

    // Validate Event
    EXPECT_EQUAL(event.getEuid(), 1);
    EXPECT_EQUAL(event.getEventType(), atlas::cosim::Event::Type::INSTRUCTION);
    EXPECT_EQUAL(event.getHartId(), hart_id);
    EXPECT_EQUAL(event.isDone(), true);
    // EXPECT_EQUAL(event.isLastEvent(), false);
    // EXPECT_EQUAL(event.isEventInRoi(), false);
    // EXPECT_EQUAL(event.isEventEnteringRoi(), false);
    // EXPECT_EQUAL(event.isEventExitingRoi(), false);
    EXPECT_EQUAL(event.getArchId(), 1);
    EXPECT_EQUAL(event.getOpcode(), opcode);
    EXPECT_EQUAL(event.getOpcodeSize(), 4);
    // EXPECT_EQUAL(event.getInstType(), atlas::InstType);
    // EXPECT_EQUAL(event.isChangeOfFlowEvent(), false);
    EXPECT_EQUAL(event.getPc(), paddr);
    EXPECT_EQUAL(event.getNextPc(), 0x1004);
    // EXPECT_EQUAL(event.getAltNextPc(), 0);
    // EXPECT_EQUAL(event.getPrivilegeMode(), atlas::PrivMode::MACHINE);
    // EXPECT_EQUAL(event.getNextPrivilegeMode(), atlas::PrivMode::MACHINE);
    // EXPECT_EQUAL(event.getExceptionType(), atlas::ExcpType::NONE);
    // EXPECT_EQUAL(event.getExceptionCode(), 0);

    // TODO: Validate register values

    // Validate CoSim
    EXPECT_EQUAL(cosim.getNumCommittedEvents(hart_id), 0);
    EXPECT_EQUAL(cosim.getLastCommittedEvent(hart_id).getEventType(),
                 atlas::cosim::Event::Type::INVALID);
    EXPECT_EQUAL(cosim.getNumUncommittedEvents(hart_id), 1);

    // Validate AtlasState
    EXPECT_EQUAL(state->getPc(), 0x1004);
    EXPECT_EQUAL(sim_state->inst_count, 1);

    // Commit Event
    cosim.commit(hart_id);

    // Validate CoSim
    EXPECT_EQUAL(cosim.getNumCommittedEvents(hart_id), 1);
    EXPECT_EQUAL(cosim.getLastCommittedEvent(hart_id).getEuid(), event.getEuid());
    EXPECT_EQUAL(cosim.getNumUncommittedEvents(hart_id), 0);
}

int main()
{
    // Step a single nop instruction
    testSingleStepNop();

    // Step a single add instruction
    testSingleStepAdd();

    // TODO: test a load, branch, and system call

    return 0;
}
