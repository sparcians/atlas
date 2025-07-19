
#include "RegisterDefnsJSON.hpp"
#include "mavis/JSONUtils.hpp"

namespace pegasus
{
    void RegisterDefnsFromJSON::parse_(const std::string & register_defns_json_filename)
    {
        // Parse the JSON file
        const boost::json::value document = mavis::parseJSON(register_defns_json_filename);

        for (auto & reg : document.as_array())
        {
            if (auto value = reg.as_object().if_contains("enabled"); value)
            {
                if (false == value->as_bool())
                {
                    continue;
                }
            }

            const sparta::RegisterBase::ident_type id = reg.at("num").as_int64();

            cached_strings_.emplace_back(std::string(reg.at("name").as_string()));
            const char* name = cached_strings_.back().raw();

            const sparta::RegisterBase::group_num_type group_num = reg.at("group_num").as_int64();
            auto iter = group_idx_map_.find(group_num);
            if (iter == group_idx_map_.end())
            {
                group_idx_map_[group_num] = 0;
            }

            sparta::RegisterBase::group_idx_type group_idx = group_idx_map_[group_num]++;
            cached_strings_.emplace_back(std::string(reg.at("group_name").as_string()));
            const char* group = cached_strings_.back().raw();
            if (std::string(group).empty())
            {
                group_idx = sparta::RegisterBase::GROUP_IDX_NONE;
            }

            cached_strings_.emplace_back(std::string(reg.at("desc").as_string()));
            const char* desc = cached_strings_.back().raw();

            const sparta::RegisterBase::size_type bytes = reg.at("size").as_int64();

            std::vector<sparta::RegisterBase::Field::Definition> field_defns;
            if (auto value = reg.as_object().if_contains("fields"); value)
            {
                auto it = value->as_object().begin();
                const auto eit = value->as_object().end();
                while (it != eit)
                {
                    cached_field_defns_.emplace_back(std::string(it->key()), it->value());
                    field_defns.emplace_back(cached_field_defns_.back().getDefn());
                    ++it;
                }
            }

            static const std::vector<sparta::RegisterBase::bank_idx_type> bank_membership;

            std::vector<std::string> alias_strings;
            for (auto & alias : reg.at("aliases").as_array())
            {
                alias_strings.emplace_back(alias.as_string());
            }
            cached_aliases_.emplace_back(alias_strings);
            const char** aliases = cached_aliases_.back().raw();

            constexpr sparta::RegisterBase::ident_type subset_of = sparta::RegisterBase::INVALID_ID;
            constexpr sparta::RegisterBase::size_type subset_offset = 0;

            const unsigned char* initial_value = nullptr;
            if (auto value = reg.as_object().if_contains("initial_value"); value)
            {
                cached_initial_values_.emplace_back(std::string(value->as_string()));
                initial_value = cached_initial_values_.back().raw();
            }
            else
            {
                cached_initial_values_.emplace_back(bytes);
                initial_value = cached_initial_values_.back().raw();
            }

            constexpr sparta::RegisterBase::Definition::HintsT hints = 0;
            constexpr sparta::RegisterBase::Definition::RegDomainT regdomain = 0;

            sparta::RegisterBase::Definition defn = {
                id,        name,          group_num,     group,           group_idx,
                desc,      bytes,         field_defns,   bank_membership, aliases,
                subset_of, subset_offset, initial_value, hints,           regdomain,
                true};

            register_defns_.push_back(defn);
        }
    }
} // namespace pegasus
