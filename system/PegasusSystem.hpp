#pragma once

#include "elfio/elfio.hpp"

#include "include/PegasusTypes.hpp"
#include "system/SimpleUART.hpp"
#include "system/MagicMemory.hpp"

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/memory/AddressTypes.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"

namespace sparta::memory
{
    class MemoryObject;
    class BlockingMemoryIF;
    class BlockingMemoryObjectIFNode;
    class SimpleMemoryMapNode;
} // namespace sparta::memory

namespace pegasus
{
    class PegasusSystem : public sparta::Unit
    {
      public:
        //! brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "PegasusSystem";

        class PegasusSystemParameters : public sparta::ParameterSet
        {
          public:
            PegasusSystemParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(bool, enable_uart, false, "Enable a Uart")
            HIDDEN_PARAMETER(std::vector<std::string>, workload_and_args, {},
                             "Workload and command line arguments")
        };

        // Constructor
        PegasusSystem(sparta::TreeNode* sys_node, const PegasusSystemParameters* p);

        // Get pointer to system memory
        sparta::memory::SimpleMemoryMapNode* getSystemMemory() { return memory_map_.get(); }

        // Get starting PC from ELF
        Addr getStartingPc() const { return starting_pc_; }

        // Tell the system we're using pass/fail criteria for stopping simulation
        void enableEOTPassFailMode();

        // Get the workload and its program arguments
        const std::vector<std::string> & getWorkloadAndArgs() const { return workload_and_args_; }

        const std::unordered_map<Addr, std::string> & getSymbols() const { return symbols_; }

        constexpr static sparta::memory::addr_t PEGASUS_SYSTEM_BLOCK_SIZE = 0x1000; // 4K
        constexpr static sparta::memory::addr_t PEGASUS_SYSTEM_TOTAL_MEMORY =
            0x8000000000000000; // 4G
        constexpr static uint64_t PEGASUS_MEMORY_FILL = 0x0;

      private:
        // Device factories
        sparta::ResourceFactory<SimpleUART, SimpleUART::SimpleUARTParameters> uart_fact_;
        sparta::ResourceFactory<MagicMemory, MagicMemory::MagicMemoryParameters> magic_mem_fact_;

        // Tree nodes
        std::vector<std::unique_ptr<sparta::TreeNode>> tree_nodes_;

        // Devices
        SimpleUART* uart_ = nullptr;
        MagicMemory* magic_mem_ = nullptr;

        // Memory and memory maps
        std::unique_ptr<sparta::memory::SimpleMemoryMapNode> memory_map_;
        std::vector<std::unique_ptr<sparta::memory::MemoryObject>> memory_objects_;

        struct MemorySection
        {
            std::string name = "?";
            sparta::memory::addr_t file_size = 0;
            sparta::memory::addr_t total_size_aligned = 0;
            sparta::memory::addr_t start_address = 0;
            const uint8_t* data = nullptr;

            MemorySection() = default;

            MemorySection(const std::string name, const sparta::memory::addr_t file_size,
                          const sparta::memory::addr_t total_size_aligned,
                          const sparta::memory::addr_t start_address, const uint8_t* data) :
                name(name),
                file_size(file_size),
                total_size_aligned(total_size_aligned),
                start_address(start_address),
                data(data)
            {
            }
        };

        std::vector<MemorySection> memory_sections_;
        sparta::utils::ValidValue<MemorySection> magic_memory_section_;

        void createMemoryMappings_(sparta::TreeNode* sys_node);

        // Workload and workload arguments
        const std::vector<std::string> workload_and_args_;
        void loadWorkload_(const std::string & workload);
        ELFIO::elfio elf_reader_;
        Addr starting_pc_;
        std::unordered_map<Addr, std::string> symbols_;
        sparta::utils::ValidValue<Addr> tohost_addr_;
        sparta::utils::ValidValue<Addr> fromhost_addr_;
        sparta::utils::ValidValue<Addr> pass_addr_;
        sparta::utils::ValidValue<Addr> fail_addr_;
    };
} // namespace pegasus
