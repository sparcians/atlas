#include "core/observers/InstructionLogger.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"

#include "system/AtlasSystem.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
#ifndef INSTLOG
#define INSTLOG(msg) SPARTA_LOG(inst_logger_, msg)
#endif

    InstructionLogger::InstructionLogger(sparta::log::MessageSource & inst_logger) :
        inst_logger_(inst_logger)
    {
    }

    ActionGroup* InstructionLogger::preExecute(AtlasState* state)
    {
        reset_();

        pc_ = state->getPc();
        AtlasInstPtr inst = state->getCurrentInst();
        if (inst)
        {
            opcode_ = inst->getOpcode();

            // Get value of source registers
            if (inst->hasRs1())
            {
                const auto rs1_reg = inst->getRs1Reg();
                const std::vector<uint8_t> value =
                    convertToByteVector(rs1_reg->dmiRead<uint64_t>());
                src_regs_.emplace_back(getRegId(rs1_reg), value);
            }

            if (inst->hasRs2())
            {
                const auto rs2_reg = inst->getRs2Reg();
                const std::vector<uint8_t> value =
                    convertToByteVector(rs2_reg->dmiRead<uint64_t>());
                src_regs_.emplace_back(getRegId(rs2_reg), value);
            }

            // Get initial value of destination registers
            if (inst->hasRd())
            {
                const auto rd_reg = inst->getRdReg();
                const std::vector<uint8_t> value = convertToByteVector(rd_reg->dmiRead<uint64_t>());
                dst_regs_.emplace_back(getRegId(rd_reg), value);
            }

            // For instructions that read/write CSRs
            if (inst->hasCsr())
            {
                const auto csr_reg = state->getCsrRegister(inst->getCsr());
                if (csr_reg)
                {
                    const std::vector<uint8_t> value =
                        convertToByteVector(csr_reg->dmiRead<uint64_t>());
                    dst_regs_.emplace_back(getRegId(csr_reg), value);
                }
            }
        }

        return nullptr;
    }

    ActionGroup* InstructionLogger::preException(AtlasState* state)
    {
        preExecute(state);

        // Get value of source registers
        trap_cause_ = state->getExceptionUnit()->getUnhandledException();
        return nullptr;
    }

    ActionGroup* InstructionLogger::postExecute(AtlasState* state)
    {
        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();

        if (trap_cause_.isValid() == false)
        {
            sparta_assert(inst != nullptr, "Instruction is not valid for logging!");
        }

        if (inst)
        {
            for (auto & dst_reg : dst_regs_)
            {
                sparta::Register* reg = nullptr;
                switch (dst_reg.reg_id.reg_type)
                {
                    case RegType::INTEGER:
                        reg = state->getIntRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::FLOATING_POINT:
                        reg = state->getFpRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::VECTOR:
                        reg = state->getVecRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::CSR:
                        reg = state->getCsrRegister(dst_reg.reg_id.reg_num);
                        break;
                    default:
                        sparta_assert(false, "Invalid register type!");
                }
                sparta_assert(reg != nullptr);
                const std::vector<uint8_t> value = convertToByteVector(reg->dmiRead<uint64_t>());
                dst_reg.setValue(value);
            }
        }

        // Write to instruction logger
        const auto & symbols = state->getAtlasSystem()->getSymbols();
        if (symbols.find(pc_) != symbols.end())
        {
            INSTLOG("<" << symbols.at(pc_) << ">");
        }

        if (inst)
        {
            INSTLOG(HEX8(pc_) << ": " << inst->dasmString() << " (" << HEX8(opcode_)
                              << ") uid:" << inst->getUid());
        }
        else
        {
            // TODO: Only display opcode for certain exception types
            INSTLOG(HEX8(pc_) << ": ??? (" << HEX8(opcode_) << ") uid: ?");
        }

        if (trap_cause_.isValid())
        {
            INSTLOG("trap cause: " << HEX16((uint64_t)trap_cause_.getValue()));
        }

        if (inst && inst->hasImmediate())
        {
            const int64_t imm_val = inst->getImmediate();
            INSTLOG("       imm: " << HEX16(imm_val));
        }

        // TODO: Support for different register sizes (RV32, vector)
        for (const auto & src_reg : src_regs_)
        {
            const uint64_t reg_value = convertFromByteVector<uint64_t>(src_reg.reg_value);
            INSTLOG("   src " << std::setfill(' ') << std::setw(3) << src_reg.reg_id.reg_name
                              << ": " << HEX16(reg_value));
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const uint64_t reg_value = convertFromByteVector<uint64_t>(dst_reg.reg_value);
            const uint64_t reg_prev_value = convertFromByteVector<uint64_t>(dst_reg.reg_prev_value);
            INSTLOG("   dst " << std::setfill(' ') << std::setw(3) << dst_reg.reg_id.reg_name
                              << ": " << HEX16(reg_value) << " (prev: " << HEX16(reg_prev_value)
                              << ")");
        }

        INSTLOG("");
        return nullptr;
    }
} // namespace atlas
