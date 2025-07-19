#include "system/MagicMemory.hpp"
#include "system/PegasusSystem.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", PegasusSystem::PEGASUS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "magic_memory"}, nullptr),
        base_addr_(params->base_addr),
        tohost_addr_(params->tohost_addr),
        fromhost_addr_(params->fromhost_addr),
        size_(params->size),
        memory_(node, PegasusSystem::PEGASUS_SYSTEM_BLOCK_SIZE, size_, 0)
    {
    }

    void MagicMemory::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getRoot()->getChildAs<sparta::ResourceTreeNode>("core0");
        state_ = core_tn->getResourceAs<PegasusState>();
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf, const void*, void*)
    {
        DLOG("addr: 0x" << std::hex << (base_addr_ + addr) << " sz: " << std::dec << size);
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                                const uint8_t* buf, const void*, void*)
    {
        DLOG("addr: 0x" << std::hex << (base_addr_ + addr) << " sz: " << std::dec << size);
        if ((base_addr_ + addr) != tohost_addr_)
        {
            memory_.write(addr, size, buf);
            return true;
        }

        MagicMemCommand mm_command;
        ::memcpy(mm_command.mm_buffer, buf, size);

        ILOG("device: " << mm_command.cmd.device << " command: " << mm_command.cmd.command
                        << " payload: " << mm_command.cmd.payload);

        if (mm_command.cmd.payload == 0)
        {
            return true;
        }

        switch (static_cast<SupportedDevices>(mm_command.cmd.device))
        {
            case SupportedDevices::SYSCALL:
                {
                    if (mm_command.cmd.payload & 0x1)
                    {
                        const auto exit_code = mm_command.cmd.payload >> 1;
                        state_->stopSim(exit_code);
                    }
                    else
                    {
                        sparta_assert(false, "Magic memory command not supported: "
                                                 << std::dec << mm_command.cmd.command);
                    }
                }
                break;
            case SupportedDevices::BLOCK_CHAR:
                if (mm_command.cmd.command == 0)
                {
                    sparta_assert(false, "TODO: Block char read is unsupported");
                }
                else
                {
                    const char letter = char(mm_command.tohost_data);
                    block_char_msg_ << letter;
                    if (letter == '\n')
                    {
                        std::cout << "MAGICMEM: " << block_char_msg_.str();
                        block_char_msg_.str("");
                    }
                }
                break;
        }

        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf) const
    {
        DLOG("addr: 0x" << std::hex << (base_addr_ + addr) << " sz: " << std::dec << size);
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               const uint8_t* buf)
    {
        DLOG("addr: 0x" << std::hex << (base_addr_ + addr) << " sz: " << std::dec << size);
        memory_.write(addr, size, buf);
        return true;
    }

    void MagicMemory::onStartingTeardown_()
    {
        const std::string msg = block_char_msg_.str();
        if (false == msg.empty())
        {
            std::cout << "MAGICMEM: " << msg;
        }
    }

} // namespace pegasus
