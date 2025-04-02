#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"

namespace atlas
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

    sparta::Register* getSpartaReg(AtlasState* state, const mavis::OperandInfo::Element* operand)
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

    AtlasInst::AtlasInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                         const AtlasExtractorPtr & extractor_info, AtlasState* state) :
        opcode_info_(opcode_info),
        extractor_info_(extractor_info),
        opcode_size_(((getOpcode() & 0x3) != 0x3) ? 2 : 4),
        rs1_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS1>(
            opcode_info->getSourceOpInfoList())),
        rs2_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS2>(
            opcode_info->getSourceOpInfoList())),
        rs3_info_(getOperand<mavis::InstMetaData::OperandFieldID::RS3>(
            opcode_info->getSourceOpInfoList())),
        rd_info_(
            getOperand<mavis::InstMetaData::OperandFieldID::RD>(opcode_info->getDestOpInfoList())),
        rs1_reg_(getSpartaReg(state, rs1_info_)),
        rs2_reg_(getSpartaReg(state, rs2_info_)),
        rs3_reg_(getSpartaReg(state, rs3_info_)),
        rd_reg_(getSpartaReg(state, rd_info_)),
        inst_action_group_(extractor_info_->inst_action_group_)
    {
    }

    bool AtlasInst::writesCsr() const
    {
        switch (getMavisUid())
        {
            // csrrw/csrrwi always writes
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRW:
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRWI:
                return true;
            // csrrs/csrrc write if rs1 != 0
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRS:
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRC:
                return rs1_info_ && (rs1_info_->field_value != 0);
            // csrrsi/csrrci writes if imm != 0
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRSI:
            case AtlasState::MavisUIDs::MAVIS_UID_CSRRCI:
                return getImmediate() != 0;
            default:
                sparta_assert(hasCsr() == false, "Unknown instruction with CSR: " << *this);
                return false;
        }
    }

    std::ostream & operator<<(std::ostream & os, const AtlasInst & inst)
    {
        os << "uid: " << std::dec << inst.uid_ << " " << inst.dasmString() << " "
           << inst.inst_action_group_;
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const AtlasInstPtr & inst)
    {
        os << *inst;
        return os;
    }
} // namespace atlas
