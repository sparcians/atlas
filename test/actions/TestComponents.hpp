#include "core/PegasusState.hpp"

#include <algorithm>

// Base Unit class
class Unit
{
  public:
    using base_type = Unit;

    Unit(const std::string name) : name_(name), action_group_(name) {}

    virtual ~Unit() = default;

    const std::string & getName() const { return name_; }

    pegasus::ActionGroup* getActionGroup() { return &action_group_; }

  protected:
    const std::string name_;
    pegasus::ActionGroup action_group_;
};

class FetchUnit : public Unit
{
  public:
    FetchUnit() : Unit("Fetch") { loadWorkload(); }

    pegasus::Action::ItrType fetch_inst(pegasus::PegasusState* state,
                                        pegasus::Action::ItrType action_it)
    {
        // Set the current inst, using the PC as an index
        const pegasus::Addr pc = state->getPc();
        std::cout << "Fetching PC 0x" << std::hex << pc << std::endl;

        // Read the "opcode" from the "workload"
        const uint64_t opcode = workload_.at(pc);

        // Construct and set the current inst
        auto inst = state->getMavis()->makeInst(opcode, state);
        state->setCurrentInst(inst);

        // Always go to either Decode or Translate
        return ++action_it;
    }

    void loadWorkload(const bool vector = false)
    {
        workload_.clear();
        // Initialize a fake workload
        const uint64_t opcode = (vector) ? vadd_opcode : add_opcode;
        pegasus::Addr pc = 01000;
        while (pc < 0x1012)
        {
            workload_[pc] = opcode;
            pc += 4;
        }
        workload_[pc] = wfi_opcode;
    }

  private:
    std::map<pegasus::Addr, uint64_t> workload_;
    std::vector<pegasus::PegasusInstPtr> fetched_insts_;
    const uint64_t add_opcode = 0x00000033;  // add x0, x0, x0
    const uint64_t vadd_opcode = 0x02000057; // vadd.vv v0, v0, v0
    const uint64_t wfi_opcode = 0x10500073;  // wfi
};

class TranslateUnit : public Unit
{
  public:
    TranslateUnit() : Unit("Translate") {}

    pegasus::Action::ItrType translate_addr(pegasus::PegasusState*,
                                            pegasus::Action::ItrType action_it)
    {
        std::cout << "Translating" << std::endl;

        // Always go to Decode
        return ++action_it;
    }
};

class DecodeUnit : public Unit
{
  public:
    DecodeUnit() : Unit("Decode") {}

    pegasus::Action::ItrType decode_inst(pegasus::PegasusState*, pegasus::Action::ItrType action_it)
    {
        std::cout << "Decoding" << std::endl;

        // Always go to Execute
        return ++action_it;
    }
};

class ExecuteUnit : public Unit
{
  public:
    ExecuteUnit(pegasus::ActionGroup* fetch_action_group) :
        Unit("Execute"),
        fetch_action_group_(fetch_action_group)
    {
    }

    pegasus::Action::ItrType execute_inst(pegasus::PegasusState* state,
                                          pegasus::Action::ItrType action_it)
    {
        // Get current inst
        const pegasus::PegasusInstPtr inst = state->getCurrentInst();

        auto inst_action_group_it = inst_action_groups_.find(inst->getMnemonic());
        if (inst_action_group_it == inst_action_groups_.end())
        {
            auto ret = inst_action_groups_.insert({inst->getMnemonic(), {inst->getMnemonic()}});
            sparta_assert(ret.second == true,
                          "Failed to create instruction ActionGroup for " << inst);

            // Create an ActionGroup for the current instruction
            inst_action_group_it = ret.first;
            pegasus::ActionGroup & inst_action_group = inst_action_group_it->second;
            inst_action_group.addAction(pegasus::Action::createAction<&ExecuteUnit::inst_handler>(
                this, inst->getMnemonic().c_str()));

            if (inst->getMnemonic() == "wfi")
            {
                inst_action_group.setNextActionGroup(state->getStopSimActionGroup());
            }
            else
            {
                // Go back to Fetch
                inst_action_group.setNextActionGroup(fetch_action_group_);
            }
        }

        // Execute the instruction
        action_group_.setNextActionGroup(&inst_action_group_it->second);
        return ++action_it;
    }

    pegasus::Action::ItrType inst_handler(pegasus::PegasusState* state,
                                          pegasus::Action::ItrType action_it)
    {
        const pegasus::PegasusInstPtr & inst = state->getCurrentInst();
        std::cout << "Executing "
                  << "uid: " << std::dec << inst->getUid() << " " << inst->dasmString()
                  << std::endl;
        state->getSimState()->inst_count++;

        // Determine if the inst handler needs to be called again
        static uint32_t num_uops_to_execute = 2;
        --num_uops_to_execute;
        const bool is_vector = inst->getMnemonic() == "vadd.vv";
        if (is_vector && (num_uops_to_execute > 0))
        {
            return action_it;
        }
        else
        {
            // reset
            num_uops_to_execute = 2;

            // Increment the PC and inst count
            state->setPc(state->getPc() + 4);

            return ++action_it;
        }
    }

  private:
    pegasus::ActionGroup* fetch_action_group_;
    std::map<std::string, pegasus::ActionGroup> inst_action_groups_;
};
