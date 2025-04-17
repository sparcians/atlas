#include "cosim/AtlasCoSim.hpp"
#include "core/Fetch.hpp"
#include "core/observers/CoSimObserver.hpp"
#include "include/ActionTags.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/SpartaAssert.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
#ifndef COSIMLOG
#define COSIMLOG(msg) SPARTA_LOG(cosim_logger_, msg)
#endif

    CoSimMemoryInterface::CoSimMemoryInterface(sparta::memory::SimpleMemoryMapNode* memory) :
        memory_(memory)
    {
    }

    bool CoSimMemoryInterface::peek(HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryPeek(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::read(HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryRead(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::poke(HartId, Addr paddr, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryPoke(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::write(HartId, Addr paddr, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryWrite(paddr, size, buffer.data());
        return success;
    }

    AtlasCoSim::AtlasCoSim(sparta::Scheduler* scheduler, const std::string & workload,
                           uint64_t ilimit) :
        AtlasSim(scheduler, workload, ilimit),
        cosim_logger_(getRoot(), "cosim", "Atlas Cosim Logger")

    {
        buildTree();     // Calls AtlasSim::buildTree_
        configureTree(); // Calls AtlasSim::configureTree_
        finalizeTree();  // Calls AtlasCoSim::bindTree_
    }

    AtlasCoSim::~AtlasCoSim() { getRoot()->enterTeardown(); }

    void AtlasCoSim::bindTree_()
    {
        AtlasSim::bindTree_();

        const uint32_t num_harts = 1;
        for (uint32_t hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            // Get Fetch for each hart
            const std::string core_name = "core" + std::to_string(hart_id);
            fetch_.emplace_back(
                getRoot()->getChild(core_name + ".fetch")->getResourceAs<atlas::Fetch>());

            // Create and attach CoSimObserver to AtlasState for each hart
            auto cosim_obs = std::make_unique<CoSimObserver>();
            cosim_observer_.emplace_back(cosim_obs.get());
            state_.at(hart_id)->addObserver(std::move(cosim_obs));
        }

        event_list_.resize(num_harts);
        last_committed_event_.resize(num_harts, cosim::Event(-1, cosim::Event::Type::INVALID));

        // Single memory IF for all harts
        cosim_memory_if_ = new CoSimMemoryInterface(getAtlasSystem()->getSystemMemory());
    }

    cosim::Event AtlasCoSim::step(HartId hart_id)
    {
        ActionGroup* next_action_group = fetch_.at(hart_id)->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state_.at(hart_id));
        } while (next_action_group && (next_action_group->hasTag(ActionTags::FETCH_TAG) == false));

        const cosim::Event event = cosim_observer_.at(hart_id)->getLastEvent();
        event_list_.at(hart_id).emplace_back(event);
        COSIMLOG(event);
        if (event.getRegisterReads().empty() == false)
        {
            COSIMLOG("    " << event.getRegisterReads());
        }
        if (event.getRegisterWrites().empty() == false)
        {
            COSIMLOG("    " << event.getRegisterWrites());
        }
        return event_list_.at(hart_id).back();
    }

    cosim::Event AtlasCoSim::step(HartId hart_id, Addr addr)
    {
        setPc(hart_id, addr);
        return step(hart_id);
    }

    cosim::Event AtlasCoSim::stepOperation(HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    cosim::Event AtlasCoSim::stepOperation(HartId, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::commit(HartId hart_id)
    {
        const cosim::Event & event = event_list_.at(hart_id).front();
        // COSIMLOG(event);
        sparta_assert(event.isDone(), "Cannot commit an event that isn't done! " << event);

        last_committed_event_.at(hart_id) = event;
        event_list_.at(hart_id).pop_front();
        // TODO: commit store writes
    }

    void AtlasCoSim::commit(const cosim::Event* event)
    {
        const HartId hart_id = event->getHartId();

        // Find event in the event list
        const cosim::EventList & event_list = event_list_.at(hart_id);
        auto it = std::find_if(event_list.begin(), event_list.end(), [event](const cosim::Event & e)
                               { return e.getEuid() == event->getEuid(); });
        sparta_assert(it != event_list.end(), "Event not found in event list!");

        // Commit all events up to and including the event
        const uint32_t num_events_to_commit = std::distance(event_list.begin(), std::next(it));
        for (uint32_t i = 0; i < num_events_to_commit; ++i)
        {
            commit(hart_id);
        }
        COSIMLOG("Number of events committed: " << num_events_to_commit);
    }

    void AtlasCoSim::commitStoreWrite(const cosim::Event*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::commitStoreWrite(const cosim::Event*, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::dropStoreWrite(const cosim::Event*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::dropStoreWrite(const cosim::Event*, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::flush(const cosim::Event*, bool)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    cosim::MemoryInterface* AtlasCoSim::getMemoryInterface() { return cosim_memory_if_; }

    void AtlasCoSim::setMemoryInterface(cosim::MemoryInterface*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::readRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::peekRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::writeRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::pokeRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void AtlasCoSim::setPc(HartId hart_id, Addr addr)
    {
        // TODO: Create Event for PC override
        state_.at(hart_id)->setPc(addr);
    }

    Addr AtlasCoSim::getPc(HartId hart_id) const { return state_.at(hart_id)->getPc(); }

    void AtlasCoSim::setPrivilegeMode(HartId, PrivMode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    PrivMode AtlasCoSim::getPrivilegeMode(HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    bool AtlasCoSim::isSimulationFinished(HartId hart_id) const
    {
        const AtlasState::SimState* sim_state = state_[hart_id]->getSimState();
        return sim_state->sim_stopped;
    }

    cosim::Event AtlasCoSim::injectInstruction(HartId, Opcode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    cosim::Event AtlasCoSim::injectInterrupt(HartId, uint64_t)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    cosim::Event AtlasCoSim::injectReset(HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    uint64_t AtlasCoSim::getNumCommittedEvents(HartId hart_id) const
    {
        return state_.at(hart_id)->getSimState()->inst_count - getNumUncommittedEvents(hart_id);
    }

    const cosim::Event & AtlasCoSim::getLastCommittedEvent(HartId hart_id) const
    {
        return last_committed_event_.at(hart_id);
    }

    const cosim::EventList & AtlasCoSim::getUncommittedEvents(HartId hart_id) const
    {
        return event_list_.at(hart_id);
    }

    uint64_t AtlasCoSim::getNumUncommittedEvents(HartId hart_id) const
    {
        return event_list_.at(hart_id).size();
    }

    uint64_t AtlasCoSim::getNumUncommittedWrites(HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }
} // namespace atlas
