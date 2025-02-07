#include "core/inst_handlers/i/RviInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "arch/register_macros.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

#include <functional>

namespace atlas
{
    template <typename XLEN>
    void RviInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "lb",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint8_t>,
                                            RviInsts>(nullptr, "lb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lbu",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint8_t>,
                                            RviInsts>(nullptr, "lbu",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "ld",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint64_t>,
                                            RviInsts>(nullptr, "ld", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lh",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint16_t>,
                                            RviInsts>(nullptr, "lh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lhu",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint16_t>,
                                            RviInsts>(nullptr, "lhu",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lw",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint32_t>,
                                            RviInsts>(nullptr, "lw", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lwu",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint32_t>,
                                            RviInsts>(nullptr, "lwu",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sb",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint8_t>,
                                            RviInsts>(nullptr, "sb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sd",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint64_t>,
                                            RviInsts>(nullptr, "sd", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sh",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint16_t>,
                                            RviInsts>(nullptr, "sh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sw",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint32_t>,
                                            RviInsts>(nullptr, "sw", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "lb",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint8_t>,
                                            RviInsts>(nullptr, "lb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lbu",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint8_t>,
                                            RviInsts>(nullptr, "lbu",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lh",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint16_t>,
                                            RviInsts>(nullptr, "lh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lhu",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint16_t>,
                                            RviInsts>(nullptr, "lhu",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lw",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint32_t>,
                                            RviInsts>(nullptr, "lw", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sb",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint8_t>,
                                            RviInsts>(nullptr, "sb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sh",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV32, uint16_t>,
                                            RviInsts>(nullptr, "sh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sw",
                atlas::Action::createAction<&RviInsts::compute_address_handler<RV64, uint32_t>,
                                            RviInsts>(nullptr, "sw", ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template <typename XLEN>
    void RviInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        constexpr bool SIGN_EXTEND = true;

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "add", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV64, std::plus<RV64>>, RviInsts>(
                           nullptr, "add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "addi", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV64, std::plus<RV64>>, RviInsts>(
                            nullptr, "addi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addiw",
                                  atlas::Action::createAction<&RviInsts::addiw_handler, RviInsts>(
                                      nullptr, "addiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addw",
                                  atlas::Action::createAction<&RviInsts::addw_handler, RviInsts>(
                                      nullptr, "addw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "and", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV64, std::bit_and<RV64>>, RviInsts>(
                           nullptr, "and", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "andi", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV64, std::bit_and<RV64>>, RviInsts>(
                            nullptr, "andi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "auipc", atlas::Action::createAction<&RviInsts::auipc_handler<RV64>, RviInsts>(
                             nullptr, "auipc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "beq",
                atlas::Action::createAction<&RviInsts::branch_handler<RV64, std::equal_to<int64_t>>,
                                            RviInsts>(nullptr, "beq", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bge", atlas::Action::createAction<
                           &RviInsts::branch_handler<RV64, std::greater_equal<int64_t>>, RviInsts>(
                           nullptr, "bge", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bgeu",
                atlas::Action::createAction<
                    &RviInsts::branch_handler<RV64, std::greater_equal<uint64_t>>, RviInsts>(
                    nullptr, "bgeu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "blt",
                atlas::Action::createAction<&RviInsts::branch_handler<RV64, std::less<int64_t>>,
                                            RviInsts>(nullptr, "blt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bltu",
                atlas::Action::createAction<&RviInsts::branch_handler<RV64, std::less<uint64_t>>,
                                            RviInsts>(nullptr, "bltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bne", atlas::Action::createAction<
                           &RviInsts::branch_handler<RV64, std::not_equal_to<uint64_t>>, RviInsts>(
                           nullptr, "bne", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ebreak",
                                  atlas::Action::createAction<&RviInsts::ebreak_handler, RviInsts>(
                                      nullptr, "ebreak", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ecall",
                                  atlas::Action::createAction<&RviInsts::ecall_handler, RviInsts>(
                                      nullptr, "ecall", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fence",
                                  atlas::Action::createAction<&RviInsts::fence_handler, RviInsts>(
                                      nullptr, "fence", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jal", atlas::Action::createAction<&RviInsts::jal_handler<RV64>, RviInsts>(
                           nullptr, "jal", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jalr", atlas::Action::createAction<&RviInsts::jalr_handler<RV64>, RviInsts>(
                            nullptr, "jalr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lb",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint8_t, SIGN_EXTEND>,
                                            RviInsts>(nullptr, "lb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lbu",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint8_t>, RviInsts>(
                    nullptr, "lbu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ld",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint64_t>, RviInsts>(
                    nullptr, "ld", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lh",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint16_t, SIGN_EXTEND>,
                                            RviInsts>(nullptr, "lh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lhu",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint16_t>, RviInsts>(
                    nullptr, "lhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "li", atlas::Action::createAction<&RviInsts::li_handler<RV64>, RviInsts>(
                          nullptr, "li", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lui", atlas::Action::createAction<&RviInsts::lui_handler<RV64>, RviInsts>(
                           nullptr, "lui", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lw",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint32_t, SIGN_EXTEND>,
                                            RviInsts>(nullptr, "lw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lwu",
                atlas::Action::createAction<&RviInsts::load_handler<RV64, uint32_t>, RviInsts>(
                    nullptr, "lwu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mret",
                atlas::Action::createAction<&RviInsts::xret_handler<RV64, PrivMode::MACHINE>,
                                            RviInsts>(nullptr, "mret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mv", atlas::Action::createAction<&RviInsts::mv_handler<RV64>, RviInsts>(
                          nullptr, "mv", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("nop",
                                  atlas::Action::createAction<&RviInsts::nop_handler, RviInsts>(
                                      nullptr, "nop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "or", atlas::Action::createAction<
                          &RviInsts::integer_reg_reg_handler<RV64, std::bit_or<RV64>>, RviInsts>(
                          nullptr, "or", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ori", atlas::Action::createAction<
                           &RviInsts::integer_reg_imm_handler<RV64, std::bit_or<RV64>>, RviInsts>(
                           nullptr, "ori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sb",
                atlas::Action::createAction<&RviInsts::store_handler<RV64, uint8_t>, RviInsts>(
                    nullptr, "sb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sd",
                atlas::Action::createAction<&RviInsts::store_handler<RV64, uint64_t>, RviInsts>(
                    nullptr, "sd", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sfence.vma", atlas::Action::createAction<&RviInsts::sfence_vma_handler, RviInsts>(
                                  nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh",
                atlas::Action::createAction<&RviInsts::store_handler<RV64, uint16_t>, RviInsts>(
                    nullptr, "sh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sll",
                                  atlas::Action::createAction<&RviInsts::sll_handler, RviInsts>(
                                      nullptr, "sll", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slli",
                                  atlas::Action::createAction<&RviInsts::slli_handler, RviInsts>(
                                      nullptr, "slli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slliw",
                                  atlas::Action::createAction<&RviInsts::slliw_handler, RviInsts>(
                                      nullptr, "slliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sllw",
                                  atlas::Action::createAction<&RviInsts::sllw_handler, RviInsts>(
                                      nullptr, "sllw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slt", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV64, std::less<int64_t>>, RviInsts>(
                           nullptr, "slt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slti", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV64, std::less<int64_t>>, RviInsts>(
                            nullptr, "slti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltiu", atlas::Action::createAction<
                             &RviInsts::integer_reg_imm_handler<RV64, std::less<RV64>>, RviInsts>(
                             nullptr, "sltiu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltu", atlas::Action::createAction<
                            &RviInsts::integer_reg_reg_handler<RV64, std::less<RV64>>, RviInsts>(
                            nullptr, "sltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sra",
                                  atlas::Action::createAction<&RviInsts::sra_handler, RviInsts>(
                                      nullptr, "sra", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srai",
                                  atlas::Action::createAction<&RviInsts::srai_handler, RviInsts>(
                                      nullptr, "srai", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sraiw",
                                  atlas::Action::createAction<&RviInsts::sraiw_handler, RviInsts>(
                                      nullptr, "sraiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sraw",
                                  atlas::Action::createAction<&RviInsts::sraw_handler, RviInsts>(
                                      nullptr, "sraw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sret",
                atlas::Action::createAction<&RviInsts::xret_handler<RV64, PrivMode::SUPERVISOR>,
                                            RviInsts>(nullptr, "sret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srl",
                                  atlas::Action::createAction<&RviInsts::srl_handler, RviInsts>(
                                      nullptr, "srl", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srli",
                                  atlas::Action::createAction<&RviInsts::srli_handler, RviInsts>(
                                      nullptr, "srli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srliw",
                                  atlas::Action::createAction<&RviInsts::srliw_handler, RviInsts>(
                                      nullptr, "srliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srlw",
                                  atlas::Action::createAction<&RviInsts::srlw_handler, RviInsts>(
                                      nullptr, "srlw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sub", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV64, std::minus<RV64>>, RviInsts>(
                           nullptr, "sub", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("subw",
                                  atlas::Action::createAction<&RviInsts::subw_handler, RviInsts>(
                                      nullptr, "subw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sw",
                atlas::Action::createAction<&RviInsts::store_handler<RV64, uint32_t>, RviInsts>(
                    nullptr, "sw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("wfi",
                                  atlas::Action::createAction<&RviInsts::wfi_handler, RviInsts>(
                                      nullptr, "wfi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xor", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV64, std::bit_xor<RV64>>, RviInsts>(
                           nullptr, "xor", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xori", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV64, std::bit_xor<RV64>>, RviInsts>(
                            nullptr, "xori", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "add", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV32, std::plus<RV32>>, RviInsts>(
                           nullptr, "add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "addi", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV32, std::plus<RV32>>, RviInsts>(
                            nullptr, "addi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "and", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV32, std::bit_and<RV32>>, RviInsts>(
                           nullptr, "and", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "andi", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV32, std::bit_and<RV32>>, RviInsts>(
                            nullptr, "andi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "auipc", atlas::Action::createAction<&RviInsts::auipc_handler<RV32>, RviInsts>(
                             nullptr, "auipc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "beq",
                atlas::Action::createAction<&RviInsts::branch_handler<RV32, std::equal_to<int64_t>>,
                                            RviInsts>(nullptr, "beq", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bge", atlas::Action::createAction<
                           &RviInsts::branch_handler<RV32, std::greater_equal<int64_t>>, RviInsts>(
                           nullptr, "bge", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bgeu",
                atlas::Action::createAction<
                    &RviInsts::branch_handler<RV32, std::greater_equal<uint64_t>>, RviInsts>(
                    nullptr, "bgeu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "blt",
                atlas::Action::createAction<&RviInsts::branch_handler<RV32, std::less<int64_t>>,
                                            RviInsts>(nullptr, "blt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bltu",
                atlas::Action::createAction<&RviInsts::branch_handler<RV32, std::less<uint64_t>>,
                                            RviInsts>(nullptr, "bltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bne", atlas::Action::createAction<
                           &RviInsts::branch_handler<RV32, std::not_equal_to<uint64_t>>, RviInsts>(
                           nullptr, "bne", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ebreak",
                                  atlas::Action::createAction<&RviInsts::ebreak_handler, RviInsts>(
                                      nullptr, "ebreak", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ecall",
                                  atlas::Action::createAction<&RviInsts::ecall_handler, RviInsts>(
                                      nullptr, "ecall", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fence",
                                  atlas::Action::createAction<&RviInsts::fence_handler, RviInsts>(
                                      nullptr, "fence", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jal", atlas::Action::createAction<&RviInsts::jal_handler<RV32>, RviInsts>(
                           nullptr, "jal", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jalr", atlas::Action::createAction<&RviInsts::jalr_handler<RV32>, RviInsts>(
                            nullptr, "jalr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lb",
                atlas::Action::createAction<&RviInsts::load_handler<RV32, uint8_t, SIGN_EXTEND>,
                                            RviInsts>(nullptr, "lb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lbu",
                atlas::Action::createAction<&RviInsts::load_handler<RV32, uint8_t>, RviInsts>(
                    nullptr, "lbu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lh",
                atlas::Action::createAction<&RviInsts::load_handler<RV32, uint16_t, SIGN_EXTEND>,
                                            RviInsts>(nullptr, "lh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lhu",
                atlas::Action::createAction<&RviInsts::load_handler<RV32, uint16_t>, RviInsts>(
                    nullptr, "lhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "li", atlas::Action::createAction<&RviInsts::li_handler<RV32>, RviInsts>(
                          nullptr, "li", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lui", atlas::Action::createAction<&RviInsts::lui_handler<RV32>, RviInsts>(
                           nullptr, "lui", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lw",
                atlas::Action::createAction<&RviInsts::load_handler<RV32, uint32_t>, RviInsts>(
                    nullptr, "lw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mret",
                atlas::Action::createAction<&RviInsts::xret_handler<RV32, PrivMode::MACHINE>,
                                            RviInsts>(nullptr, "mret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mv", atlas::Action::createAction<&RviInsts::mv_handler<RV32>, RviInsts>(
                          nullptr, "mv", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("nop",
                                  atlas::Action::createAction<&RviInsts::nop_handler, RviInsts>(
                                      nullptr, "nop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "or", atlas::Action::createAction<
                          &RviInsts::integer_reg_reg_handler<RV32, std::bit_or<RV32>>, RviInsts>(
                          nullptr, "or", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ori", atlas::Action::createAction<
                           &RviInsts::integer_reg_imm_handler<RV32, std::bit_or<RV32>>, RviInsts>(
                           nullptr, "ori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sb",
                atlas::Action::createAction<&RviInsts::store_handler<RV32, uint8_t>, RviInsts>(
                    nullptr, "sb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sfence.vma", atlas::Action::createAction<&RviInsts::sfence_vma_handler, RviInsts>(
                                  nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh",
                atlas::Action::createAction<&RviInsts::store_handler<RV32, uint16_t>, RviInsts>(
                    nullptr, "sh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sll",
                                  atlas::Action::createAction<&RviInsts::sll_handler, RviInsts>(
                                      nullptr, "sll", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slt", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV32, std::less<int64_t>>, RviInsts>(
                           nullptr, "slt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slti", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV32, std::less<int64_t>>, RviInsts>(
                            nullptr, "slti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltiu", atlas::Action::createAction<
                             &RviInsts::integer_reg_imm_handler<RV32, std::less<RV32>>, RviInsts>(
                             nullptr, "sltiu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltu", atlas::Action::createAction<
                            &RviInsts::integer_reg_reg_handler<RV32, std::less<RV32>>, RviInsts>(
                            nullptr, "sltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sra",
                                  atlas::Action::createAction<&RviInsts::sra_handler, RviInsts>(
                                      nullptr, "sra", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sret",
                atlas::Action::createAction<&RviInsts::xret_handler<RV32, PrivMode::SUPERVISOR>,
                                            RviInsts>(nullptr, "sret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srl",
                                  atlas::Action::createAction<&RviInsts::srl_handler, RviInsts>(
                                      nullptr, "srl", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sub", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV32, std::minus<RV32>>, RviInsts>(
                           nullptr, "sub", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sw",
                atlas::Action::createAction<&RviInsts::store_handler<RV32, uint32_t>, RviInsts>(
                    nullptr, "sw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("wfi",
                                  atlas::Action::createAction<&RviInsts::wfi_handler, RviInsts>(
                                      nullptr, "wfi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xor", atlas::Action::createAction<
                           &RviInsts::integer_reg_reg_handler<RV32, std::bit_xor<RV32>>, RviInsts>(
                           nullptr, "xor", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xori", atlas::Action::createAction<
                            &RviInsts::integer_reg_imm_handler<RV32, std::bit_xor<RV32>>, RviInsts>(
                            nullptr, "xori", ActionTags::EXECUTE_TAG));
        }
    }

    template void RviInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RviInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RviInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RviInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename OPERATOR>
    ActionGroup* RviInsts::integer_reg_reg_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG(insn->getRs1());
        const XLEN rs2_val = READ_INT_REG(insn->getRs2());
        const XLEN rd_val = OPERATOR()(rs1_val, rs2_val);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::addw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + rs2_val));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::subw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val - rs2_val));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN, class OPERATOR>
    ActionGroup* RviInsts::integer_reg_imm_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        constexpr uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        const uint64_t rd_val = OPERATOR()(rs1_val, imm);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::addiw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint32_t rs1_val = READ_INT_REG(insn->getRs1());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + imm));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::mv_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        WRITE_INT_REG(insn->getRd(), rs1_val);

        return nullptr;
    }

    ActionGroup* RviInsts::nop_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    template <typename XLEN, typename SIZE>
    ActionGroup* RviInsts::compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(SIZE));
        return nullptr;
    }

    template <typename XLEN, typename SIZE, bool SIGN_EXTEND>
    ActionGroup* RviInsts::load_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        if constexpr (SIGN_EXTEND)
        {
            const XLEN rd_val = signExtend<SIZE, XLEN>(state->readMemory<SIZE>(paddr));
            WRITE_INT_REG(insn->getRd(), rd_val);
        }
        else
        {
            const XLEN rd_val = state->readMemory<SIZE>(paddr);
            WRITE_INT_REG(insn->getRd(), rd_val);
        }
        return nullptr;
    }

    template <typename XLEN, typename SIZE>
    ActionGroup* RviInsts::store_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<SIZE>(paddr, rs2_val);
        return nullptr;
    }

    template <typename XLEN, typename OPERATOR>
    ActionGroup* RviInsts::branch_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG(insn->getRs1());
        const XLEN rs2_val = READ_INT_REG(insn->getRs2());

        if (OPERATOR()(rs1_val, rs2_val))
        {
            const uint64_t pc = state->getPc();
            constexpr uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::jal_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        XLEN rd_val = state->getPc() + insn->getOpcodeSize();
        const XLEN jump_target = state->getPc() + insn->getImmediate();
        state->setNextPc(jump_target);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::jalr_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        XLEN rd_val = state->getPc() + insn->getOpcodeSize();
        const XLEN rs1_val = READ_INT_REG(insn->getRs1());
        constexpr uint32_t IMM_SIZE = 12;
        const XLEN imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        const XLEN jump_target = (rs1_val + imm) & ~std::make_signed_t<XLEN>(1);
        state->setNextPc(jump_target);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::li_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        WRITE_INT_REG(insn->getRd(), imm);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::lui_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 32;
        const uint64_t imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        WRITE_INT_REG(insn->getRd(), imm);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RviInsts::auipc_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 32;
        const XLEN imm = insn->getSignExtendedImmediate<XLEN, IMM_SIZE>();
        const XLEN pc = state->getPc();
        const XLEN rd_val = ((std::make_signed_t<XLEN>)(imm + pc) << (64 - (state->getXlen())))
                            >> (64 - (state->getXlen()));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srl_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        const uint64_t rd_val = (int64_t)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srliw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sll_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        const uint64_t rd_val = rs1_val << (rs2_val & (state->getXlen() - 1));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srai_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const int64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const int64_t rd_val = (int64_t)(rs1_val >> shift_amount);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sraw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srli_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const int64_t rd_val = (int64_t)(rs1_val >> shift_amount);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sllw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint32_t rs2_val = READ_INT_REG(insn->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << (rs2_val & 0x1F));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::slliw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & 0x1F;
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << shift_amount);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sraiw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sra_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        const uint64_t rd_val = (int64_t)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::slli_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const uint64_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const uint64_t rd_val = rs1_val << shift_amount;
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srlw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG(insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG(insn->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        WRITE_INT_REG(insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN, PrivMode PRIV_MODE>
    ActionGroup* RviInsts::xret_handler(atlas::AtlasState* state)
    {
        static_assert(PRIV_MODE == PrivMode::MACHINE || PRIV_MODE == PrivMode::SUPERVISOR);

        // mret can only be executed in Machine mode
        // sret can be executee in Supervisor or Machine mode
        if (state->getPrivMode() < PRIV_MODE)
        {
            THROW_ILLEGAL_INSTRUCTION;
        }

        // FIXME: Register macros are currently hardcoded for RV64
        PrivMode prev_priv_mode = PrivMode::INVALID;
        if constexpr (PRIV_MODE == PrivMode::MACHINE)
        {
            // Update the PC with MEPC value
            state->setNextPc(READ_CSR_REG(MEPC));

            // Get the previous privilege mode from the MPP field of MSTATUS
            prev_priv_mode = (PrivMode)READ_CSR_FIELD(MSTATUS, mpp);

            // If the mret instruction changes the privilege mode to a mode less privileged
            // than Machine mode, the MPRV bit is reset to 0
            if (prev_priv_mode != PrivMode::MACHINE)
            {
                // TODO: Will need to update the load/store translation mode when translation is
                // supported
                WRITE_CSR_FIELD(MSTATUS, mprv, (XLEN)0);
            }

            // Set MIE = MPIE and reset MPIE
            WRITE_CSR_FIELD(MSTATUS, mie, READ_CSR_FIELD(MSTATUS, mpie));
            WRITE_CSR_FIELD(MSTATUS, mpie, (XLEN)1);

            // Reset MPP
            // TODO: Check if User mode is available
            WRITE_CSR_FIELD(MSTATUS, mpp, (XLEN)PrivMode::USER);
        }
        else
        {
            // Update the PC with SEPC value
            state->setNextPc(READ_CSR_REG(SEPC));

            // Get the previous privilege mode from the MPP field of MSTATUS
            prev_priv_mode = (PrivMode)READ_CSR_FIELD(SSTATUS, spp);

            // Reset the MPRV bit
            // TODO: Will need to update the load/store translation mode when translation is
            // supported
            WRITE_CSR_FIELD(MSTATUS, mprv, (XLEN)0);

            // Set MIE = MPIE and reset MPIE
            WRITE_CSR_FIELD(SSTATUS, sie, READ_CSR_FIELD(SSTATUS, spie));
            WRITE_CSR_FIELD(SSTATUS, spie, (XLEN)1);

            // Reset MPP
            WRITE_CSR_FIELD(SSTATUS, spp, (XLEN)PrivMode::USER);
        }

        // TODO: Update MSTATUSH

        // Update the privilege mode to the previous privilege mode
        state->setNextPrivMode(prev_priv_mode);

        return nullptr;
    }

    ActionGroup* RviInsts::ecall_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // switch (STATE.prv)
        //{
        //   case PRV_U: throw trap_user_ecall();
        //   case PRV_S:
        //     if (STATE.v)
        //       throw trap_virtual_supervisor_ecall();
        //     else
        //       throw trap_supervisor_ecall();
        //   case PRV_M: throw trap_machine_ecall();
        //   default: abort();
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        // Command
        const uint64_t cmd = READ_INT_REG(17); // a7

        // Only support exit for now so we can end simulation
        if (cmd == 93)
        {
            // Function arguments are a0-a6 (x10-x16)
            const uint64_t exit_code = READ_INT_REG(10);

            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->workload_exit_code = exit_code;
            sim_state->test_passed = (exit_code == 0) ? true : false;
            sim_state->sim_stopped = true;

            // Stop simulation
            return state->getStopSimActionGroup();
        }

        return nullptr;
    }

    ActionGroup* RviInsts::ebreak_handler(atlas::AtlasState* state)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (!STATE.debug_mode && (
        //         (!STATE.v && STATE.prv == PRV_M && STATE.dcsr->ebreakm) ||
        //         (!STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreaks) ||
        //         (!STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreaku) ||
        //         (STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreakvs) ||
        //         (STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreakvu))) {
        //     throw trap_debug_mode();
        // } else {
        //     throw trap_breakpoint(STATE.v, pc);
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::fence_handler(atlas::AtlasState* state)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::sfence_vma_handler(atlas::AtlasState* state)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('S');
        // require_impl(IMPL_MMU);
        // if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTVM))
        //     require_novirt();
        // } else {
        //   require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TVM) ? PRV_M :
        //   PRV_S);
        // }
        // MMU.flush_tlb();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::wfi_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
        //   require_privilege(PRV_M);
        // } else if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTW))
        //     require_novirt();
        // } else if (p->extension_enabled('S')) {
        //   // When S-mode is implemented, then executing WFI in
        //   // U-mode causes an illegal instruction exception.
        //   require_privilege(PRV_S);
        // }
        // wfi();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        if (state->getStopSimOnWfi())
        {
            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->sim_stopped = true;
            return state->getStopSimActionGroup();
        }
        return nullptr;
    }

} // namespace atlas
