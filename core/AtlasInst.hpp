#pragma once

#include "core/AtlasExtractor.hpp"
#include "mavis/OpcodeInfo.h"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

namespace sparta
{
    class Register;
}

namespace atlas
{
    class AtlasState;

    class AtlasInst
    {
      public:
        using PtrType = sparta::SpartaSharedPointer<AtlasInst>;

        AtlasInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                  const AtlasExtractorPtr & extractor_info, AtlasState* state);

        uint64_t getUid() const { return uid_; }

        mavis::OpcodeInfo::PtrType getMavisOpcodeInfo() { return opcode_info_; }

        const std::string & getMnemonic() const { return opcode_info_->getMnemonic(); }

        const std::string dasmString() const { return opcode_info_->dasmString(); }

        mavis::Opcode getOpcode() const { return opcode_info_->getOpcode(); }

        bool hasImmediate() const { return opcode_info_->hasImmediate(); }

        uint64_t getImmediate() const
        {
            sparta_assert(opcode_info_->hasImmediate(), "Failed to get immediate value!");
            return opcode_info_->getImmediate();
        }

        template <class T, uint32_t imm_size> T getSignExtendedImmediate() const
        {
            sparta_assert(opcode_info_->hasImmediate(), "Failed to get immediate value!");
            static_assert(std::is_same<T, RV64>() or std::is_same<T, RV32>());
            static_assert(imm_size <= 32);
            T imm_val = 1;
            imm_val <<= imm_size - 1;
            imm_val = (opcode_info_->getImmediate() ^ imm_val) - imm_val;
            return imm_val;
        }

        bool isMemoryInst() const { return extractor_info_->isMemoryInst(); }

        uint32_t getOpcodeSize() const { return opcode_size_; }

        uint64_t getRM()
        {
            return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::RM);
        }

        sparta::Register*& getRs1()
        {
            sparta_assert(rs1_, "Operand RS1 is a nullptr! " << *this);
            return rs1_;
        }

        sparta::Register* getRs2()
        {
            sparta_assert(rs2_, "Operand RS2 is a nullptr! " << *this);
            return rs2_;
        }

        sparta::Register*& getRs3()
        {
            sparta_assert(rs3_, "Operand RS2 is a nullptr! " << *this);
            return rs3_;
        }

        sparta::Register*& getRd()
        {
            sparta_assert(rd_, "Operand RD is a nullptr! " << *this);
            return rd_;
        }

        bool hasRs1() const { return rs1_ != nullptr; }

        bool hasRs2() const { return rs2_ != nullptr; }

        bool hasRs3() const { return rs3_ != nullptr; }

        bool hasRd() const { return rd_ != nullptr; }

        ActionGroup* getActionGroup() { return &inst_action_group_; }

        const ActionGroup* getActionGroup() const { return &inst_action_group_; }

      private:
        // Unique ID
        const uint64_t uid_;

        mavis::OpcodeInfo::PtrType opcode_info_;
        AtlasExtractorPtr extractor_info_;

        // Opcode size in bytes, either 4 or 2 (compressed)
        const uint32_t opcode_size_;

        // Next PC
        Addr next_pc_;

        // Registers
        sparta::Register* rs1_;
        sparta::Register* rs2_;
        sparta::Register* rs3_;
        sparta::Register* rd_;

        ActionGroup inst_action_group_;

        friend std::ostream & operator<<(std::ostream & os, const AtlasInst & inst);
    };

    using AtlasInstPtr = AtlasInst::PtrType;
    using AtlasInstAllocator = sparta::SpartaSharedPointerAllocator<AtlasInst>;

    std::ostream & operator<<(std::ostream & os, const AtlasInst & inst);
    std::ostream & operator<<(std::ostream & os, const AtlasInstPtr & inst);
} // namespace atlas
