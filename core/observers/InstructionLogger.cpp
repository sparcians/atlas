#include "core/observers/InstructionLogger.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "include/PegasusUtils.hpp"

#include "system/PegasusSystem.hpp"

#include "sparta/utils/LogUtils.hpp"
#include "sparta/log/MessageSource.hpp"

namespace pegasus
{
#ifndef INSTLOG
#define INSTLOG(msg) SPARTA_LOG(inst_logger_, msg)
#endif

    class InstLogWriterBase
    {
      public:
        InstLogWriterBase(sparta::log::MessageSource & inst_logger, const uint32_t reg_width) :
            inst_logger_(inst_logger),
            reg_width_(reg_width)
        {
        }

        virtual ~InstLogWriterBase() = default;

        virtual void beginInst(PegasusState* state, const PegasusInst* inst, uint64_t opcode)
        {
            (void)state;
            (void)inst;
            (void)opcode;
        }

        virtual void writeSymbols(const std::string & symbols) { (void)symbols; }

        virtual void writeInstHeader(const PegasusInst* inst, uint64_t pc, uint64_t opcode)
        {
            (void)inst;
            (void)pc;
            (void)opcode;
        }

        virtual void writeFaultCause(const FaultCause cause) { (void)cause; }

        virtual void writeImmediate(const uint64_t imm) { (void)imm; }

        virtual void writeSrcRegister(const std::string &, const Observer::RegValue &) {}

        virtual void writeDstRegister(const std::string &, const Observer::RegValue &,
                                      const Observer::RegValue &)
        {
        }

        virtual void writeCsrRead(const std::string &, const Observer::RegValue &) {}

        virtual void writeCsrWrite(const std::string &, const Observer::RegValue &,
                                   const Observer::RegValue &)
        {
        }

        virtual void writeDstCSR(const std::string & reg_name, const uint32_t reg_num,
                                 const Observer::RegValue & reg_value,
                                 const Observer::RegValue & reg_prev_value)
        {
            (void)reg_num;
            writeDstRegister(reg_name, reg_value, reg_prev_value);
        }

        virtual void writeMemRead(const Observer::MemRead & mem_read) { (void)mem_read; }

        virtual void writeMemWrite(const Observer::MemWrite & mem_write) { (void)mem_write; }

        virtual void finishInst() {}

      protected:
        sparta::log::MessageSource & inst_logger_;

        uint32_t getRegWidth() const { return reg_width_; }

      private:
        const uint32_t reg_width_;
    };

    class PegasusInstLogWriter : public InstLogWriterBase
    {
      public:
        PegasusInstLogWriter(sparta::log::MessageSource & inst_logger, const uint32_t reg_width) :
            InstLogWriterBase(inst_logger, reg_width)
        {
        }

        void writeSymbols(const std::string & symbols) override
        {
            reset_();
            inst_oss_ << "Call <" << symbols << ">";
            postExecute_(inst_oss_.str());
        }

        void writeInstHeader(const PegasusInst* inst, uint64_t pc, uint64_t opcode) override
        {
            reset_();
            if (inst)
            {
                inst_oss_ << HEX(pc, getRegWidth()) << " " << inst->dasmString() << " ("
                          << HEX8(opcode) << ") uid:" << inst->getUid();
            }
            else
            {
                // TODO: Only display opcode for certain exception types
                inst_oss_ << HEX(pc, getRegWidth()) << " ??? (" << HEX8(opcode) << ") uid: ?";
            }
            postExecute_(inst_oss_.str());
        }

        void writeFaultCause(const FaultCause cause) override
        {
            reset_();
            inst_oss_ << "Fault cause: " << cause << " (" << HEX8(static_cast<uint32_t>(cause))
                      << ")";
            postExecute_(inst_oss_.str());
        }

        void writeImmediate(const uint64_t imm) override
        {
            reset_();

            if (getRegWidth() == 8)
            {
                using SXLEN = int32_t;
                const SXLEN imm_val = imm;
                inst_oss_ << "   imm: " << HEX(imm_val, getRegWidth());
            }
            else
            {
                using SXLEN = int64_t;
                const SXLEN imm_val = imm;
                inst_oss_ << "   imm: " << HEX(imm_val, getRegWidth());
            }

            postExecute_(inst_oss_.str());
        }

        void writeSrcRegister(const std::string & reg_name,
                              const Observer::RegValue & reg_value) override
        {
            reset_();
            inst_oss_ << "   src " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << reg_value;
            postExecute_(inst_oss_.str());
        }

        void writeDstRegister(const std::string & reg_name, const Observer::RegValue & reg_value,
                              const Observer::RegValue & reg_prev_value) override
        {
            reset_();
            inst_oss_ << "   dst " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << reg_value << " (prev: " << reg_prev_value << ")";
            postExecute_(inst_oss_.str());
        }

        void writeCsrRead(const std::string & reg_name,
                          const Observer::RegValue & reg_value) override
        {
            reset_();
            inst_oss_ << "   csr " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << reg_value;
            postExecute_(inst_oss_.str());
        }

        void writeCsrWrite(const std::string & reg_name, const Observer::RegValue & reg_value,
                           const Observer::RegValue & reg_prev_value) override
        {
            reset_();
            inst_oss_ << "   csr " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << reg_value << " (prev: " << reg_prev_value << ")";
            postExecute_(inst_oss_.str());
        }

        void writeMemRead(const Observer::MemRead & mem_read) override
        {
            reset_();
            inst_oss_ << "   mem read addr: " << HEX(mem_read.addr, getRegWidth())
                      << " size: " << mem_read.size
                      << " value: " << HEX(mem_read.value, getRegWidth());
            postExecute_(inst_oss_.str());
        }

        void writeMemWrite(const Observer::MemWrite & mem_write) override
        {
            reset_();
            inst_oss_ << "   mem write addr: " << HEX(mem_write.addr, getRegWidth())
                      << " size: " << mem_write.size
                      << " value: " << HEX(mem_write.value, getRegWidth())
                      << " (prev: " << HEX(mem_write.prior_value, getRegWidth()) << ")";
            postExecute_(inst_oss_.str());
        }

        void finishInst() override { postExecute_(""); }

      private:
        void reset_()
        {
            inst_oss_.str("");
            inst_oss_.clear();
        }

        void postExecute_(const std::string & msg)
        {
            // The reason we call INSTLOG inside postExecute_() is because
            // we want the log to look like this:
            //
            //     postExecute_: 0x8000000c: li x1, +0xf (0xdeadbeef) uid: 34
            //     postExecute_:    imm: 0xf
            //     postExecute_:    dst x1: 0xf (prev: 0x0)
            //
            // Instead of this:
            //
            //     writeInstHeader:  0x8000000c: li x1, +0xf (0xdeadbeef) uid: 34
            //     writeImmediate:      imm: 0xf
            //     writeDstRegister:    dst x1: 0xf (prev: 0x0)
            //
            // We also use the somewhat confusing method name postExecute_() here since
            // that is the InstructionLogger's calling function name.

            INSTLOG(msg);
        }

        std::ostringstream inst_oss_;
    };

    class SpikeInstLogWriter : public InstLogWriterBase
    {
      public:
        SpikeInstLogWriter(sparta::log::MessageSource & inst_logger, const uint32_t reg_width) :
            InstLogWriterBase(inst_logger, reg_width)
        {
        }

        void beginInst(PegasusState* state, const PegasusInst*, uint64_t opcode) override
        {
            reset_();
            inst_oss_ << "core   " << state->getHartId() << ": " << (int)state->getPrivMode() << " "
                      << HEX(state->getPrevPc(), getRegWidth()) << " (" << HEX8(opcode) << ")";
        }

        void writeDstRegister(const std::string & reg_name, const Observer::RegValue & reg_value,
                              const Observer::RegValue & reg_prev_value) override
        {
            (void)reg_prev_value;
            inst_oss_ << " " << std::setw(4) << std::left << reg_name << reg_value;
        }

        void writeDstCSR(const std::string & reg_name, const uint32_t reg_num,
                         const Observer::RegValue & reg_value,
                         const Observer::RegValue & reg_prev_value) override
        {
            (void)reg_prev_value;
            inst_oss_ << " c" << reg_num << "_" << reg_name << " " << reg_value;
        }

        void writeMemRead(const Observer::MemRead & mem_read) override
        {
            inst_oss_ << " mem " << HEX(mem_read.addr, getRegWidth());
        }

        void writeMemWrite(const Observer::MemWrite & mem_write) override
        {
            inst_oss_ << " mem " << HEX(mem_write.addr, getRegWidth()) << " "
                      << HEX8(mem_write.value);
        }

        void finishInst() override { postExecute_(inst_oss_.str()); }

      private:
        void reset_()
        {
            inst_oss_.str("");
            inst_oss_.clear();
        }

        void postExecute_(const std::string & msg)
        {
            // clang-format off

            // The reason we call INSTLOG inside postExecute_() is because
            // we want the log to look like this:
            //
            //     postExecute_: core   0: 3 0x800000e4 (0x30529073) x0  0x00000000 c773_mtvec 0x800000e8
            //
            // Instead of this:
            //
            //     beginInst:         core   0: 3 0x800000e4 (0x30529073)
            //     writeDstRegister:  x0  0x00000000
            //     writeDstCSR:       c773_mtvec 0x800000e8
            //
            // We also use the somewhat confusing method name postExecute_() here since
            // that is the InstructionLogger's calling function name.

            // clang-format on
            INSTLOG(msg);
        }

        std::ostringstream inst_oss_;
    };

    InstructionLogger::InstructionLogger(sparta::log::MessageSource & inst_logger,
                                         const ObserverMode arch) :
        Observer(arch),
        inst_logger_(inst_logger),
        inst_log_writer_(std::make_shared<PegasusInstLogWriter>(inst_logger_, getRegWidth()))
    {
    }

    void InstructionLogger::useSpikeFormatting()
    {
        inst_log_writer_ = std::make_shared<SpikeInstLogWriter>(inst_logger_, getRegWidth());
    }

    void InstructionLogger::postExecute_(PegasusState* state)
    {
        PegasusInstPtr inst = state->getCurrentInst();
        inst_log_writer_->beginInst(state, inst.get(), opcode_);

        // Write to instruction logger
        const auto & symbols = state->getPegasusSystem()->getSymbols();
        if (symbols.find(pc_) != symbols.end())
        {
            inst_log_writer_->writeSymbols(symbols.at(pc_));
        }

        inst_log_writer_->writeInstHeader(inst.get(), pc_, opcode_);

        if (fault_cause_.isValid())
        {
            inst_log_writer_->writeFaultCause(fault_cause_.getValue());
        }

        if (inst && inst->hasImmediate())
        {
            inst_log_writer_->writeImmediate(inst->getImmediate());
        }

        for (const auto & src_reg : src_regs_)
        {
            inst_log_writer_->writeSrcRegister(src_reg.reg_id.reg_name, src_reg.reg_value);
        }

        for (const auto & dst_reg : dst_regs_)
        {
            if (dst_reg.reg_id.reg_type == RegType::CSR)
            {
                inst_log_writer_->writeDstCSR(dst_reg.reg_id.reg_name, dst_reg.reg_id.reg_num,
                                              dst_reg.reg_value, dst_reg.reg_prev_value);
            }
            else
            {
                // skip x0 -- you cannot write to x0
                if (SPARTA_EXPECT_FALSE(dst_reg.reg_id.reg_num == 0
                                        && dst_reg.reg_id.reg_type == RegType::INTEGER))
                {
                    continue;
                }
                inst_log_writer_->writeDstRegister(dst_reg.reg_id.reg_name, dst_reg.reg_value,
                                                   dst_reg.reg_prev_value);
            }
        }

        for (const auto & [csr_num, csr_read] : csr_reads_)
        {
            inst_log_writer_->writeCsrRead(csr_read.reg_id.reg_name, csr_read.reg_value);
        }

        for (const auto & [csr_num, csr_write] : csr_writes_)
        {
            inst_log_writer_->writeCsrWrite(csr_write.reg_id.reg_name, csr_write.reg_value,
                                            csr_write.reg_prev_value);
        }

        for (const auto & mem_read : mem_reads_)
        {
            inst_log_writer_->writeMemRead(mem_read);
        }

        for (const auto & mem_write : mem_writes_)
        {
            inst_log_writer_->writeMemWrite(mem_write);
        }

        inst_log_writer_->finishInst();
    }
} // namespace pegasus
