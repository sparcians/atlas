#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    template <mavis::InstMetaData::OperandFieldID OperandFieldId>
    sparta::Register* getSpartaReg(AtlasState* state,
                                   const mavis::OperandInfo::ElementList & operand_list)
    {
        const auto operand = std::find_if(operand_list.begin(), operand_list.end(),
                                          [](const mavis::OperandInfo::Element & operand)
                                          { return operand.field_id == OperandFieldId; });

        // Instruction does not have this operand type
        if (operand == operand_list.end())
        {
            return nullptr;
        }

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

        return nullptr;
    }

    AtlasInst::AtlasInst(const mavis::OpcodeInfo::PtrType & opcode_info,
                         const AtlasExtractorPtr & extractor_info, AtlasState* state) :
        uid_(state->assignUid()),
        opcode_info_(opcode_info),
        extractor_info_(extractor_info),
        opcode_size_(((getOpcode() & 0x3) != 0x3) ? 2 : 4),
        rs1_(getSpartaReg<mavis::InstMetaData::OperandFieldID::RS1>(
            state, opcode_info->getSourceOpInfoList())),
        rs2_(getSpartaReg<mavis::InstMetaData::OperandFieldID::RS2>(
            state, opcode_info->getSourceOpInfoList())),
        rs3_(getSpartaReg<mavis::InstMetaData::OperandFieldID::RS3>(
            state, opcode_info->getSourceOpInfoList())),
        rd_(getSpartaReg<mavis::InstMetaData::OperandFieldID::RD>(
            state, opcode_info->getDestOpInfoList())),
        inst_action_group_(extractor_info_->inst_action_group_)
    {
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
