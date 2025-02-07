#include "core/observers/CoSimObserver.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"

#include "system/AtlasSystem.hpp"

namespace atlas
{
    CoSimObserver::CoSimObserver() {}

    ActionGroup* CoSimObserver::preExecute(AtlasState* state)
    {
        reset_();

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
            if (inst->hasRs1())
            {
                const auto rs1 = inst->getRs1Reg();
                const std::vector<uint8_t> value = convertToByteVector(rs1->dmiRead<uint64_t>());
                src_regs_.emplace_back(getRegId(rs1), value);
            }

            if (inst->hasRs2())
            {
                const auto rs2 = inst->getRs2Reg();
                const std::vector<uint8_t> value = convertToByteVector(rs2->dmiRead<uint64_t>());
                src_regs_.emplace_back(getRegId(rs2), value);
            }

            // Get initial value of destination registers
            if (inst->hasRd())
            {
                const auto rd = inst->getRdReg();
                const std::vector<uint8_t> value = convertToByteVector(rd->dmiRead<uint64_t>());
                dst_regs_.emplace_back(getRegId(rd), value);
            }

            last_event_.mavis_opcode_info_ = inst->getMavisOpcodeInfo();
        }

        return nullptr;
    }

    ActionGroup* CoSimObserver::preException(AtlasState* state)
    {
        preExecute(state);

        // Get value of source registers
        trap_cause_ = state->getExceptionUnit()->getUnhandledException();
        return nullptr;
    }

    ActionGroup* CoSimObserver::postExecute(AtlasState* state)
    {
        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();

        if (inst && inst->hasRd())
        {
            sparta_assert(dst_regs_.size() == 1);
            const auto & rd_reg = inst->getRdReg();
            std::vector<uint8_t> value(sizeof(uint64_t));
            const uint64_t rd_value = rd_reg->dmiRead<uint64_t>();
            std::memcpy(value.data(), &rd_value, sizeof(uint64_t));
            dst_regs_[0].setValue(value);
        }

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

        return nullptr;
    }
} // namespace atlas
