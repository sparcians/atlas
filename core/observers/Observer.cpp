#include "core/observers/Observer.hpp"
#include "include/AtlasUtils.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    void Observer::preExecute(AtlasState* state)
    {
        reset_();
        inspectInitialState_(state);

        // Subclass impl
        preExecute_(state);
    }

    void Observer::preException(AtlasState* state)
    {
        // We want to reuse the preExecute() code but we do not want to
        // call the subclass' preExecute_() method twice (once normally
        // and once here during exception handling).
        //
        // That is why we call reset_() and inspectInitialState_() here
        // instead of preExecute().
        reset_();
        inspectInitialState_(state);

        // Get value of source registers
        fault_cause_ = state->getExceptionUnit()->getUnhandledFault();
        interrupt_cause_ = state->getExceptionUnit()->getUnhandledInterrupt();

        // Subclass impl
        preException_(state);
    }

    void Observer::postExecute(AtlasState* state)
    {
        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();

        if (fault_cause_.isValid() == false)
        {
            sparta_assert(inst != nullptr, "Instruction is not valid for logging!");
        }

        if (inst && arch_.isValid())
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
                const uint64_t value = readRegister_(reg);
                dst_reg.setValue(value);
            }
        }

        // Subclass impl
        postExecute_(state);
    }

    void Observer::inspectInitialState_(AtlasState* state)
    {
        pc_ = state->getPc();
        AtlasInstPtr inst = state->getCurrentInst();

        if (inst)
        {
            opcode_ = inst->getOpcode();

            if (arch_.isValid())
            {
                // Get value of source registers
                if (inst->hasRs1())
                {
                    const auto rs1_reg = inst->getRs1Reg();
                    const uint64_t value = readRegister_(rs1_reg);
                    src_regs_.emplace_back(getRegId(rs1_reg), value);
                }

                if (inst->hasRs2())
                {
                    const auto rs2_reg = inst->getRs2Reg();
                    const uint64_t value = readRegister_(rs2_reg);
                    src_regs_.emplace_back(getRegId(rs2_reg), value);
                }

                // Get initial value of destination registers
                if (inst->hasRd())
                {
                    const auto rd_reg = inst->getRdReg();
                    const uint64_t value = readRegister_(rd_reg);
                    dst_regs_.emplace_back(getRegId(rd_reg), value);
                }
            }
        }
    }

    uint64_t Observer::readRegister_(const sparta::Register* reg)
    {
        const uint32_t reg_width = getRegWidth();

        if (reg_width == 8)
            return reg->dmiRead<RV32>();
        if (reg_width == 16)
            return reg->dmiRead<RV64>();

        sparta_assert(false, "Invalid register width");
        return 0;
    }

    void Observer::postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                                 const sparta::Register::PostWriteAccess & data)
    {
        const auto csr_reg = data.reg;
        const auto csr_num = csr_reg->getGroupIdx();
        const RegId reg_id{(RegType)csr_reg->getGroupNum(), csr_num, csr_reg->getName()};

        const uint64_t final_value = (csr_reg->getNumBits() == 64) ? data.final->read<uint64_t>()
                                                                   : data.final->read<uint32_t>();
        // If this CSR has already been written to, just update the final value
        if (csr_writes_.find(csr_num) != csr_writes_.end())
        {
            csr_writes_.at(csr_num).setValue(final_value);
        }
        else
        {
            const uint64_t prior_value = (csr_reg->getNumBits() == 64)
                                             ? data.prior->read<uint64_t>()
                                             : data.prior->read<uint32_t>();
            csr_writes_.insert({csr_num, DestReg(reg_id, final_value, prior_value)});
        }

        // No need to also capture a read if there is a write since the write records the previous
        // value
        csr_reads_.erase(csr_num);
    }

    void Observer::postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                                const sparta::Register::ReadAccess & data)
    {
        const auto csr_reg = data.reg;
        const auto csr_num = csr_reg->getGroupIdx();
        const RegId reg_id{(RegType)csr_reg->getGroupNum(), csr_num, csr_reg->getName()};
        if ((csr_reads_.find(csr_num) == csr_reads_.end())
            && (csr_writes_.find(csr_num) == csr_writes_.end()))
        {
            const uint64_t value = (csr_reg->getNumBits() == 64) ? data.value->read<uint64_t>()
                                                                 : data.value->read<uint32_t>();
            csr_reads_.insert({csr_num, SrcReg(reg_id, value)});
        }
    }

    void Observer::postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess & data)
    {
        uint64_t prior_val = 0;
        if (data.prior)
        {
            for (size_t i = 0; i < data.size; ++i)
            {
                prior_val |= static_cast<uint64_t>(data.prior[i]) << (i * 8);
            }
        }

        uint8_t buf[2048];
        data.mem->peek(data.addr, data.size, buf);

        uint64_t final_val = 0;
        for (size_t i = 0; i < data.size; ++i)
        {
            final_val |= static_cast<uint64_t>(buf[i]) << (i * 8);
        }

        MemWrite mem_write;
        mem_write.addr = data.addr;
        mem_write.size = data.size;
        mem_write.value = final_val;
        mem_write.prior_value = prior_val;
        mem_writes_.push_back(mem_write);
    }

    void Observer::postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess & data)
    {
        uint64_t val = 0;
        for (size_t i = 0; i < data.size; ++i)
        {
            val |= static_cast<uint64_t>(data.data[i]) << (i * 8);
        }

        MemRead mem_read;
        mem_read.addr = data.addr;
        mem_read.size = data.size;
        mem_read.value = val;
        mem_reads_.push_back(mem_read);
    }
} // namespace atlas
