#pragma once

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
        execute_unit_ = state_->getExecuteUnit();
    }

    virtual ~AtlasInstructionTester() = default;

    atlas::AtlasState* getAtlasState() const { return state_; }

    void injectInstruction(const uint64_t pc, const uint32_t opcode)
    {
        state_->writeMemory(pc, opcode);
        state_->setPc(pc);

        // Fetch and Execute instruction
        atlas::ActionGroup* next_action_group = fetch_unit_->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state_);
        } while (next_action_group
                 && (next_action_group->hasTag(atlas::ActionTags::FETCH_TAG) == false));
    }

    void executeInstruction(atlas::AtlasInst::PtrType instPtr)
    {
        state_->setCurrentInst(instPtr);

        // Execute instruction
        atlas::ActionGroup* next_action_group = execute_unit_->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state_);
        } while (next_action_group
                 && (next_action_group->hasTag(atlas::ActionTags::FETCH_TAG) == false));
    }

    template <typename XLEN> XLEN setVtypeCSR(XLEN vlmul, XLEN vsew, XLEN vta, XLEN vma, XLEN vill)
    {
        WRITE_CSR_FIELD<XLEN>(state_, atlas::VTYPE, "vlmul", vlmul);
        WRITE_CSR_FIELD<XLEN>(state_, atlas::VTYPE, "vsew", vsew);
        WRITE_CSR_FIELD<XLEN>(state_, atlas::VTYPE, "vta", vta);
        WRITE_CSR_FIELD<XLEN>(state_, atlas::VTYPE, "vma", vma);
        WRITE_CSR_FIELD<XLEN>(state_, atlas::VTYPE, "vill", vill);
        return READ_CSR_REG<XLEN>(state_, atlas::VTYPE);
    }

    template <typename... Args> atlas::AtlasInst::PtrType makeAtlasInst(const Args &... args)
    {
        atlas::AtlasState* state = getAtlasState();
        mavis::ExtractorDirectInfo ex_info(args...);
        atlas::AtlasInst::PtrType instPtr = state->getMavis()->makeInstDirectly(ex_info, state);
        std::cout << instPtr->dasmString() << std::endl;
        return instPtr;
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<atlas::AtlasSim> atlas_sim_;

    atlas::AtlasState* state_ = nullptr;
    atlas::Fetch* fetch_unit_ = nullptr;
    atlas::Execute* execute_unit_ = nullptr;
};
