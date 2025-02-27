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

        void setUid(const uint64_t uid) { uid_ = uid; }

        mavis::OpcodeInfo::PtrType getMavisOpcodeInfo() { return opcode_info_; }

        const std::string & getMnemonic() const { return opcode_info_->getMnemonic(); }

        const std::string dasmString() const { return opcode_info_->dasmString(); }

        mavis::Opcode getOpcode() const { return opcode_info_->getOpcode(); }

        bool hasImmediate() const { return opcode_info_->hasImmediate(); }

        uint64_t getImmediate() const
        {
            sparta_assert(hasImmediate(), "Failed to get immediate value!");
            return opcode_info_->getImmediate();
        }

        bool hasCsr() const
        {
            return opcode_info_->isInstType(mavis::OpcodeInfo::InstructionTypes::CSR);
        }

        uint32_t getCsr() const
        {
            sparta_assert(hasCsr(), "Failed to get CSR!");
            return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
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

        uint32_t getRs1() const
        {
            sparta_assert(rs1_info_, "Operand RS1 is a nullptr! " << *this);
            return rs1_info_->field_value;
        }

        uint32_t getRs2() const
        {
            sparta_assert(rs2_info_, "Operand RS2 is a nullptr! " << *this);
            return rs2_info_->field_value;
        }

        uint32_t getRs3() const
        {
            sparta_assert(rs3_info_, "Operand RS3 is a nullptr! " << *this);
            return rs3_info_->field_value;
        }

        uint32_t getRd() const
        {
            sparta_assert(rd_info_, "Operand RD is a nullptr! " << *this);
            return rd_info_->field_value;
        }

        uint64_t getRM() const
        {
            try
            {
                return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::RM);
            }
            catch (...)
            {
                sparta_assert(false, "Can't get SpecialField RM! " << *this);
            }
            return 0;
        }

        sparta::Register* getRs1Reg() const
        {
            sparta_assert(rs1_reg_, "Operand RS1 is a nullptr! " << *this);
            return rs1_reg_;
        }

        sparta::Register* getRs2Reg() const
        {
            sparta_assert(rs2_reg_, "Operand RS2 is a nullptr! " << *this);
            return rs2_reg_;
        }

        sparta::Register* getRs3Reg() const
        {
            sparta_assert(rs3_reg_, "Operand RS3 is a nullptr! " << *this);
            return rs3_reg_;
        }

        sparta::Register* getRdReg() const
        {
            sparta_assert(rd_reg_, "Operand RD is a nullptr! " << *this);
            return rd_reg_;
        }

        bool hasRs1() const { return rs1_reg_ != nullptr; }

        bool hasRs2() const { return rs2_reg_ != nullptr; }

        bool hasRs3() const { return rs3_reg_ != nullptr; }

        bool hasRd() const { return rd_reg_ != nullptr; }

        ActionGroup* getActionGroup() { return &inst_action_group_; }

        const ActionGroup* getActionGroup() const { return &inst_action_group_; }

        void markUnimplemented() { unimplemented_ = true; }

        bool unimplemented() const { return unimplemented_; }

      private:
        // Unique ID
        uint64_t uid_;

        mavis::OpcodeInfo::PtrType opcode_info_;
        AtlasExtractorPtr extractor_info_;

        // Opcode size in bytes, either 4 or 2 (compressed)
        const uint32_t opcode_size_;

        // Next PC
        Addr next_pc_;

        // Registers
        const mavis::OperandInfo::Element* rs1_info_;
        const mavis::OperandInfo::Element* rs2_info_;
        const mavis::OperandInfo::Element* rs3_info_;
        const mavis::OperandInfo::Element* rd_info_;
        sparta::Register* rs1_reg_;
        sparta::Register* rs2_reg_;
        sparta::Register* rs3_reg_;
        sparta::Register* rd_reg_;

        ActionGroup inst_action_group_;
        bool unimplemented_ = false;

        friend std::ostream & operator<<(std::ostream & os, const AtlasInst & inst);
    };

    using AtlasInstPtr = AtlasInst::PtrType;
    using AtlasInstAllocator = sparta::SpartaSharedPointerAllocator<AtlasInst>;

    std::ostream & operator<<(std::ostream & os, const AtlasInst & inst);
    std::ostream & operator<<(std::ostream & os, const AtlasInstPtr & inst);
} // namespace atlas
