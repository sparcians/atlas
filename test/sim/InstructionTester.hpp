#include "sim/AtlasSim.hpp"
#include "core/AtlasState.hpp"
#include "include/AtlasTypes.hpp"

class AtlasInstructionTester
{

  public:
    AtlasInstructionTester()
    {
        // Create the simulator
        const std::string workload = "";
        const uint64_t ilimit = 0;
        atlas_sim_.reset(new atlas::AtlasSim(&scheduler_, workload, ilimit));

        atlas_sim_->buildTree();
        atlas_sim_->configureTree();
        atlas_sim_->finalizeTree();

        state_ = atlas_sim_->getAtlasState();
        fetch_unit_ = state_->getFetchUnit();
    }

    atlas::AtlasState* getAtlasState() { return state_; }

    void injectInstruction(const uint64_t pc, const uint32_t opcode)
    {
        state_->writeMemory(pc, opcode);
        state_->setPc(pc);

        // Execute instruction
        atlas::ActionGroup* next_action_group = fetch_unit_->getActionGroup();
        do 
        {
            next_action_group = next_action_group->execute(state_);
        } while (next_action_group && (next_action_group->hasTag(atlas::ActionTags::FETCH_TAG) == false));
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<atlas::AtlasSim> atlas_sim_;

    atlas::AtlasState* state_ = nullptr;
    atlas::Fetch* fetch_unit_ = nullptr;
};
