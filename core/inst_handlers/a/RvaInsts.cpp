#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/a/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd.d",
                atlas::Action::createAction<&RvaInsts::amoadd_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoadd_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoadd.w",
                atlas::Action::createAction<&RvaInsts::amoadd_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoadd_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoand.d",
                atlas::Action::createAction<&RvaInsts::amoand_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoand_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoand.w",
                atlas::Action::createAction<&RvaInsts::amoand_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoand_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomax.d",
                atlas::Action::createAction<&RvaInsts::amomax_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomax_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomax.w",
                atlas::Action::createAction<&RvaInsts::amomax_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomax_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomaxu.d",
                atlas::Action::createAction<&RvaInsts::amomaxu_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomaxu_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomaxu.w",
                atlas::Action::createAction<&RvaInsts::amomaxu_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomaxu_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomin.d",
                atlas::Action::createAction<&RvaInsts::amomin_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomin_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomin.w",
                atlas::Action::createAction<&RvaInsts::amomin_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amomin_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amominu.d",
                atlas::Action::createAction<&RvaInsts::amominu_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amominu_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amominu.w",
                atlas::Action::createAction<&RvaInsts::amominu_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amominu_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoor.d",
                atlas::Action::createAction<&RvaInsts::amoor_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoor_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoor.w",
                atlas::Action::createAction<&RvaInsts::amoor_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoor_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoswap.d",
                atlas::Action::createAction<&RvaInsts::amoswap_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoswap_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoswap.w",
                atlas::Action::createAction<&RvaInsts::amoswap_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoswap_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoxor.d",
                atlas::Action::createAction<&RvaInsts::amoxor_d_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoxor_d",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoxor.w",
                atlas::Action::createAction<&RvaInsts::amoxor_w_64_compute_address_handler,
                                            RvaInsts>(nullptr, "amoxor_w",
                                                      ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lr.d",
                atlas::Action::createAction<&RvaInsts::lr_d_64_compute_address_handler, RvaInsts>(
                    nullptr, "lr_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lr.w",
                atlas::Action::createAction<&RvaInsts::lr_w_64_compute_address_handler, RvaInsts>(
                    nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sc.d",
                atlas::Action::createAction<&RvaInsts::sc_d_64_compute_address_handler, RvaInsts>(
                    nullptr, "sc_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sc.w",
                atlas::Action::createAction<&RvaInsts::sc_w_64_compute_address_handler, RvaInsts>(
                    nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template <typename XLEN>
    void RvaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd.d", atlas::Action::createAction<&RvaInsts::amoadd_d_64_handler, RvaInsts>(
                                nullptr, "amoadd_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoadd.w", atlas::Action::createAction<&RvaInsts::amoadd_w_64_handler, RvaInsts>(
                                nullptr, "amoadd_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand.d", atlas::Action::createAction<&RvaInsts::amoand_d_64_handler, RvaInsts>(
                                nullptr, "amoand_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand.w", atlas::Action::createAction<&RvaInsts::amoand_w_64_handler, RvaInsts>(
                                nullptr, "amoand_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax.d", atlas::Action::createAction<&RvaInsts::amomax_d_64_handler, RvaInsts>(
                                nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax.w", atlas::Action::createAction<&RvaInsts::amomax_w_64_handler, RvaInsts>(
                                nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu.d", atlas::Action::createAction<&RvaInsts::amomaxu_d_64_handler, RvaInsts>(
                                 nullptr, "amomaxu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu.w", atlas::Action::createAction<&RvaInsts::amomaxu_w_64_handler, RvaInsts>(
                                 nullptr, "amomaxu_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin.d", atlas::Action::createAction<&RvaInsts::amomin_d_64_handler, RvaInsts>(
                                nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin.w", atlas::Action::createAction<&RvaInsts::amomin_w_64_handler, RvaInsts>(
                                nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu.d", atlas::Action::createAction<&RvaInsts::amominu_d_64_handler, RvaInsts>(
                                 nullptr, "amominu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu.w", atlas::Action::createAction<&RvaInsts::amominu_w_64_handler, RvaInsts>(
                                 nullptr, "amominu_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor.d", atlas::Action::createAction<&RvaInsts::amoor_d_64_handler, RvaInsts>(
                               nullptr, "amoor_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor.w", atlas::Action::createAction<&RvaInsts::amoor_w_64_handler, RvaInsts>(
                               nullptr, "amoor_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap.d", atlas::Action::createAction<&RvaInsts::amoswap_d_64_handler, RvaInsts>(
                                 nullptr, "amoswap_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap.w", atlas::Action::createAction<&RvaInsts::amoswap_w_64_handler, RvaInsts>(
                                 nullptr, "amoswap_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor.d", atlas::Action::createAction<&RvaInsts::amoxor_d_64_handler, RvaInsts>(
                                nullptr, "amoxor_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor.w", atlas::Action::createAction<&RvaInsts::amoxor_w_64_handler, RvaInsts>(
                                nullptr, "amoxor_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lr.d",
                                  atlas::Action::createAction<&RvaInsts::lr_d_64_handler, RvaInsts>(
                                      nullptr, "lr_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lr.w",
                                  atlas::Action::createAction<&RvaInsts::lr_w_64_handler, RvaInsts>(
                                      nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sc.d",
                                  atlas::Action::createAction<&RvaInsts::sc_d_64_handler, RvaInsts>(
                                      nullptr, "sc_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sc.w",
                                  atlas::Action::createAction<&RvaInsts::sc_w_64_handler, RvaInsts>(
                                      nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RvaInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvaInsts::amoadd_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoadd_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, std::plus>(state);
    }

    ActionGroup* RvaInsts::amoadd_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoadd_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, std::plus>(state);
    }

    ActionGroup* RvaInsts::amoand_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoand_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, std::bit_and>(state);
    }

    ActionGroup* RvaInsts::amoand_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoand_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, std::bit_and>(state);
    }

    ActionGroup* RvaInsts::amomax_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomax_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, MaxFunctor, false>(state);
    }

    ActionGroup* RvaInsts::amomax_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomax_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, MaxFunctor, false>(state);
    }

    ActionGroup* RvaInsts::amomaxu_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomaxu_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, MaxFunctor>(state);
    }

    ActionGroup* RvaInsts::amomaxu_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomaxu_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, MaxFunctor>(state);
    }

    ActionGroup* RvaInsts::amomin_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomin_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, MinFunctor, false>(state);
    }

    ActionGroup* RvaInsts::amomin_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amomin_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, MinFunctor, false>(state);
    }

    ActionGroup* RvaInsts::amominu_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amominu_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, MinFunctor>(state);
    }

    ActionGroup* RvaInsts::amominu_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amominu_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, MinFunctor>(state);
    }

    ActionGroup* RvaInsts::amoor_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoor_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, std::bit_or>(state);
    }

    ActionGroup* RvaInsts::amoor_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoor_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, std::bit_or>(state);
    }

    ActionGroup* RvaInsts::amoswap_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoswap_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, SwapFunctor>(state);
    }

    ActionGroup* RvaInsts::amoswap_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoswap_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, SwapFunctor>(state);
    }

    ActionGroup* RvaInsts::amoxor_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoxor_d_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, D, std::bit_xor>(state);
    }

    ActionGroup* RvaInsts::amoxor_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::amoxor_w_64_handler(atlas::AtlasState* state)
    {
        return amo_handler<RV64, W, std::bit_xor>(state);
    }

    ActionGroup* RvaInsts::lr_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::lr_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint64_t>(paddr);
        inst->getRd()->write(rd_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::lr_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = signExtend<uint32_t, uint64_t>(state->readMemory<uint32_t>(paddr));
        inst->getRd()->write(rd_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::sc_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = inst->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint64_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvaInsts::sc_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = inst->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint32_t>(paddr, rs2_val);
        return nullptr;
    }
} // namespace atlas
