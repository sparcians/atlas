#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename T> struct MaxFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const { return lhs > rhs ? lhs : rhs; }
    };

    template <typename T> struct MinFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const { return lhs > rhs ? rhs : lhs; }
    };

    template <typename T> struct SwapFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const
        {
            (void)lhs;
            return rhs;
        }
    };

    template <typename XLEN>
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amoadd_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amoadd_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amoand_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amomax_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amomaxu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amomin_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amominu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amoor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amoswap_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "amoxor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInsts>(
                nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amoadd_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoand_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amoand_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomax_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amomax_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomaxu_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amomaxu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomin_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amomin_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amominu_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amominu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoor_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amoor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoswap_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amoswap_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoxor_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "amoxor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lr_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "lr_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sc_d",
                pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>, RvaInsts>(
                    nullptr, "sc_d", ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template <typename XLEN>
    void RvaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::plus<D>>,
                                              RvaInsts>(nullptr, "amoadd_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoadd_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::plus<W>>,
                                              RvaInsts>(nullptr, "amoadd_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_and<D>>,
                                              RvaInsts>(nullptr, "amoand_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_and<W>>,
                                              RvaInsts>(nullptr, "amoand_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_d",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV64, D, MaxFunctor<std::make_signed_t<D>>, false>,
                    RvaInsts>(nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV64, W, MaxFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, MaxFunctor<D>>,
                                              RvaInsts>(nullptr, "amomaxu_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, MaxFunctor<W>>,
                                              RvaInsts>(nullptr, "amomaxu_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_d",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV64, D, MinFunctor<std::make_signed_t<D>>, false>,
                    RvaInsts>(nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV64, W, MinFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, MinFunctor<D>>,
                                              RvaInsts>(nullptr, "amominu_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, MinFunctor<W>>,
                                              RvaInsts>(nullptr, "amominu_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_or<D>>,
                                              RvaInsts>(nullptr, "amoor_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_or<W>>,
                                              RvaInsts>(nullptr, "amoor_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, SwapFunctor<D>>,
                                              RvaInsts>(nullptr, "amoswap_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, SwapFunctor<W>>,
                                              RvaInsts>(nullptr, "amoswap_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_xor<D>>,
                                              RvaInsts>(nullptr, "amoxor_d",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_xor<W>>,
                                              RvaInsts>(nullptr, "amoxor_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_d",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV64, uint64_t>, RvaInsts>(
                    nullptr, "lr_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_w",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV64, uint32_t>, RvaInsts>(
                    nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_d",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV64, uint64_t>, RvaInsts>(
                    nullptr, "sc_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_w",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV64, uint32_t>, RvaInsts>(
                    nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "amoadd_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::plus<W>>,
                                              RvaInsts>(nullptr, "amoadd_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_and<W>>,
                                              RvaInsts>(nullptr, "amoand_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV32, W, MaxFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, MaxFunctor<W>>,
                                              RvaInsts>(nullptr, "amomaxu_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w",
                pegasus::Action::createAction<
                    &RvaInsts::amoHandler_<RV32, W, MinFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, MinFunctor<W>>,
                                              RvaInsts>(nullptr, "amominu_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_or<W>>,
                                              RvaInsts>(nullptr, "amoor_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, SwapFunctor<W>>,
                                              RvaInsts>(nullptr, "amoswap_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_xor<W>>,
                                              RvaInsts>(nullptr, "amoxor_w",
                                                        ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_w",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV32, uint32_t>, RvaInsts>(
                    nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_w",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV32, uint32_t>, RvaInsts>(
                    nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvaInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename T>
    Action::ItrType RvaInsts::computeAddressHandler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<T, RV64> || std::is_same_v<T, RV32>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1Reg()->dmiRead<uint64_t>();
        const T imm = inst->getImmediate();
        const T vaddr = rs1_val + imm;
        inst->getTranslationState()->makeRequest(vaddr, sizeof(T));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, typename OP, bool U>
    Action::ItrType RvaInsts::amoHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, W> || std::is_same_v<SIZE, D>);
        static_assert(sizeof(XLEN) >= sizeof(SIZE));

        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();

        XLEN rd_val = 0;
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            rd_val = signExtend<SIZE, XLEN>(state->readMemory<SIZE>(paddr));
        }
        else
        {
            rd_val = state->readMemory<SIZE>(paddr);
        }
        // Must read the RS2 value before writing the Rd (might be the
        // same register!)
        const XLEN rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
        inst->getRdReg()->write(rd_val);
        state->writeMemory<SIZE>(paddr, OP()(rd_val, rs2_val));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvaInsts::lr_handler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto xlation_state = inst->getTranslationState();

        // Make the reservation
        state->getReservation() = xlation_state->getResult().getVAddr();

        // Get the memory
        const uint64_t paddr = xlation_state->getResult().getPAddr();
        xlation_state->popResult();
        const XLEN rd_val = state->readMemory<SIZE>(paddr);
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            inst->getRdReg()->write(signExtend<SIZE, XLEN>(rd_val));
        }
        else
        {
            inst->getRdReg()->write(rd_val);
        }

        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvaInsts::sc_handler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto xlation_state = inst->getTranslationState();

        XLEN sc_bad = 1; // assume bad
        if (auto & resv = state->getReservation(); resv.isValid())
        {
            if (resv == xlation_state->getResult().getVAddr())
            {
                const uint64_t paddr = xlation_state->getResult().getPAddr();
                const uint64_t rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
                state->writeMemory<SIZE>(paddr, rs2_val);
                sc_bad = 0;
            }
            resv.clearValid();
        }
        xlation_state->popResult();
        inst->getRdReg()->dmiWrite<XLEN>(sc_bad);
        return ++action_it;
    }
} // namespace pegasus
