
#include "system/SimpleUART.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    SimpleUART::SimpleUART(sparta::TreeNode* node, const SimpleUARTParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Simple UART", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "uart_window"}, nullptr),
        base_addr_(params->base_addr),
        size_(params->size)
    {
    }

    bool SimpleUART::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                              uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        *buf = 0x60;
        return true;
    }

    bool SimpleUART::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               const uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        std::cout << *buf;
        return true;
    }

    bool SimpleUART::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                              uint8_t* buf) const
    {
        (void)addr;
        (void)buf;
        sparta_assert(false, "Simple UART peeks are not implemented");
        return false;
    }

    bool SimpleUART::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                              const uint8_t* buf)
    {
        (void)addr;
        (void)buf;
        sparta_assert(false, "Simple UART pokes are not implemented");
        return false;
    }
} // namespace atlas
