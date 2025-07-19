#pragma once

#include "mavis/OpcodeInfo.h"
#include "sparta/functional/Register.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"
#include "core/Trap.hpp"
#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class PegasusState;
    class ActionGroup;

    // The base class needs to know if we are rv32 or rv64 since it is responsible for
    // reading register values (XLEN).
    //
    // We do not use templates here (template <typename XLEN>) because then PegasusState
    // cannot hold onto arch-agnostic observers (std::vector<std::unique_ptr<Observer>>).
    //
    // Note that if your subclass tells the Observer to use ObserverMode::UNUSED, then
    // the before/after register values will NOT be tracked.
    enum class ObserverMode
    {
        RV32,
        RV64,
        UNUSED
    };

    class Observer
    {
      public:
        static uint32_t getRegWidth(const ObserverMode arch)
        {
            switch (arch)
            {
                case ObserverMode::RV32:
                    return 8;
                case ObserverMode::RV64:
                    return 16;
                default:
                    sparta_assert(false, "Invalid architecture");
            }
        }

        uint32_t getRegWidth() const { return Observer::getRegWidth(arch_); }

        Observer(const ObserverMode arch)
        {
            if (arch != ObserverMode::UNUSED)
            {
                arch_ = arch;
            }

            reset_();
        }

        virtual ~Observer() = default;

        // Holds a register's value as a byte vector
        class RegValue
        {
          public:
            RegValue() = default;

            RegValue(const std::vector<uint8_t> & value) : value_(value) {}

            template <typename TYPE> RegValue(TYPE value) { setValue<TYPE>(value); }

            void setValue(const std::vector<uint8_t> & value) { value_ = value; }

            template <typename TYPE> void setValue(TYPE value)
            {
                static_assert(std::is_trivial_v<TYPE>);
                static_assert(std::is_standard_layout_v<TYPE>);
                static_assert(std::is_integral_v<TYPE>);
                value_.resize(sizeof(TYPE));
                memcpy(value_.data(), &value, sizeof(TYPE));
            }

            template <typename TYPE> TYPE getValue(uint32_t offset = 0) const
            {
                static_assert(std::is_trivial_v<TYPE>);
                static_assert(std::is_standard_layout_v<TYPE>);
                static_assert(std::is_integral_v<TYPE>);
                const size_t num_bytes = sizeof(TYPE);
                assert((offset + num_bytes) < value_.size());
                TYPE val = 0;
                for (size_t i = offset; i < num_bytes; ++i)
                {
                    val |= static_cast<TYPE>(value_[i]) << (i * 8);
                }
                return val;
            }

            size_t size() const { return value_.size(); }

            const std::vector<uint8_t> & getByteVector() const { return value_; }

          private:
            std::vector<uint8_t> value_;

            friend std::ostream & operator<<(std::ostream & os, const RegValue & reg_value);
        };

        struct ObservedReg
        {
            ObservedReg(const RegId id) : reg_id(id) {}

            template <typename TYPE>
            ObservedReg(const RegId id, TYPE value) : reg_id(id), reg_value(value)
            {
            }

            template <typename TYPE> TYPE getRegValue() const { return reg_value.getValue<TYPE>(); }

            const RegId reg_id;
            RegValue reg_value;
        };

        using SrcReg = ObservedReg;

        struct DestReg : ObservedReg
        {
            template <typename TYPE>
            DestReg(const RegId id, TYPE prev_value) : ObservedReg(id), reg_prev_value(prev_value)
            {
            }

            template <typename TYPE>
            DestReg(const RegId id, TYPE value, TYPE prev_value) :
                ObservedReg(id, value),
                reg_prev_value(prev_value)
            {
            }

            template <typename TYPE> TYPE getRegPrevValue() const
            {
                return reg_value.getValue<TYPE>();
            }

            RegValue reg_prev_value;
        };

        void preExecute(PegasusState* state);

        void postExecute(PegasusState* state);

        void preException(PegasusState* state);

        virtual void stopSim() {}

        struct ObservedMemAccess
        {
            Addr addr;
            size_t size;
            uint64_t value;
        };

        using MemRead = ObservedMemAccess;

        struct MemWrite : ObservedMemAccess
        {
            uint64_t prior_value;
        };

        void registerReadWriteCsrCallbacks(sparta::RegisterBase* reg)
        {
            if (arch_.isValid())
            {
                reg->getPostWriteNotificationSource().REGISTER_FOR_THIS(postCsrWrite_);
                reg->getReadNotificationSource().REGISTER_FOR_THIS(postCsrRead_);
            }
        }

        void registerReadWriteMemCallbacks(sparta::memory::BlockingMemoryIFNode* m)
        {
            if (arch_.isValid())
            {
                m->getPostWriteNotificationSource().REGISTER_FOR_THIS(postMemWrite_);
                m->getReadNotificationSource().REGISTER_FOR_THIS(postMemRead_);
            }
        }

      protected:
        uint64_t pc_;
        uint64_t opcode_;

        // Mavis pointer for getting the disassembly string
        mavis::OpcodeInfo::PtrType opcode_info_;

        // Instruction source and destination registers
        std::vector<SrcReg> src_regs_;
        std::vector<DestReg> dst_regs_;

        // Implicit CSR reads and writes
        std::unordered_map<uint32_t, SrcReg> csr_reads_;
        std::unordered_map<uint32_t, DestReg> csr_writes_;

        // Memory reads and writes
        std::vector<MemRead> mem_reads_;
        std::vector<MemWrite> mem_writes_;

        // Exception cause
        sparta::utils::ValidValue<FaultCause> fault_cause_;
        sparta::utils::ValidValue<InterruptCause> interrupt_cause_;

      private:
        sparta::utils::ValidValue<ObserverMode> arch_;

        void inspectInitialState_(PegasusState* state);

        virtual void preExecute_(PegasusState*) {}

        virtual void postExecute_(PegasusState*) {}

        virtual void preException_(PegasusState*) {}

        void reset_()
        {
            pc_ = 0;
            opcode_ = std::numeric_limits<uint64_t>::max();
            src_regs_.clear();
            dst_regs_.clear();
            csr_reads_.clear();
            csr_writes_.clear();
            fault_cause_.clearValid();
            interrupt_cause_.clearValid();
            mem_reads_.clear();
            mem_writes_.clear();
        }

        std::vector<uint8_t> readRegister_(const sparta::Register* reg) const
        {
            const size_t num_bytes = reg->getNumBytes();
            std::vector<uint8_t> value(num_bytes, 0);
            const uint32_t offset = 0;
            reg->peek(value.data(), num_bytes, offset);
            return value;
        }

        // Callbacks
        void postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                           const sparta::Register::PostWriteAccess &);
        void postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                          const sparta::Register::ReadAccess &);
        void postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess &);
        void postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess &);
    };

    std::ostream & operator<<(std::ostream & os, const Observer::RegValue & reg_value);

} // namespace pegasus
