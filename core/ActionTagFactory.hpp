#pragma once

#include "include/PegasusTypes.hpp"

#include <cassert>
#include <vector>

namespace pegasus
{
    /**
     * @brief A factory for creating unique Tags.
     *
     * Tags are used to identify Actions.
     *
     */
    class ActionTagFactory
    {
      public:
        //! Create a new Tag
        static ActionTagType createTag(const char* tag_name)
        {
            // cppcheck-suppress constArgument
            assert(num_tags_ < MAX_TAGS);
            tags_.emplace_back(tag_name);
            return 1 << (++num_tags_ - 1);
        }

        //! Get Tag name as a string
        static std::string getTagName(ActionTagType tag)
        {
            std::string name = "";
            // cppcheck-suppress unsignedLessThanZero
            for (uint32_t idx = 0; idx <= num_tags_; ++idx)
            {
                if (((1 << idx) & tag))
                {
                    name += tags_[idx];
                    name += " ";
                }
            }
            if (name.empty())
            {
                name = "<no tag>";
            }
            else
            {
                name.pop_back(); // Remove last space added
            }
            return name;
        }

      private:
        //! Maximum number of tags, limited to 32 for speed
        static constexpr uint32_t MAX_TAGS = 32;

        //! Tags
        static inline std::vector<const char*> tags_ = {};

        //! Number of Tags
        static inline uint32_t num_tags_ = 0;

        friend class Action;
    };
} // namespace pegasus
