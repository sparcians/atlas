#include "core/PegasusInst.hpp"
#include "core/PegasusState.hpp"

namespace pegasus
{

    template <mavis::InstMetaData::OperandFieldID OperandFieldId>
    const mavis::OperandInfo::Element*
    getOperand(const mavis::OperandInfo::ElementList & operand_list)
    {
        const auto operand_it = std::find_if(operand_list.begin(), operand_list.end(),
                                             [](const mavis::OperandInfo::Element & operand)
                                             { return operand.field_id == OperandFieldId; });

        // Instruction does not have this operand type
        if (operand_it == operand_list.end())
        {
            return nullptr;
        }
        else
        {
            return &(*operand_it);
        }
    }

    sparta::Register* getSpartaReg(PegasusState* state, const mavis::OperandInfo::Element* operand)
    {
        if (operand)
        {
            switch (operand->operand_type)
            {
                case mavis::InstMetaData::OperandTypes::WORD:
                case mavis::InstMetaData::OperandTypes::LONG:
                    return state->getIntRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::SINGLE:
                case mavis::InstMetaData::OperandTypes::DOUBLE:
                case mavis::InstMetaData::OperandTypes::QUAD:
                    return state->getFpRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::VECTOR:
                    return state->getVecRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::NONE:
                    sparta_assert(false, "Invalid Mavis Operand Type!");
            }
        }

        return nullptr;
    }

    uint64_t getImmediateValue(const mavis::OpcodeInfo::PtrType & opcode_info)
    {
        return opcode_info->getImmediateType() == mavis::ImmediateType::SIGNED
                   ? opcode_info->getSignedOffset()
                   : opcode_info->getImmediate();
    }

    PegasusInst::PegasusInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                             const PegasusExtractorPtr & extractor_info, PegasusState* state) :
        opcode_info_(opcode_info),
        extractor_info_(extractor_info),
        opcode_size_(((getOpcode() & 0x3) != 0x3) ? 2 : 4),
        is_store_type_(opcode_info->isInstType(mavis::OpcodeInfo::InstructionTypes::STORE)),
        immediate_value_(getImmediateValue(opcode_info)),
        rs1_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS1>(
            opcode_info->getSourceOpInfoList())),
        rs2_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS2>(
            opcode_info->getSourceOpInfoList())),
        rs3_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS3>(
            opcode_info->getSourceOpInfoList())),
        rd_info_(
            getOperand<mavis::InstMetaData::OperandFieldID::RD>(opcode_info->getDestOpInfoList())),
        rd2_info_(
            getOperand<mavis::InstMetaData::OperandFieldID::RD2>(opcode_info->getDestOpInfoList())),
        rs1_reg_(getSpartaReg(state, rs1_info_)),
        rs2_reg_(getSpartaReg(state, rs2_info_)),
        rs3_reg_(getSpartaReg(state, rs3_info_)),
        rd_reg_(getSpartaReg(state, rd_info_)),
        rd2_reg_(getSpartaReg(state, rd2_info_)),
        inst_action_group_(extractor_info_->inst_action_group_)
    {
    }

    bool PegasusInst::writesCsr() const
    {
        switch (getMavisUid())
        {
            // csrrw/csrrwi always writes
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRW:
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRWI:
                return true;
            // csrrs/csrrc write if rs1 != 0
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRS:
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRC:
                return rs1_info_ && (rs1_info_->field_value != 0);
            // csrrsi/csrrci writes if imm != 0
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRSI:
            case PegasusState::MavisUIDs::MAVIS_UID_CSRRCI:
                return getImmediate() != 0;
            default:
                sparta_assert(hasCsr() == false, "Unknown instruction with CSR: " << *this);
                return false;
        }
    }

    std::ostream & operator<<(std::ostream & os, const PegasusInst & inst)
    {
        os << "uid: " << std::dec << inst.uid_ << " " << inst.dasmString() << " "
           << inst.inst_action_group_;
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const PegasusInstPtr & inst)
    {
        os << *inst;
        return os;
    }
} // namespace pegasus
