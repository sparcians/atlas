#include "system/PegasusSystem.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/memory/MemoryObject.hpp"

namespace pegasus
{

    PegasusSystem::PegasusSystem(sparta::TreeNode* sys_node, const PegasusSystemParameters* p) :
        sparta::Unit(sys_node),
        workload_and_args_(p->workload_and_args)
    {
        if (false == workload_and_args_.empty())
        {
            loadWorkload_(workload_and_args_[0]);
        }

        if (p->enable_uart)
        {
            sparta::ResourceTreeNode* uart_rtn = nullptr;
            tree_nodes_.emplace_back(uart_rtn = new sparta::ResourceTreeNode(
                                         sys_node, "uart", "SimpleUART", &uart_fact_));
            uart_rtn->finalize();
            uart_ = uart_rtn->getResourceAs<SimpleUART>();
        }

        // Initialize memory
        memory_map_.reset(new sparta::memory::SimpleMemoryMapNode(
            sys_node, "memory_map", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Pegasus System Memory Map",
            PEGASUS_SYSTEM_BLOCK_SIZE, PEGASUS_SYSTEM_TOTAL_MEMORY));

        // Create memory objects and add them to the memory map
        createMemoryMappings_(sys_node);

        // Initialize memory with ELF contents
        for (const auto & memory_section : memory_sections_)
        {
            if (memory_section.data != nullptr)
            {
                std::cout << "  -- Loading section " << memory_section.name << " (" << std::dec
                          << memory_section.file_size << "B) "
                          << " to 0x" << std::hex << memory_section.start_address << std::endl;
                bool success = memory_map_->tryPoke(memory_section.start_address,
                                                    memory_section.file_size, memory_section.data);
                if (!success)
                {
                    std::cout << "FAILED!\n";
                }
            }
        }
    }

    void PegasusSystem::loadWorkload_(const std::string & workload)
    {
        if (elf_reader_.load(workload) == false)
        {
            throw sparta::SpartaException()
                << "\nERROR: '" << workload << "' failed to load! Does it exist?\n";
        }

        if (elf_reader_.get_type() == ELFIO::ET_DYN)
        {
            throw sparta::SpartaException()
                << "\nERROR: '" << workload
                << "' is dynamically linked. Pegasus can only run statically linked binaries\n";
        }

        std::cout << "\nLoading ELF binary: " << workload << std::endl;

        for (const auto & section : elf_reader_.sections)
        {
            ELFIO::symbol_section_accessor symbols(elf_reader_, section.get());
            for (uint32_t symbol_id = 0; symbol_id < symbols.get_symbols_num(); ++symbol_id)
            {
                std::string name;
                // ELFIO::Elf64_Addr addr;
                Addr addr;
                ELFIO::Elf_Xword size;
                unsigned char bind;
                unsigned char type;
                ELFIO::Elf_Half section;
                unsigned char other;
                symbols.get_symbol(symbol_id, name, addr, size, bind, type, section, other);

                // Intentionally overwriting symbols with the same address since an ELF may contain
                // more than one label at the same address
                if (name.empty() == false)
                {
                    symbols_[addr] = name;

                    if (name == "tohost")
                    {
                        sparta_assert(tohost_addr_.isValid() == false,
                                      "Found multiple tohost symbols in ELF!");
                        tohost_addr_ = addr;
                    }
                    else if (name == "fromhost")
                    {
                        sparta_assert(fromhost_addr_.isValid() == false,
                                      "Found multiple fromhost symbols in ELF!");
                        fromhost_addr_ = addr;
                    }
                    else if (name == "pass")
                    {
                        sparta_assert(pass_addr_.isValid() == false,
                                      "Found multiple pass symbols in ELF!");
                        pass_addr_ = addr;
                    }
                    else if (name == "fail")
                    {
                        sparta_assert(fail_addr_.isValid() == false,
                                      "Found multiple fail symbols in ELF!");
                        fail_addr_ = addr;
                    }
                }
            }
        }

        // Magic Memory
        if (tohost_addr_.isValid() && fromhost_addr_.isValid())
        {
            std::cout << "Found magic memory symbols in ELF" << std::endl;
            std::cout << "    tohost:   0x" << std::hex << tohost_addr_.getValue() << std::endl;
            std::cout << "    fromhost: 0x" << std::hex << fromhost_addr_.getValue() << std::endl;

            sparta::memory::addr_t base_addr =
                std::min(tohost_addr_.getValue(), fromhost_addr_.getValue());
            if (base_addr % PEGASUS_SYSTEM_BLOCK_SIZE != 0)
            {
                // Align base address of section to the block size
                base_addr = base_addr & ~(PEGASUS_SYSTEM_BLOCK_SIZE - 1);
                std::cout << "Warning: tohost/fromhost address doesn't align with "
                          << PEGASUS_SYSTEM_BLOCK_SIZE << " bytes" << std::endl;
            }
            std::cout << "Automatically constructing magic memory at 0x" << std::hex << base_addr
                      << std::endl;

            // Determine how many blocks are needed to hold the section
            const sparta::memory::addr_t addr_delt = std::max(
                tohost_addr_.getValue() - base_addr, fromhost_addr_.getValue() - base_addr);
            const sparta::memory::addr_t size_aligned =
                ((addr_delt / PEGASUS_SYSTEM_BLOCK_SIZE) + 1) * PEGASUS_SYSTEM_BLOCK_SIZE;

            magic_memory_section_ = MemorySection("MAGIC_MEM", 0, size_aligned, base_addr, nullptr);
        }

        for (const auto & segment : elf_reader_.segments)
        {
            // Ignore empty segments
            if (segment->get_file_size() == 0)
            {
                continue;
            }

            // Bug in ELFIO where segment name is incorrect, so use the section name instead
            std::string segment_name;
            for (const auto & section : elf_reader_.sections)
            {
                if (section->get_address() == segment->get_virtual_address())
                {
                    segment_name = section->get_name();
                    break;
                }
            }

            // Round up segment size to Sparta memory block size
            const sparta::memory::addr_t size_aligned =
                ((segment->get_memory_size() - 1) & ~(PEGASUS_SYSTEM_BLOCK_SIZE - 1))
                + PEGASUS_SYSTEM_BLOCK_SIZE;

            MemorySection section = {(segment_name.empty() ? "?" : segment_name),
                                     segment->get_file_size(), size_aligned,
                                     segment->get_physical_address(),
                                     reinterpret_cast<const uint8_t*>(segment->get_data())};
            memory_sections_.emplace_back(section);
        }

        // Get entrypoint
        starting_pc_ = elf_reader_.get_entry();
        std::cout << "Starting PC: 0x" << std::hex << starting_pc_ << std::endl;
    }

    void PegasusSystem::createMemoryMappings_(sparta::TreeNode* sys_node)
    {
        using BMOIfNode = sparta::memory::BlockingMemoryObjectIFNode;
        using BMIfNode = sparta::memory::BlockingMemoryIFNode;
        using MemObj = sparta::memory::MemoryObject;

        BMIfNode* memory_if = nullptr;
        MemObj* mem_obj = nullptr;

        // The allocated memory blocks (Magic Mem, UART, etc)
        struct AllocatedMemoryBlock
        {
            AllocatedMemoryBlock(sparta::memory::addr_t start_address,
                                 sparta::memory::addr_t size) :
                start_address(start_address),
                size(size)
            {
            }

            sparta::memory::addr_t start_address = 0;
            sparta::memory::addr_t size = 0;

            bool operator<(const AllocatedMemoryBlock & rhs) const
            {
                return start_address < rhs.start_address;
            }
        };

        std::set<AllocatedMemoryBlock> allocated_blocks;

        ////////////////////////////////////////////////////////////////////////////////
        // Magic Memory
        if (magic_memory_section_.isValid())
        {
            const auto & magic_mem = magic_memory_section_.getValue();

            // Add the magic memory block
            sparta::ResourceTreeNode* mm_rtn = nullptr;
            tree_nodes_.emplace_back(mm_rtn = new sparta::ResourceTreeNode(sys_node, "magic_memory",
                                                                           "Magic memory node",
                                                                           &magic_mem_fact_));

            // Set Magic Memory params
            auto base_addr_param =
                getContainer()->getChildAs<sparta::ParameterBase>("magic_memory.params.base_addr");
            auto tohost_addr_param = getContainer()->getChildAs<sparta::ParameterBase>(
                "magic_memory.params.tohost_addr");
            auto fromhost_addr_param = getContainer()->getChildAs<sparta::ParameterBase>(
                "magic_memory.params.fromhost_addr");
            auto size_param =
                getContainer()->getChildAs<sparta::ParameterBase>("magic_memory.params.size");
            base_addr_param->setValueFromString(std::to_string(magic_mem.start_address));
            tohost_addr_param->setValueFromString(std::to_string(tohost_addr_.getValue()));
            fromhost_addr_param->setValueFromString(std::to_string(fromhost_addr_.getValue()));
            size_param->setValueFromString(std::to_string(magic_mem.total_size_aligned));

            mm_rtn->finalize();
            magic_mem_ = mm_rtn->getResourceAs<MagicMemory>();
            memory_map_->addMapping(magic_mem_->getBaseAddr(), magic_mem_->getHighEnd(), magic_mem_,
                                    0x0 /* Additional offset -- not used */);
            allocated_blocks.emplace(magic_mem_->getBaseAddr(), magic_mem_->getSize());
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UART
        if (nullptr != uart_)
        {
            memory_map_->addMapping(uart_->getBaseAddr(), uart_->getHighEnd(), uart_,
                                    0x0 /* Additional offset -- not used */);
            allocated_blocks.emplace(uart_->getBaseAddr(), uart_->getSize());
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Now fill in the memory "blanks"
        sparta::memory::addr_t addr_block_start = 0;
        const uint32_t illop = 0;
        uint32_t block_num = 1;

        while (false == allocated_blocks.empty())
        {
            const auto & alloc_block = *(allocated_blocks.begin());

            // Check for a blank space between addr_block_start and the
            // next allocated block. Sparta will check that mappings
            // do not overlap.
            if (addr_block_start < alloc_block.start_address)
            {
                // Add a memory block up to the allocated block
                const auto block_size = alloc_block.start_address - addr_block_start;
                memory_objects_.emplace_back(mem_obj =
                                                 new MemObj(sys_node, PEGASUS_SYSTEM_BLOCK_SIZE,
                                                            block_size, illop, sizeof(illop)));
                tree_nodes_.emplace_back(memory_if = new BMOIfNode(
                                             sys_node, "mb_" + std::to_string(block_num),
                                             sparta::TreeNode::GROUP_NAME_NONE,
                                             sparta::TreeNode::GROUP_IDX_NONE,
                                             "mb_" + std::to_string(block_num), nullptr, *mem_obj));
                memory_map_->addMapping(addr_block_start, addr_block_start + block_size, memory_if,
                                        0x0 /* Additional offset */);
            }

            // Determine the next large block of memory
            addr_block_start = (((alloc_block.start_address + alloc_block.size - 1)
                                 & ~(PEGASUS_SYSTEM_BLOCK_SIZE - 1))
                                + PEGASUS_SYSTEM_BLOCK_SIZE);
            allocated_blocks.erase(allocated_blocks.begin());
            ++block_num;
        }

        // Add the rest of memory
        memory_objects_.emplace_back(mem_obj =
                                         new MemObj(sys_node, PEGASUS_SYSTEM_BLOCK_SIZE,
                                                    PEGASUS_SYSTEM_TOTAL_MEMORY - addr_block_start,
                                                    illop, sizeof(illop)));
        tree_nodes_.emplace_back(
            memory_if =
                new BMOIfNode(sys_node, "mb_" + std::to_string(block_num),
                              sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
                              "mb_" + std::to_string(block_num), nullptr, *mem_obj));
        memory_map_->addMapping(addr_block_start, PEGASUS_SYSTEM_TOTAL_MEMORY, memory_if,
                                0x0 /* Additional offset */);
        memory_map_->dumpMappings(std::cout);
    }

    void PegasusSystem::enableEOTPassFailMode()
    {
        sparta_assert(pass_addr_.isValid() and fail_addr_.isValid(),
                      "ERROR: ELF binary does not contain pass/fail labels for EOT Pass/Fail");
    }

} // namespace pegasus
