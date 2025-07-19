#pragma once

#include <iostream>

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"

namespace pegasus
{
    class SimpleUART : public sparta::Unit, public sparta::memory::BlockingMemoryIF
    {
      public:
        //! \brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "SimpleUART";

        class SimpleUARTParameters : public sparta::ParameterSet
        {
          public:
            explicit SimpleUARTParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(sparta::memory::addr_t, base_addr, 0x20000000, "Base address")
            PARAMETER(sparta::memory::addr_t, size, 4096, "Memory size")
        };

        SimpleUART(sparta::TreeNode* node, const SimpleUARTParameters* params);

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
} // namespace pegasus
