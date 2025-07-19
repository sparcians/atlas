#include "core/inst_handlers/i/RviInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"

#include <functional>

namespace pegasus
{
    template <typename XLEN>
    void RviInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "lb",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint8_t>,
                                              RviInsts>(nullptr, "lb",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lbu",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint8_t>,
                                              RviInsts>(nullptr, "lbu",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "ld",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint64_t>,
                                              RviInsts>(nullptr, "ld",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lh",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint16_t>,
                                              RviInsts>(nullptr, "lh",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lhu",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint16_t>,
                                              RviInsts>(nullptr, "lhu",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lw",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint32_t>,
                                              RviInsts>(nullptr, "lw",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lwu",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint32_t>,
                                              RviInsts>(nullptr, "lwu",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sb",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint8_t>,
                                              RviInsts>(nullptr, "sb",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sd",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint64_t>,
                                              RviInsts>(nullptr, "sd",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sh",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint16_t>,
                                              RviInsts>(nullptr, "sh",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sw",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint32_t>,
                                              RviInsts>(nullptr, "sw",
                                                        ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "lb",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint8_t>,
                                              RviInsts>(nullptr, "lb",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lbu",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint8_t>,
                                              RviInsts>(nullptr, "lbu",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lh",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint16_t>,
                                              RviInsts>(nullptr, "lh",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lhu",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint16_t>,
                                              RviInsts>(nullptr, "lhu",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lw",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint32_t>,
                                              RviInsts>(nullptr, "lw",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sb",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint8_t>,
                                              RviInsts>(nullptr, "sb",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sh",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV32, uint16_t>,
                                              RviInsts>(nullptr, "sh",
                                                        ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sw",
                pegasus::Action::createAction<&RviInsts::computeAddressHandler_<RV64, uint32_t>,
                                              RviInsts>(nullptr, "sw",
                                                        ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template <typename XLEN>
    void RviInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers,
                                   bool enable_syscall_emulation)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        constexpr bool SIGN_EXTEND = true;

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "add", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV64, std::plus<RV64>>, RviInsts>(
                           nullptr, "add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "addi", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV64, std::plus<RV64>>, RviInsts>(
                            nullptr, "addi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addiw",
                                  pegasus::Action::createAction<&RviInsts::addiwHandler_, RviInsts>(
                                      nullptr, "addiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addw",
                                  pegasus::Action::createAction<&RviInsts::addwHandler_, RviInsts>(
                                      nullptr, "addw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "and", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV64, std::bit_and<RV64>>, RviInsts>(
                           nullptr, "and", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "andi", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV64, std::bit_and<RV64>>, RviInsts>(
                            nullptr, "andi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "auipc", pegasus::Action::createAction<&RviInsts::auipcHandler_<RV64>, RviInsts>(
                             nullptr, "auipc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "beq", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV64, std::equal_to<int64_t>>, RviInsts>(
                           nullptr, "beq", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bge", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV64, std::greater_equal<int64_t>>, RviInsts>(
                           nullptr, "bge", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bgeu",
                pegasus::Action::createAction<
                    &RviInsts::branchHandler_<RV64, std::greater_equal<uint64_t>>, RviInsts>(
                    nullptr, "bgeu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "blt",
                pegasus::Action::createAction<&RviInsts::branchHandler_<RV64, std::less<int64_t>>,
                                              RviInsts>(nullptr, "blt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bltu",
                pegasus::Action::createAction<&RviInsts::branchHandler_<RV64, std::less<uint64_t>>,
                                              RviInsts>(nullptr, "bltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bne", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV64, std::not_equal_to<uint64_t>>, RviInsts>(
                           nullptr, "bne", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ebreak", pegasus::Action::createAction<&RviInsts::ebreakHandler_, RviInsts>(
                              nullptr, "ebreak", ActionTags::EXECUTE_TAG));
            if (enable_syscall_emulation)
            {
                inst_handlers.emplace(
                    "ecall",
                    pegasus::Action::createAction<&RviInsts::ecallHandlerSystemEmulation_<RV64>,
                                                  RviInsts>(nullptr, "ecall",
                                                            ActionTags::EXECUTE_TAG));
            }
            else
            {
                inst_handlers.emplace(
                    "ecall",
                    pegasus::Action::createAction<&RviInsts::ecallHandler_<RV64>, RviInsts>(
                        nullptr, "ecall", ActionTags::EXECUTE_TAG));
            }
            inst_handlers.emplace("fence",
                                  pegasus::Action::createAction<&RviInsts::fenceHandler_, RviInsts>(
                                      nullptr, "fence", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jal", pegasus::Action::createAction<&RviInsts::jalHandler_<RV64>, RviInsts>(
                           nullptr, "jal", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jalr", pegasus::Action::createAction<&RviInsts::jalrHandler_<RV64>, RviInsts>(
                            nullptr, "jalr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lb",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint8_t, SIGN_EXTEND>,
                                              RviInsts>(nullptr, "lb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lbu",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint8_t>, RviInsts>(
                    nullptr, "lbu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ld",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint64_t>, RviInsts>(
                    nullptr, "ld", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lh",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint16_t, SIGN_EXTEND>,
                                              RviInsts>(nullptr, "lh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lhu",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint16_t>, RviInsts>(
                    nullptr, "lhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "li", pegasus::Action::createAction<&RviInsts::liHandler_<RV64>, RviInsts>(
                          nullptr, "li", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lui", pegasus::Action::createAction<&RviInsts::luiHandler_<RV64>, RviInsts>(
                           nullptr, "lui", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lw",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint32_t, SIGN_EXTEND>,
                                              RviInsts>(nullptr, "lw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lwu",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV64, uint32_t>, RviInsts>(
                    nullptr, "lwu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mret",
                pegasus::Action::createAction<&RviInsts::xretHandler_<RV64, PrivMode::MACHINE>,
                                              RviInsts>(nullptr, "mret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mv", pegasus::Action::createAction<&RviInsts::mvHandler_<RV64>, RviInsts>(
                          nullptr, "mv", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("nop",
                                  pegasus::Action::createAction<&RviInsts::nopHandler_, RviInsts>(
                                      nullptr, "nop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "or", pegasus::Action::createAction<
                          &RviInsts::integer_reg_regHandler_<RV64, std::bit_or<RV64>>, RviInsts>(
                          nullptr, "or", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ori", pegasus::Action::createAction<
                           &RviInsts::integer_reg_immHandler_<RV64, std::bit_or<RV64>>, RviInsts>(
                           nullptr, "ori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sb",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV64, uint8_t>, RviInsts>(
                    nullptr, "sb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sd",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV64, uint64_t>, RviInsts>(
                    nullptr, "sd", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sfence_vma",
                pegasus::Action::createAction<&RviInsts::sfence_vmaHandler_<RV64>, RviInsts>(
                    nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV64, uint16_t>, RviInsts>(
                    nullptr, "sh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sll",
                                  pegasus::Action::createAction<&RviInsts::sllHandler_, RviInsts>(
                                      nullptr, "sll", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slli",
                                  pegasus::Action::createAction<&RviInsts::slliHandler_, RviInsts>(
                                      nullptr, "slli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slliw",
                                  pegasus::Action::createAction<&RviInsts::slliwHandler_, RviInsts>(
                                      nullptr, "slliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sllw",
                                  pegasus::Action::createAction<&RviInsts::sllwHandler_, RviInsts>(
                                      nullptr, "sllw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slt", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV64, std::less<int64_t>>, RviInsts>(
                           nullptr, "slt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slti", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV64, std::less<int64_t>>, RviInsts>(
                            nullptr, "slti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltiu", pegasus::Action::createAction<
                             &RviInsts::integer_reg_immHandler_<RV64, std::less<RV64>>, RviInsts>(
                             nullptr, "sltiu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltu", pegasus::Action::createAction<
                            &RviInsts::integer_reg_regHandler_<RV64, std::less<RV64>>, RviInsts>(
                            nullptr, "sltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sra", pegasus::Action::createAction<&RviInsts::sraHandler_<RV64>, RviInsts>(
                           nullptr, "sra", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srai", pegasus::Action::createAction<&RviInsts::sraiHandler_<RV64>, RviInsts>(
                            nullptr, "srai", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sraiw",
                                  pegasus::Action::createAction<&RviInsts::sraiwHandler_, RviInsts>(
                                      nullptr, "sraiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sraw",
                                  pegasus::Action::createAction<&RviInsts::srawHandler_, RviInsts>(
                                      nullptr, "sraw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sret",
                pegasus::Action::createAction<&RviInsts::xretHandler_<RV64, PrivMode::SUPERVISOR>,
                                              RviInsts>(nullptr, "sret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srl", pegasus::Action::createAction<&RviInsts::srlHandler_<RV64>, RviInsts>(
                           nullptr, "srl", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srli", pegasus::Action::createAction<&RviInsts::srliHandler_<RV64>, RviInsts>(
                            nullptr, "srli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srliw",
                                  pegasus::Action::createAction<&RviInsts::srliwHandler_, RviInsts>(
                                      nullptr, "srliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srlw",
                                  pegasus::Action::createAction<&RviInsts::srlwHandler_, RviInsts>(
                                      nullptr, "srlw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sub", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV64, std::minus<RV64>>, RviInsts>(
                           nullptr, "sub", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("subw",
                                  pegasus::Action::createAction<&RviInsts::subwHandler_, RviInsts>(
                                      nullptr, "subw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sw",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV64, uint32_t>, RviInsts>(
                    nullptr, "sw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "wfi", pegasus::Action::createAction<&RviInsts::wfiHandler_<RV64>, RviInsts>(
                           nullptr, "wfi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xor", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV64, std::bit_xor<RV64>>, RviInsts>(
                           nullptr, "xor", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xori", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV64, std::bit_xor<RV64>>, RviInsts>(
                            nullptr, "xori", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "add", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV32, std::plus<RV32>>, RviInsts>(
                           nullptr, "add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "addi", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV32, std::plus<RV32>>, RviInsts>(
                            nullptr, "addi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "and", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV32, std::bit_and<RV32>>, RviInsts>(
                           nullptr, "and", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "andi", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV32, std::bit_and<RV32>>, RviInsts>(
                            nullptr, "andi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "auipc", pegasus::Action::createAction<&RviInsts::auipcHandler_<RV32>, RviInsts>(
                             nullptr, "auipc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "beq", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV32, std::equal_to<int32_t>>, RviInsts>(
                           nullptr, "beq", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bge", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV32, std::greater_equal<int32_t>>, RviInsts>(
                           nullptr, "bge", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bgeu",
                pegasus::Action::createAction<
                    &RviInsts::branchHandler_<RV32, std::greater_equal<uint32_t>>, RviInsts>(
                    nullptr, "bgeu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "blt",
                pegasus::Action::createAction<&RviInsts::branchHandler_<RV32, std::less<int32_t>>,
                                              RviInsts>(nullptr, "blt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bltu",
                pegasus::Action::createAction<&RviInsts::branchHandler_<RV32, std::less<uint32_t>>,
                                              RviInsts>(nullptr, "bltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "bne", pegasus::Action::createAction<
                           &RviInsts::branchHandler_<RV32, std::not_equal_to<uint32_t>>, RviInsts>(
                           nullptr, "bne", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ebreak", pegasus::Action::createAction<&RviInsts::ebreakHandler_, RviInsts>(
                              nullptr, "ebreak", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ecall", pegasus::Action::createAction<&RviInsts::ecallHandler_<RV32>, RviInsts>(
                             nullptr, "ecall", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fence",
                                  pegasus::Action::createAction<&RviInsts::fenceHandler_, RviInsts>(
                                      nullptr, "fence", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jal", pegasus::Action::createAction<&RviInsts::jalHandler_<RV32>, RviInsts>(
                           nullptr, "jal", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "jalr", pegasus::Action::createAction<&RviInsts::jalrHandler_<RV32>, RviInsts>(
                            nullptr, "jalr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lb",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV32, uint8_t, SIGN_EXTEND>,
                                              RviInsts>(nullptr, "lb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lbu",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV32, uint8_t>, RviInsts>(
                    nullptr, "lbu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lh",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV32, uint16_t, SIGN_EXTEND>,
                                              RviInsts>(nullptr, "lh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lhu",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV32, uint16_t>, RviInsts>(
                    nullptr, "lhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "li", pegasus::Action::createAction<&RviInsts::liHandler_<RV32>, RviInsts>(
                          nullptr, "li", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lui", pegasus::Action::createAction<&RviInsts::luiHandler_<RV32>, RviInsts>(
                           nullptr, "lui", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lw",
                pegasus::Action::createAction<&RviInsts::loadHandler_<RV32, uint32_t>, RviInsts>(
                    nullptr, "lw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mret",
                pegasus::Action::createAction<&RviInsts::xretHandler_<RV32, PrivMode::MACHINE>,
                                              RviInsts>(nullptr, "mret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mv", pegasus::Action::createAction<&RviInsts::mvHandler_<RV32>, RviInsts>(
                          nullptr, "mv", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("nop",
                                  pegasus::Action::createAction<&RviInsts::nopHandler_, RviInsts>(
                                      nullptr, "nop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "or", pegasus::Action::createAction<
                          &RviInsts::integer_reg_regHandler_<RV32, std::bit_or<RV32>>, RviInsts>(
                          nullptr, "or", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ori", pegasus::Action::createAction<
                           &RviInsts::integer_reg_immHandler_<RV32, std::bit_or<RV32>>, RviInsts>(
                           nullptr, "ori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sb",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV32, uint8_t>, RviInsts>(
                    nullptr, "sb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sfence_vma",
                pegasus::Action::createAction<&RviInsts::sfence_vmaHandler_<RV32>, RviInsts>(
                    nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV32, uint16_t>, RviInsts>(
                    nullptr, "sh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sll",
                                  pegasus::Action::createAction<&RviInsts::sllHandler_, RviInsts>(
                                      nullptr, "sll", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slli",
                                  pegasus::Action::createAction<&RviInsts::slliHandler_, RviInsts>(
                                      nullptr, "slli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slt", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV32, std::less<int32_t>>, RviInsts>(
                           nullptr, "slt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slti", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV32, std::less<int32_t>>, RviInsts>(
                            nullptr, "slti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltiu", pegasus::Action::createAction<
                             &RviInsts::integer_reg_immHandler_<RV32, std::less<RV32>>, RviInsts>(
                             nullptr, "sltiu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltu", pegasus::Action::createAction<
                            &RviInsts::integer_reg_regHandler_<RV32, std::less<RV32>>, RviInsts>(
                            nullptr, "sltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sra", pegasus::Action::createAction<&RviInsts::sraHandler_<RV32>, RviInsts>(
                           nullptr, "sra", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srai", pegasus::Action::createAction<&RviInsts::sraiHandler_<RV32>, RviInsts>(
                            nullptr, "srai", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sret",
                pegasus::Action::createAction<&RviInsts::xretHandler_<RV32, PrivMode::SUPERVISOR>,
                                              RviInsts>(nullptr, "sret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srl", pegasus::Action::createAction<&RviInsts::srlHandler_<RV32>, RviInsts>(
                           nullptr, "srl", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srli", pegasus::Action::createAction<&RviInsts::srliHandler_<RV32>, RviInsts>(
                            nullptr, "srli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sub", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV32, std::minus<RV32>>, RviInsts>(
                           nullptr, "sub", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sw",
                pegasus::Action::createAction<&RviInsts::storeHandler_<RV32, uint32_t>, RviInsts>(
                    nullptr, "sw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "wfi", pegasus::Action::createAction<&RviInsts::wfiHandler_<RV32>, RviInsts>(
                           nullptr, "wfi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xor", pegasus::Action::createAction<
                           &RviInsts::integer_reg_regHandler_<RV32, std::bit_xor<RV32>>, RviInsts>(
                           nullptr, "xor", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xori", pegasus::Action::createAction<
                            &RviInsts::integer_reg_immHandler_<RV32, std::bit_xor<RV32>>, RviInsts>(
                            nullptr, "xori", ActionTags::EXECUTE_TAG));
        }
    }

    template void RviInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RviInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RviInsts::getInstHandlers<RV32>(std::map<std::string, Action> &, bool);
    template void RviInsts::getInstHandlers<RV64>(std::map<std::string, Action> &, bool);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RviInsts::integer_reg_regHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const XLEN rd_val = OPERATOR()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::addwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + rs2_val));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::subwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val - rs2_val));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, class OPERATOR>
    Action::ItrType RviInsts::integer_reg_immHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const uint64_t imm = inst->getImmediate();
        const uint64_t rd_val = OPERATOR()(rs1_val, imm);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::addiwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t imm = inst->getImmediate();
        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + imm));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::mvHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rs1_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::nopHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        (void)state;
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RviInsts::computeAddressHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm = inst->getImmediate();
        const XLEN vaddr = rs1_val + imm;
        inst->getTranslationState()->makeRequest(vaddr, sizeof(SIZE));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, bool SIGN_EXTEND>
    Action::ItrType RviInsts::loadHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();
        if constexpr (SIGN_EXTEND)
        {
            const XLEN rd_val = signExtend<SIZE, XLEN>(state->readMemory<SIZE>(paddr));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        }
        else
        {
            const XLEN rd_val = state->readMemory<SIZE>(paddr);
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        }
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RviInsts::storeHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const uint64_t paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();
        state->writeMemory<SIZE>(paddr, rs2_val);
        return ++action_it;
    }

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RviInsts::branchHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        if (OPERATOR()(rs1_val, rs2_val))
        {
            const XLEN pc = state->getPc();
            const XLEN imm = inst->getImmediate();
            const XLEN branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::jalHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        XLEN rd_val = state->getPc() + inst->getOpcodeSize();
        const XLEN imm = inst->getImmediate();
        const XLEN jump_target = state->getPc() + imm;
        state->setNextPc(jump_target);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::jalrHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        XLEN rd_val = state->getPc() + inst->getOpcodeSize();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm = inst->getImmediate();
        const XLEN jump_target = (rs1_val + imm) & ~std::make_signed_t<XLEN>(1);
        state->setNextPc(jump_target);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::liHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t imm = inst->getImmediate();
        WRITE_INT_REG<XLEN>(state, inst->getRd(), imm);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::luiHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t imm = inst->getImmediate();
        WRITE_INT_REG<XLEN>(state, inst->getRd(), imm);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::auipcHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN imm = inst->getImmediate();
        const XLEN pc = state->getPc();
        const XLEN rd_val = ((std::make_signed_t<XLEN>)(imm + pc) << (64 - (state->getXlen())))
                            >> (64 - (state->getXlen()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::srlHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const XLEN rd_val = (XLEN)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::srliwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t shift_amount = inst->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::sllHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        const uint64_t rd_val = rs1_val << (rs2_val & (state->getXlen() - 1));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::sraiHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using SXLEN = std::make_signed_t<XLEN>;
        const PegasusInstPtr & inst = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const SXLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN shift_amount = inst->getImmediate() & (state->getXlen() - 1);
        const SXLEN rd_val = (SXLEN)(rs1_val >> shift_amount);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::srawHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const int32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::srliHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN shift_amount = inst->getImmediate() & (state->getXlen() - 1);
        const XLEN rd_val = (XLEN)(rs1_val >> shift_amount);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::sllwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint32_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << (rs2_val & 0x1F));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::slliwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t shift_amount = inst->getImmediate() & 0x1F;
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << shift_amount);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::sraiwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const int32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t shift_amount = inst->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::sraHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using SXLEN = std::make_signed_t<XLEN>;
        const PegasusInstPtr & inst = state->getCurrentInst();

        const SXLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const XLEN rd_val = (int64_t)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::slliHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t shift_amount = inst->getImmediate() & (state->getXlen() - 1);
        const uint64_t rd_val = rs1_val << shift_amount;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RviInsts::srlwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const uint32_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, PrivMode PRIV_MODE>
    Action::ItrType RviInsts::xretHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        static_assert(PRIV_MODE == PrivMode::MACHINE || PRIV_MODE == PrivMode::SUPERVISOR);

        // mret can only be executed in Machine mode
        // sret can be executed in Supervisor or Machine mode
        if (state->getPrivMode() < PRIV_MODE)
        {
            THROW_ILLEGAL_INST;
        }

        PrivMode prev_priv_mode = PrivMode::INVALID;
        bool prev_virt_mode = state->getVirtualMode();
        if constexpr (PRIV_MODE == PrivMode::MACHINE)
        {
            // Update the PC with MEPC value
            state->setNextPc(READ_CSR_REG<XLEN>(state, MEPC) & state->getPcAlignmentMask());

            // Get the previous privilege mode from the MPP field of MSTATUS
            prev_priv_mode = (PrivMode)READ_CSR_FIELD<XLEN>(state, MSTATUS, "mpp");

            // Get the previous virtual mode from the MPV field of MSTATUS
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                prev_virt_mode = (bool)READ_CSR_FIELD<XLEN>(state, MSTATUS, "mpv");
            }
            else
            {
                prev_virt_mode = (bool)READ_CSR_FIELD<XLEN>(state, MSTATUSH, "mpv");
            }

            // If the mret instruction changes the privilege mode to a mode less privileged
            // than Machine mode, the MPRV bit is reset to 0
            if (prev_priv_mode != PrivMode::MACHINE)
            {
                // TODO: Will need to update the load/store translation mode when translation is
                // supported
                WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mprv", (XLEN)0);
            }

            // Set MIE = MPIE and reset MPIE
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mie",
                                  READ_CSR_FIELD<XLEN>(state, MSTATUS, "mpie"));
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpie", (XLEN)1);

            // Reset MPP
            // TODO: Check if User mode is available
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpp", (XLEN)PrivMode::USER);

            // Reset MPV
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpv", 0);
            }
            else
            {
                WRITE_CSR_FIELD<XLEN>(state, MSTATUSH, "mpv", 0);
            }
        }
        else
        {
            // When TSR=1, attempts to execute SRET in S-mode will
            // raise an illegal instruction exception
            const uint32_t tsr_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "tsr");
            if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tsr_val)
            {
                THROW_ILLEGAL_INST;
            }

            // Update the PC with SEPC value
            state->setNextPc(READ_CSR_REG<XLEN>(state, SEPC) & state->getPcAlignmentMask());

            // Get the previous privilege mode from the SPP field of MSTATUS
            prev_priv_mode = (PrivMode)READ_CSR_FIELD<XLEN>(state, MSTATUS, "spp");

            if (state->hasHypervisor())
            {
                prev_virt_mode = (bool)READ_CSR_FIELD<XLEN>(state, HSTATUS, "spv");
                WRITE_CSR_FIELD<XLEN>(state, HSTATUS, "spv", (XLEN)0);
            }

            // Reset the MPRV bit
            // TODO: Will need to update the load/store translation mode when translation is
            // supported
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mprv", (XLEN)0);

            // Set SIE = MSTATUS[SPIE] and reset SPIE
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "sie",
                                  READ_CSR_FIELD<XLEN>(state, MSTATUS, "spie"));
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "spie", (XLEN)1);

            // Reset MPP
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "spp", (XLEN)PrivMode::USER);
        }

        // TODO: Update MSTATUSH

        // Update the privilege mode to the previous privilege mode
        state->setPrivMode(prev_priv_mode, prev_virt_mode);

        // Update the MMU Mode from SATP and MSTATUS
        state->changeMMUMode<XLEN>();

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::ecallHandlerSystemEmulation_(pegasus::PegasusState* state,
                                                           Action::ItrType action_it)
    {
        // x10 -> x16 are the function arguments.
        // x17 holds the system call number, first item on the stack
        SystemCallStack call_stack = {READ_INT_REG<XLEN>(state, 17), READ_INT_REG<XLEN>(state, 10),
                                      READ_INT_REG<XLEN>(state, 11), READ_INT_REG<XLEN>(state, 12),
                                      READ_INT_REG<XLEN>(state, 13), READ_INT_REG<XLEN>(state, 14),
                                      READ_INT_REG<XLEN>(state, 15), READ_INT_REG<XLEN>(state, 16)};

        auto ret_code = static_cast<XLEN>(state->emulateSystemCall(call_stack));
        WRITE_INT_REG<XLEN>(state, 10, ret_code);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::ecallHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        switch (state->getPrivMode())
        {
            case PrivMode::USER:
                THROW_USER_ECALL;
                break;
            case PrivMode::SUPERVISOR:
                THROW_SUPERVISOR_ECALL;
                break;
            case PrivMode::MACHINE:
                THROW_MACHINE_ECALL;
                break;
            default:
                sparta_assert(false, "Invalid privilege mode!");
                break;
        }

        return ++action_it;
    }

    Action::ItrType RviInsts::ebreakHandler_(pegasus::PegasusState* state, Action::ItrType)
    {
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

        THROW_BREAKPOINT;
    }

    Action::ItrType RviInsts::fenceHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::sfence_vmaHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
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

        const uint32_t tvm_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "tvm");
        if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tvm_val)
        {
            THROW_ILLEGAL_INST;
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RviInsts::wfiHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
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

        const uint32_t tw_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "tw");
        if (tw_val)
        {
            THROW_ILLEGAL_INST;
        }

        if (state->getStopSimOnWfi())
        {
            PegasusState::SimState* sim_state = state->getSimState();
            sim_state->sim_stopped = true;
            ActionGroup* inst_action_group = state->getCurrentInst()->getActionGroup();
            inst_action_group->setNextActionGroup(state->getStopSimActionGroup());
        }
        return ++action_it;
    }

} // namespace pegasus
