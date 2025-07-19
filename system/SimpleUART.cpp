
#include "system/SimpleUART.hpp"
#include "system/PegasusSystem.hpp"

namespace pegasus
{
    SimpleUART::SimpleUART(sparta::TreeNode* node, const SimpleUARTParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Simple UART", PegasusSystem::PEGASUS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "uart_window"}, nullptr),
        base_addr_(params->base_addr),
        size_(params->size)
    {
    }

    bool SimpleUART::tryRead_(sparta::memory::addr_t, sparta::memory::addr_t, uint8_t* buf,
                              const void*, void*)
    {
        *buf = 0x60;
        return true;
    }

    bool SimpleUART::tryWrite_(sparta::memory::addr_t, sparta::memory::addr_t, const uint8_t* buf,
                               const void*, void*)
    {
        std::cout << *buf;
        return true;
    }

    bool SimpleUART::tryPeek_(sparta::memory::addr_t, sparta::memory::addr_t, uint8_t*) const
    {
        sparta_assert(false, "Simple UART peeks are not implemented");
        return false;
    }

    bool SimpleUART::tryPoke_(sparta::memory::addr_t, sparta::memory::addr_t, const uint8_t*)
    {
        sparta_assert(false, "Simple UART pokes are not implemented");
        return false;
    }
} // namespace pegasus
