#pragma once

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"

namespace sparta::memory
{
    class SimpleMemoryMapNode;
} // namespace sparta::memory

namespace atlas
{
    class MagicMemory : public sparta::Unit, public sparta::memory::BlockingMemoryIF
    {
      public:
        //! \brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "MagicMemory";

        class MagicMemoryParameters : public sparta::ParameterSet
        {
          public:
            explicit MagicMemoryParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            HIDDEN_PARAMETER(uint64_t, base_addr, 0x0, "Base address")
            HIDDEN_PARAMETER(uint64_t, size, 0, "Memory size")
        };

        MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params);

        sparta::memory::addr_t getBaseAddr() const { return base_addr_; }

        sparta::memory::addr_t getSize() const { return size_; }

        sparta::memory::addr_t getHighEnd() const { return base_addr_ + size_; }

      private:
        const sparta::memory::addr_t base_addr_;
        const sparta::memory::addr_t size_;

        bool tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size, uint8_t* buf,
                      const void* in_supplement, void* out_supplement) override final;
        bool tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size, const uint8_t* buf,
                       const void* in_supplement, void* out_supplement) override final;
        bool tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                      uint8_t* buf) const override final;
        bool tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                      const uint8_t* buf) override final;
    };
} // namespace atlas
