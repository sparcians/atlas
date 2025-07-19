#pragma once

#include "core/PegasusExtractor.hpp"
#include "mavis/OpcodeInfo.h"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

#include "core/translate/PegasusTranslationState.hpp"

namespace sparta
{
    class Register;
}

namespace pegasus
{
    class PegasusState;

    class PegasusInst
    {
      public:
        using PtrType = sparta::SpartaSharedPointer<PegasusInst>;

        PegasusInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                    const PegasusExtractorPtr & extractor_info, PegasusState* state);

        uint64_t getUid() const { return uid_; }

        void setUid(const uint64_t uid) { uid_ = uid; }

        mavis::OpcodeInfo::PtrType getMavisOpcodeInfo() { return opcode_info_; }

        mavis::InstructionUniqueID getMavisUid() const
        {
            return opcode_info_->getInstructionUniqueID();
        }

        const std::string & getMnemonic() const { return opcode_info_->getMnemonic(); }

        std::string dasmString() const { return opcode_info_->dasmString(); }

        mavis::Opcode getOpcode() const { return opcode_info_->getOpcode(); }

        bool hasImmediate() const { return opcode_info_->hasImmediate(); }

        uint64_t getImmediate() const { return immediate_value_; }

        bool hasCsr() const
        {
            return opcode_info_->isInstType(mavis::OpcodeInfo::InstructionTypes::CSR);
        }

        uint32_t getCsr() const
        {
            sparta_assert(hasCsr(), "Failed to get CSR!");
            return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
        }

        bool isMemoryInst() const { return extractor_info_->isMemoryInst(); }

        bool writesCsr() const;

        uint32_t getOpcodeSize() const { return opcode_size_; }

        uint32_t isStoreType() const { return is_store_type_; }

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

        uint32_t getRd2() const
        {
            sparta_assert(rd_info_, "Operand RD2 is a nullptr! " << *this);
            return rd2_info_->field_value;
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

        uint64_t getAVL() const
        {
            return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::AVL);
        }

        uint64_t getVM() const
        {
            return opcode_info_->getSpecialField(mavis::OpcodeInfo::SpecialField::VM);
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

        sparta::Register* getRd2Reg() const
        {
            sparta_assert(rd2_reg_, "Operand RD2 is a nullptr! " << *this);
            return rd2_reg_;
        }

        bool hasRs1() const { return rs1_reg_ != nullptr; }

        bool hasRs2() const { return rs2_reg_ != nullptr; }

        bool hasRs3() const { return rs3_reg_ != nullptr; }

        bool hasRd() const { return rd_reg_ != nullptr; }

        bool hasRd2() const { return rd2_reg_ != nullptr; }

        ActionGroup* getActionGroup() { return &inst_action_group_; }

        const ActionGroup* getActionGroup() const { return &inst_action_group_; }

        void markUnimplemented() { unimplemented_ = true; }

        bool unimplemented() const { return unimplemented_; }

        // Translation information.  Specifically, this is for data
        // accesses
        PegasusTranslationState* getTranslationState() { return &translation_state_; }

      private:
        // Unique ID
        uint64_t uid_;

        mavis::OpcodeInfo::PtrType opcode_info_;
        PegasusExtractorPtr extractor_info_;

        // Opcode size in bytes, either 4 or 2 (compressed)
        const uint32_t opcode_size_;

        // Is this a store-type instruction
        const bool is_store_type_;

        // Cache immediate value, unsigned and signed
        const uint64_t immediate_value_;

        // Registers
        const mavis::OperandInfo::Element* rs1_info_;
        const mavis::OperandInfo::Element* rs2_info_;
        const mavis::OperandInfo::Element* rs3_info_;
        const mavis::OperandInfo::Element* rd_info_;
        const mavis::OperandInfo::Element* rd2_info_;
        sparta::Register* rs1_reg_;
        sparta::Register* rs2_reg_;
        sparta::Register* rs3_reg_;
        sparta::Register* rd_reg_;
        sparta::Register* rd2_reg_;

        // Translation state for load/store instructions
        PegasusTranslationState translation_state_;

        ActionGroup inst_action_group_;
        bool unimplemented_ = false;

        friend std::ostream & operator<<(std::ostream & os, const PegasusInst & inst);
    };

    using PegasusInstPtr = PegasusInst::PtrType;
    using PegasusInstAllocator = sparta::SpartaSharedPointerAllocator<PegasusInst>;

    std::ostream & operator<<(std::ostream & os, const PegasusInst & inst);
    std::ostream & operator<<(std::ostream & os, const PegasusInstPtr & inst);
} // namespace pegasus
