#include "core/observers/Observer.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
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
