#pragma once

#include <sstream>

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"
#include "sparta/memory/MemoryObject.hpp"

namespace pegasus
{
    class PegasusState;

    /*!
     * \class MagicMemory
     * \brief This class handles commands from applications that write to the MAGIC_MEM address
     * space
     *
     * Data written to address "zero" of this block of memory are
     * interpreted as a command.  The first double word contains that command:
     *
     * - Bits 63:56 indicate the "device"
     *     0 is the syscall device
     *     1 is the block character device (BCD)
     * - Bits 55:48 indicate the "command"
     *
     * - If syscall device:
     *     bit 0 of the command indicates sub-command behavior:
     *          0 value -> bits 47:1 represent an address to struct describing the syscall
     *          1 value -> bits 47:1 represent an exit code: zero is success
     * - If BCD device:
     *     Command 0 reads a character
     *     Command 1 write a character from the 8 LSBs of the data
     *
     *  The fromhost interface packs a 64-bit value from the call of
     *  tohost with a response.
     *
     *     bits<63:48> original tohost command (device | command)
     *     bits<15:0>  response from the call
     */
    class MagicMemory : public sparta::Unit, public sparta::memory::BlockingMemoryIF
    {
      public:
        //! \brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "MagicMemory";

        class MagicMemoryParameters : public sparta::ParameterSet
        {
          public:
            explicit MagicMemoryParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            HIDDEN_PARAMETER(sparta::memory::addr_t, base_addr, 0x0, "Base address")
            HIDDEN_PARAMETER(sparta::memory::addr_t, tohost_addr, 0x0, "tohost address")
            HIDDEN_PARAMETER(sparta::memory::addr_t, fromhost_addr, 0x0, "fromhost address")
            HIDDEN_PARAMETER(sparta::memory::addr_t, size, 0, "Memory size")
        };

        MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params);

        sparta::memory::addr_t getBaseAddr() const { return base_addr_; }

        sparta::memory::addr_t getSize() const { return size_; }

        sparta::memory::addr_t getHighEnd() const { return base_addr_ + size_; }

      private:
        void onBindTreeEarly_() override;
        void onStartingTeardown_() override;

        const sparta::memory::addr_t base_addr_;
        const sparta::memory::addr_t tohost_addr_;
        const sparta::memory::addr_t fromhost_addr_;
        const sparta::memory::addr_t size_;

        // Backend memory
        sparta::memory::MemoryObject memory_;

        PegasusState* state_ = nullptr;

        bool tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size, uint8_t* buf,
                      const void* in_supplement, void* out_supplement) override final;
        bool tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size, const uint8_t* buf,
                       const void* in_supplement, void* out_supplement) override final;
        bool tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                      uint8_t* buf) const override final;
        bool tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                      const uint8_t* buf) override final;

        enum class SupportedDevices
        {
            SYSCALL = 0,   // System Calls
            BLOCK_CHAR = 1 // Block character device or BCD
        };

        union MagicMemCommand
        {
            struct
            {
                uint64_t payload : 48;
                uint64_t command : 8;
                uint64_t device : 8;
            } cmd;

            uint8_t mm_buffer[8] = {0};
            uint64_t tohost_data;
        };

        std::stringstream block_char_msg_;
    };
} // namespace pegasus
