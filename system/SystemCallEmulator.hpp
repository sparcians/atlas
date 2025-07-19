#pragma once

#include <array>
#include <cinttypes>

#include "include/PegasusTypes.hpp"

#include "sparta/simulation/Unit.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/memory/BlockingMemoryIF.hpp"
#include "sparta/utils/ValidValue.hpp"

namespace pegasus
{
    class PegasusSim;
    class SysCallHandlers;

    /**
     * \class SystemCallEmulator
     *
     * \brief Utility class to handle system calls from ecall
     * operations or even magic memory
     *
     *
     */
    class SystemCallEmulator : public sparta::Unit
    {
      public:
        //! \brief Name of this resource. Required by sparta::UnitFactory
        static constexpr char name[] = "SystemCallEmulator";

        //! Default write FD, -1 assumes all writes go to the given
        //! system call provided by the call.
        static constexpr int DEFAULT_WRITE_FD = -1;

        //! Parameter to define behaviors of the State class
        class SystemCallEmulatorParameters : public sparta::ParameterSet
        {
          public:
            explicit SystemCallEmulatorParameters(sparta::TreeNode* node) :
                sparta::ParameterSet(node)
            {
            }
            PARAMETER(std::string, write_output, "-",
                      R"desc(For write system calls to stdout/stderr, where should the output go?
   '-'       Means to the intended target (1 for stdout, 2 for stderr, etc)
   '1'       Means ALL output should go to stdout
   '2'       Means ALL output should go to stderr
  <filename> Means ALL output to given filename)desc")
            PARAMETER(std::vector<uint64_t>, mem_map_params,
                      std::vector<uint64_t>({0x10000000, 0x1000000, 0x1000}),
                      "Memory Mapping parameters: <base addr> <total size> <page size>")
        };

        //! Construct!
        SystemCallEmulator(sparta::TreeNode* my_node, const SystemCallEmulatorParameters*);

        //! Destroy!
        ~SystemCallEmulator();

        //! Handle a system call
        int64_t emulateSystemCall(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF* memory);

        //! Handle exit call
        void exitCall(uint64_t exit_code);

        //! Get the default write FD
        int getFDOverrideForWrite(int caller_fd);

        //! Get PegasusSim
        PegasusSim* getPegasusSim() const { return sim_; }

        //! Get the memory map parameters (used by Callback delegate class)
        const std::vector<uint64_t> & getMemMapParams() const { return memory_map_params_; }

        //! Set the workload
        void setWorkload(const std::string & workload);

      private:
        const std::vector<uint64_t> memory_map_params_;

        sparta::log::MessageSource syscall_log_;
        FILE* file_for_write_ = nullptr;
        int fd_for_write_ = DEFAULT_WRITE_FD;
        PegasusSim* sim_ = nullptr;

        std::string workload_;

        std::unique_ptr<SysCallHandlers> callbacks_;
    };
} // namespace pegasus
