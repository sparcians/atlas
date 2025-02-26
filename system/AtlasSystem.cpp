#include "system/AtlasSystem.hpp"
#include "system/MagicMemory.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/memory/MemoryObject.hpp"

namespace atlas
{

    AtlasSystem::AtlasSystem(sparta::TreeNode* sys_node, const AtlasSystemParameters* p) :
        sparta::Unit(sys_node)
    {
        const std::string workload = p->workload;
        loadWorkload_(workload);

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
            sparta::TreeNode::GROUP_IDX_NONE, "Atlas System Memory Map", ATLAS_SYSTEM_BLOCK_SIZE,
            ATLAS_SYSTEM_TOTAL_MEMORY));

        // Create memory objects and add them to the memory map
        createMemoryMappings_(sys_node);

        // Initialize memory with ELF contents
        for (const auto & memory_section : memory_sections_)
        {
            if ((memory_section.data != nullptr) && (memory_section.name != "MAGIC_MEM"))
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

    void AtlasSystem::loadWorkload_(const std::string & workload)
    {
        if (elf_reader_.load(workload) == false)
        {
            // fail
        }

        std::cout << "\nLoading ELF binary: " << workload << std::endl;

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
                ((segment->get_memory_size() - 1) & ~(ATLAS_SYSTEM_BLOCK_SIZE - 1))
                + ATLAS_SYSTEM_BLOCK_SIZE;

            MemorySection section = {(segment_name.empty() ? "?" : segment_name),
                                     segment->get_file_size(), size_aligned,
                                     segment->get_physical_address(),
                                     reinterpret_cast<const uint8_t*>(segment->get_data())};
            memory_sections_.emplace_back(section);

            // TODO: Magic memory support (tohost/fromhost)
        }

        // Get entrypoint
        starting_pc_ = elf_reader_.get_entry();
        std::cout << "Starting PC: 0x" << std::hex << starting_pc_ << std::endl;

        // Get symbols for debugging
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

                // Intentionally overwriting symbols with the same address
                symbols_[addr] = name;
            }
        }
    }

    void AtlasSystem::createMemoryMappings_(sparta::TreeNode* sys_node)
    {
        using BMOIfNode = sparta::memory::BlockingMemoryObjectIFNode;
        using BMIfNode = sparta::memory::BlockingMemoryIFNode;
        using MemObj = sparta::memory::MemoryObject;

        BMIfNode* memory_if = nullptr;
        MemObj* mem_obj = nullptr;

        // The allocated memory blocks (Magic Mem, UART, etc)
        struct AllocatedMemoryBlock
        {
            AllocatedMemoryBlock(sparta::memory::addr_t _start_address,
                                 sparta::memory::addr_t _size) :
                start_address(_start_address),
                size(_size)
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
        /*if(magic_memory_section_.isValid())
        {
            const auto & magic_mem = magic_memory_section_.getValue();

            // Add the magic memory block
            sparta::TreeNode * mm_node = nullptr;
            tree_nodes_.emplace_back(mm_node = new sparta::TreeNode(sys_node, "magic_memory", "Magic
        memory node")); tree_nodes_.emplace_back(memory_if = new MagicMemory(mm_node,
                                                                 magic_mem.start_address,
                                                                 magic_mem.total_size_aligned));
            memory_map_->addMapping(magic_mem.start_address,
                                    magic_mem.start_address + magic_mem.total_size_aligned,
                                    memory_if, 0x0);

            allocated_blocks.emplace(magic_mem.start_address, magic_mem.total_size_aligned);
        }
        */

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
            const auto alloc_block = *(allocated_blocks.begin());

            // Check for a blank space between addr_block_start and the
            // next allocated block.
            if (addr_block_start < alloc_block.start_address)
            {
                // Add a memory block up to the allocated block
                const auto block_size = alloc_block.start_address - addr_block_start;
                memory_objects_.emplace_back(mem_obj =
                                                 new MemObj(sys_node, ATLAS_SYSTEM_BLOCK_SIZE,
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
                                 & ~(ATLAS_SYSTEM_BLOCK_SIZE - 1))
                                + ATLAS_SYSTEM_BLOCK_SIZE);
            allocated_blocks.erase(allocated_blocks.begin());
            ++block_num;
        }

        // Add the rest of memory
        memory_objects_.emplace_back(mem_obj =
                                         new MemObj(sys_node, ATLAS_SYSTEM_BLOCK_SIZE,
                                                    ATLAS_SYSTEM_TOTAL_MEMORY - addr_block_start,
                                                    illop, sizeof(illop)));
        tree_nodes_.emplace_back(
            memory_if =
                new BMOIfNode(sys_node, "mb_" + std::to_string(block_num),
                              sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
                              "mb_" + std::to_string(block_num), nullptr, *mem_obj));
        memory_map_->addMapping(addr_block_start, ATLAS_SYSTEM_TOTAL_MEMORY, memory_if,
                                0x0 /* Additional offset */);
        memory_map_->dumpMappings(std::cout);
    }
} // namespace atlas
