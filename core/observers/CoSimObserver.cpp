#include "core/observers/CoSimObserver.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"

#include "system/AtlasSystem.hpp"

namespace atlas
{
    CoSimObserver::CoSimObserver() : Observer(ObserverMode::RV64)
    {
        // TODO: CoSimObserver for rv32
    }

    void CoSimObserver::preExecute_(AtlasState* state)
    {
        last_event_ = cosim::Event(++event_uid_, cosim::Event::Type::INSTRUCTION);
        last_event_.hart_id_ = state->getHartId();

        AtlasInstPtr inst = state->getCurrentInst();
        last_event_.arch_id_ = inst->getUid();
        last_event_.opcode_ = inst->getOpcode();
        last_event_.opcode_size_ = inst->getOpcodeSize();
        // TODO: inst_type_

        last_event_.curr_pc_ = state->getPc();
        // TODO: curr_priv_

        if (inst)
        {
            last_event_.mavis_opcode_info_ = inst->getMavisOpcodeInfo();
        }
    }

    void CoSimObserver::postExecute_(AtlasState* state)
    {
        for (auto & src_reg : src_regs_)
        {
            last_event_.register_reads_.emplace_back(src_reg.reg_id, src_reg.reg_value);
        }

        for (auto & dst_reg : dst_regs_)
        {
            last_event_.register_writes_.emplace_back(dst_reg.reg_id, dst_reg.reg_value,
                                                      dst_reg.reg_prev_value);
        }

        last_event_.done_ = true;
        last_event_.event_ends_sim_ = state->getSimState()->sim_stopped;

        last_event_.next_pc_ = state->getPc();
        sparta_assert(
            last_event_.next_pc_ != last_event_.curr_pc_,
            "Next PC is the same as the current PC! Check ordering of post-execute Events");
        // TODO: for branches, is_change_of_flow_, alternate_next_pc_
        // TODO: next_priv_
    }
} // namespace atlas
