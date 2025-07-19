#include "core/PegasusExtractor.hpp"
#include "core/PegasusState.hpp"
#include "core/Execute.hpp"

namespace pegasus
{
    template <typename ValueType>
    ValueType getUarchJsonValue(const boost::json::object & entry, const std::string & key)
    {
        if (SPARTA_EXPECT_FALSE(entry.find(key) == entry.end()))
        {
            sparta_assert(false, "Missing key in uarch json: " << key);
        }
        return boost::json::value_to<ValueType>(entry.find(key)->value());
    }

    PegasusExtractor::PegasusExtractor(const boost::json::object & uarch_json,
                                       const PegasusState* state) :
        mnemonic_(getUarchJsonValue<std::string>(uarch_json, "mnemonic")),
        inst_handler_name_(getUarchJsonValue<std::string>(uarch_json, "handler")),
        is_memory_inst_(getUarchJsonValue<bool>(uarch_json, "memory")),
        is_cof_inst_(getUarchJsonValue<bool>(uarch_json, "cof")),
        inst_action_group_(mnemonic_)
    {
        const Execute* execute_unit = state->getExecuteUnit();

        const auto xlen = state->getXlen();
        const Execute::InstHandlersMap* inst_compute_address_handlers =
            (xlen == 64) ? execute_unit->getInstComputeAddressHandlersMap<RV64>()
                         : execute_unit->getInstComputeAddressHandlersMap<RV32>();
        if (is_memory_inst_)
        {
            try
            {
                const Action & inst_compute_address_handler =
                    inst_compute_address_handlers->at(inst_handler_name_);
                inst_action_group_.addAction(inst_compute_address_handler);
            }
            catch (const std::out_of_range & excp)
            {
                sparta_assert(inst_handler_name_ == "nop",
                              "Missing key in rv"
                                  << std::to_string(xlen)
                                  << " inst compute address handler map: " << inst_handler_name_);
            }
        }

        const Execute::InstHandlersMap* inst_handlers =
            (xlen == 64) ? execute_unit->getInstHandlersMap<RV64>()
                         : execute_unit->getInstHandlersMap<RV32>();
        try
        {
            const Action & inst_handler = inst_handlers->at(inst_handler_name_);
            inst_action_group_.addAction(inst_handler);
        }
        catch (const std::out_of_range & excp)
        {
            sparta_assert(false, "Missing key in rv"
                                     << std::to_string(xlen)
                                     << " inst handler map: " << inst_handler_name_);
        }
    }
} // namespace pegasus
