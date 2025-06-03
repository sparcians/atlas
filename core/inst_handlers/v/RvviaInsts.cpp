#include "core/inst_handlers/v/RvviaInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VectorState.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvviaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vadd.vv",
            atlas::Action::createAction<&RvviaInsts::viavvHandler_<std::plus>, RvviaInsts>(
                nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            atlas::Action::createAction<&RvviaInsts::viavxHandler_<XLEN, std::plus>, RvviaInsts>(
                nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            atlas::Action::createAction<&RvviaInsts::viaviHandler_<std::plus>, RvviaInsts>(
                nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));
    }

    template void RvviaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvviaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <uint8_t ElemWidth, typename Functor>
    Action::ItrType viavvHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        VectorElements<ElemWidth> elems_vs1{state, inst->getRs1(), false};
        VectorElements<ElemWidth> elems_vs2{state, inst->getRs2(), false};
        VectorElements<ElemWidth> elems_vd{state, inst->getRd(), false};

        for (auto iter_vs1 = elems_vs1.begin(), iter_vs2 = elems_vs2.begin(),
                  iter_vd = elems_vd.begin();
             iter_vs1 != elems_vs1.end(); ++iter_vs1, ++iter_vs2, ++iter_vd)
        {
            iter_vd->setVal(Functor()(iter_vs1->getVal(), iter_vs2->getVal()));
        }

        return ++action_it;
    }

    template <template <typename> typename FunctorTemp>
    Action::ItrType RvviaInsts::viavvHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config_ptr = state->getVectorConfig();
        switch (vector_config_ptr->getSEW())
        {
            case 8:
                return viavvHelper<8, FunctorTemp<uint8_t>>(state, action_it);
                break;
            case 16:
                return viavvHelper<16, FunctorTemp<uint16_t>>(state, action_it);
                break;
            case 32:
                return viavvHelper<32, FunctorTemp<uint32_t>>(state, action_it);
                break;
            case 64:
                return viavvHelper<64, FunctorTemp<uint64_t>>(state, action_it);
                break;
            default:
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, template <typename> typename OP>
    Action::ItrType RvviaInsts::viavxHandler_(AtlasState* state, Action::ItrType action_it)
    {
        (void)state;
        return ++action_it;
    }

    template <template <typename> typename OP>
    Action::ItrType RvviaInsts::viaviHandler_(AtlasState* state, Action::ItrType action_it)
    {
        (void)state;
        return ++action_it;
    }

} // namespace atlas
