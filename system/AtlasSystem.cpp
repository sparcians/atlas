#include "system/AtlasSystem.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/memory/MemoryObject.hpp"

namespace atlas
{

    AtlasSystem::AtlasSystem(sparta::TreeNode* sys_node, const AtlasSystemParameters* p) :
        sparta::Unit(sys_node)
    {
        const std::string workload = p->workload;
        loadWorkload_(workload);

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
            if (memory_section.data != nullptr)
            {
                std::cout << "  -- Loading section " << memory_section.name << " (" << std::dec
                          << memory_section.file_size << "B) " << " to 0x" << std::hex
                          << memory_section.start_address << std::endl;
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
            ELFIO::symbol_section_accessor symbols(elf_reader_, section);
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
        // Just use one memory object for all of memory for now
        memory_objects_.emplace_back(new sparta::memory::MemoryObject(
            sys_node, ATLAS_SYSTEM_BLOCK_SIZE, ATLAS_SYSTEM_TOTAL_MEMORY));

        // Wrap the new MemoryObject with the BlockingMemoryIF API
        sparta::memory::MemoryObject & memory_object = *(memory_objects_.back().get());
        memory_object_nodes_.emplace_back(new sparta::memory::BlockingMemoryObjectIFNode(
            sys_node, "mem", "memory", nullptr, memory_object));

        // Add a mapping to the MemoryMap for this MemoryObject
        sparta::memory::BlockingMemoryObjectIFNode* memory_object_node_ptr =
            memory_object_nodes_.back().get();
        memory_map_->addMapping(0x0, ATLAS_SYSTEM_TOTAL_MEMORY, memory_object_node_ptr, 0);
    }
} // namespace atlas
