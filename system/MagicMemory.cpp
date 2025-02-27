
#include "system/MagicMemory.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "magic_memory"}, nullptr),
        base_addr_(params->base_addr),
        size_(params->size)
    {
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        // TODO: Implement magic memory reads
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                                const uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        // TODO: Implement magic memory writes
        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf) const
    {
        (void)addr;
        (void)buf;
        // Peeks are allowed, but have no side effects
        return true;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               const uint8_t* buf)
    {
        (void)addr;
        (void)buf;
        // Pokes are allowed, but have no side effects
        return true;
    }
} // namespace atlas
