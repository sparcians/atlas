
#include "ActionGroup.hpp"

namespace atlas
{
    const char* ActionException::what() const noexcept { return action_group_->getName().c_str(); }

    // Not default -- defined in source file to reduce massive inlining
    ActionGroup::~ActionGroup() {}
} // namespace atlas
