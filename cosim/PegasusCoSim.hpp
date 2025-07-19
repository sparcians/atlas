#include "sim/PegasusSim.hpp"
#include "cosim/CoSimApi.hpp"

namespace pegasus
{
    class CoSimMemoryInterface : public cosim::MemoryInterface
    {
      public:
        CoSimMemoryInterface(sparta::memory::SimpleMemoryMapNode* memory_map);

        ~CoSimMemoryInterface() {}

        bool peek(HartId hart, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool read(HartId hart, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool poke(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const override;
        bool write(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const override;

      private:
        sparta::memory::SimpleMemoryMapNode* memory_ = nullptr;
    };

    class Fetch;
    class CoSimObserver;

    class PegasusCoSim : public PegasusSim, public pegasus::cosim::CoSim
    {
      public:
        PegasusCoSim(sparta::Scheduler* scheduler, uint64_t ilimit);
        ~PegasusCoSim();

        void enableLogger(const std::string & filename = "")
        {
            if (!filename.empty())
            {
                sparta_tap_.reset(new sparta::log::Tap(getRoot(), "cosim", filename));
            }
            else
            {
                sparta_tap_.reset(new sparta::log::Tap(getRoot(), "cosim", std::cout));
            }
        }

        cosim::Event step(HartId hart) override final;
        cosim::Event step(HartId hart, Addr override_pc) override final;

        // Unimplemented methods
        cosim::Event stepOperation(HartId hart) override final;
        cosim::Event stepOperation(HartId hart, Addr override_pc) override final;
        void commit(HartId hart) override final;
        void commit(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void dropStoreWrite(const cosim::Event* event) override final;
        void dropStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void flush(const cosim::Event* event, bool flush_younger_only = false) override final;
        cosim::MemoryInterface* getMemoryInterface() override final;
        void setMemoryInterface(cosim::MemoryInterface* mem_if) override final;
        void readRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void peekRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void writeRegister(HartId hart, RegId reg,
                           std::vector<uint8_t> & buffer) const override final;
        void pokeRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void setPc(HartId hart, Addr pc) override final;
        Addr getPc(HartId hart) const override final;
        void setPrivilegeMode(HartId hart, PrivMode priv_mode) override final;
        PrivMode getPrivilegeMode(HartId hart) const override final;
        bool isSimulationFinished(HartId hart) const override final;
        cosim::Event injectInstruction(HartId hart, Opcode opcode) override final;
        cosim::Event injectInterrupt(HartId hart, uint64_t interrupt_code) override final;
        cosim::Event injectReset(HartId hart) override final;
        uint64_t getNumCommittedEvents(HartId hart) const override final;
        const cosim::Event & getLastCommittedEvent(HartId hart) const override final;
        const cosim::EventList & getUncommittedEvents(HartId hart) const override final;
        uint64_t getNumUncommittedEvents(HartId hart) const override final;
        uint64_t getNumUncommittedWrites(HartId hart) const override final;

      private:
        void bindTree_() override;

        // CoSim Logger
        sparta::log::MessageSource cosim_logger_;
        std::unique_ptr<sparta::log::Tap> sparta_tap_;

        // Fetch Unit for each hart
        std::vector<Fetch*> fetch_;

        // CoSim memory interface
        CoSimMemoryInterface* cosim_memory_if_ = nullptr;

        // Event List for each hart
        std::vector<cosim::EventList> event_list_;

        // Last committed event for each hart
        std::vector<cosim::Event> last_committed_event_;

        // CoSim Observer for capturing Events from each hart
        std::vector<CoSimObserver*> cosim_observer_;
    };
} // namespace pegasus
