#pragma once

#include <elfio/elfio.hpp>

#include "include/AtlasTypes.hpp"

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/AddressTypes.hpp"

namespace sparta::memory
{
    class MemoryObject;
    class BlockingMemoryIF;
    class BlockingMemoryObjectIFNode;
    class SimpleMemoryMapNode;
} // namespace sparta::memory

namespace atlas
{
    class AtlasSystem : public sparta::Unit
    {
      public:
        //! brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "AtlasSystem";

        class AtlasSystemParameters : public sparta::ParameterSet
        {
          public:
            AtlasSystemParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            HIDDEN_PARAMETER(std::string, workload, "", "Workload to load into memory")
        };

        // Constructor
        AtlasSystem(sparta::TreeNode* sys_node, const AtlasSystemParameters* p);

        // Get pointer to system memory
        sparta::memory::SimpleMemoryMapNode* getSystemMemory() { return memory_map_.get(); }

        // Get starting PC from ELF
        Addr getStartingPc() const { return starting_pc_; }

        //
        void initializeMemory(const std::string & name, sparta::memory::BlockingMemoryIF* memory);

        const std::unordered_map<Addr, std::string> & getSymbols() const { return symbols_; }

        constexpr static sparta::memory::addr_t ATLAS_SYSTEM_BLOCK_SIZE = 0x1000; // 4K
        constexpr static sparta::memory::addr_t ATLAS_SYSTEM_TOTAL_MEMORY =
            0x8000000000000000; // 4G

      private:
        // Factories and tree nodes
        std::vector<std::unique_ptr<sparta::TreeNode>> tree_nodes_;

        // Memory and memory maps
        std::unique_ptr<sparta::memory::SimpleMemoryMapNode> memory_map_;
        std::vector<std::unique_ptr<sparta::memory::MemoryObject>> memory_objects_;
        std::vector<std::unique_ptr<sparta::memory::BlockingMemoryObjectIFNode>>
            memory_object_nodes_;

        struct MemorySection
        {
            std::string name = "?";
            sparta::memory::addr_t file_size = 0;
            sparta::memory::addr_t total_size_aligned = 0;
            sparta::memory::addr_t start_address = 0;
            const uint8_t* data = nullptr;
        };

        std::vector<MemorySection> memory_sections_;

        void createMemoryMappings_(sparta::TreeNode* sys_node);

        // Workload
        void loadWorkload_(const std::string & workload);
        ELFIO::elfio elf_reader_;
        Addr starting_pc_;
        std::unordered_map<Addr, std::string> symbols_;
    };
} // namespace atlas
