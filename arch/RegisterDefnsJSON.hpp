#pragma once

#include <vector>
#include <string>
#include <deque>
#include <boost/json.hpp>

#include "sparta/functional/Register.hpp"

namespace pegasus
{

    class RegisterDefnsFromJSON
    {
      public:
        RegisterDefnsFromJSON(const std::vector<std::string> & register_defns_json_filenames)
        {
            for (const auto & filename : register_defns_json_filenames)
            {
                parse_(filename);
            }

            // Add a definition that indicates the end of the array
            register_defns_.push_back(sparta::RegisterBase::DEFINITION_END);
        }

        RegisterDefnsFromJSON(const std::string & register_defns_json_filename)
        {
            parse_(register_defns_json_filename);

            // Add a definition that indicates the end of the array
            register_defns_.push_back(sparta::RegisterBase::DEFINITION_END);
        }

        sparta::RegisterBase::Definition* getAllDefns() { return register_defns_.data(); }

        size_t getNumDefns() const { return register_defns_.size() - 1; }

      private:
        void parse_(const std::string & register_defns_json_filename);

        // Converts a string to a const char* pointer
        class StringRef
        {
          public:
            StringRef(const std::string & str) : storage_(str) {}

            const char* raw() const { return storage_.c_str(); }

          private:
            std::string storage_;
        };

        // Converts a vector of strings to an array of const char* pointers
        class AliasRef
        {
          public:
            AliasRef(const std::vector<std::string> & aliases) : storage_(aliases)
            {
                for (const auto & str : storage_)
                {
                    pointers_.push_back(str.c_str());
                }
                pointers_.push_back(nullptr);
            }

            const char** raw() { return pointers_.data(); }

          private:
            std::vector<std::string> storage_;
            std::vector<const char*> pointers_;
        };

        // Converts any hex ("0xdeafbeef") to a const unsigned char* pointer
        class InitialValueRef
        {
          public:
            InitialValueRef(const std::string & hex_str)
            {
                // Remove the "0x" prefix if present
                std::string hex = hex_str;
                if (hex.substr(0, 2) == "0x")
                {
                    hex = hex.substr(2);
                }

                // Ensure the hex string has an even length
                sparta_assert(hex.length() % 2 == 0, "Hex string must have an even length");

                // Create a vector to hold the bytes
                hex_bytes_.resize(hex.length() / 2);

                // Convert hex string to bytes
                for (size_t i = 0; i < hex.length(); i += 2)
                {
                    const auto byte_string = hex.substr(i, 2);
                    std::istringstream iss(byte_string);
                    int byte;
                    iss >> std::hex >> byte;
                    hex_bytes_[i / 2] = static_cast<char>(byte);
                }
            }

            InitialValueRef(size_t num_bytes)
            {
                hex_bytes_ = std::vector<unsigned char>(num_bytes, 0);
            }

            const unsigned char* raw() const { return hex_bytes_.data(); }

          private:
            std::vector<unsigned char> hex_bytes_;
        };

        // Converts a rapidjson::Value that represents a field to a Field::Definition
        class FieldDefnConverter
        {
          public:
            FieldDefnConverter(const std::string & field_name,
                               const boost::json::value & field_info) :
                field_name_(field_name),
                desc_(field_info.at("desc").as_string()),
                field_defn_(field_name_.c_str(), desc_.c_str(), field_info.at("low_bit").as_int64(),
                            field_info.at("high_bit").as_int64(),
                            field_info.at("readonly").as_bool())
            {
            }

            const sparta::RegisterBase::Field::Definition & getDefn() const { return field_defn_; }

          private:
            std::string field_name_;
            std::string desc_;
            sparta::RegisterBase::Field::Definition field_defn_;
        };

        std::deque<StringRef> cached_strings_;
        std::deque<AliasRef> cached_aliases_;
        std::deque<InitialValueRef> cached_initial_values_;
        std::deque<FieldDefnConverter> cached_field_defns_;
        std::vector<sparta::RegisterBase::Definition> register_defns_;

        // TODO: Find the official way to handle group_idx. For now we will just use
        // a map of auto-incrementing group_idx values for each group_num
        std::map<sparta::RegisterBase::group_num_type, sparta::RegisterBase::group_idx_type>
            group_idx_map_;
    };

} // namespace pegasus
