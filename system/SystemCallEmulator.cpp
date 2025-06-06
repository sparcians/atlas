
#include <vector>
#include <unordered_map>

#include "system/SystemCallEmulator.hpp"
#include "sim/AtlasSim.hpp"
#include "sparta/utils/LogUtils.hpp"

#include <unistd.h>      // for write, etc
#include <fcntl.h>       // for openat, open
#include <sys/uio.h>     // for writev
#include <sys/utsname.h> // for uname
#include <string.h>
#include <sys/time.h> // get time of day
#include <sys/mman.h> // mmap
#include <sys/types.h>
#include <sys/stat.h> //fstat, etc

#define SYSCALL_LOG(x)                                                                             \
    if (SPARTA_EXPECT_FALSE(syscall_log_))                                                         \
    {                                                                                              \
        syscall_log_ << x;                                                                         \
    }

namespace atlas
{
    class SysCallHandlers
    {
      public:
        SysCallHandlers(SystemCallEmulator* emulator, sparta::log::MessageSource & sys_log) :
            emulator_(emulator),
            memory_map_manager_(emulator_->getMemMapParams()),
            syscall_log_(sys_log)
        {

            // Create function pointer
            auto cfp = [this]<typename FuncT>(FuncT && funct)
            { return std::bind(funct, this, std::placeholders::_1, std::placeholders::_2); };

            // RISCV64/32 SystemCall Numbers
            // https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html
            supported_sys_calls_.insert(
                {{17, {"getcwd", cfp(&SysCallHandlers::getcwd_)}},
                 {23, {"setuid", cfp(&SysCallHandlers::setuid_)}},
                 {25, {"stime", cfp(&SysCallHandlers::stime_)}},
                 {29, {"ioctl", cfp(&SysCallHandlers::ioctl_)}},
                 {48, {"faccessat", cfp(&SysCallHandlers::faccessat_)}},
                 {57, {"close", cfp(&SysCallHandlers::close_)}},
                 {62, {"lseek", cfp(&SysCallHandlers::lseek_)}},
                 {63, {"read", cfp(&SysCallHandlers::read_)}},
                 {64, {"write", cfp(&SysCallHandlers::write_)}},
                 {66, {"writev", cfp(&SysCallHandlers::writev_)}},
                 {67, {"pread", cfp(&SysCallHandlers::pread_)}},
                 {68, {"pwrite", cfp(&SysCallHandlers::pwrite_)}},
                 {78, {"readlinkat", cfp(&SysCallHandlers::readlinkat_)}},
                 {79, {"fstatat", cfp(&SysCallHandlers::fstatat_)}},
                 {80, {"fstat", cfp(&SysCallHandlers::fstat_)}},
                 {93, {"exit", cfp(&SysCallHandlers::exit_)}},
                 {96, {"set_tid_address", cfp(&SysCallHandlers::setTIDAddress_)}},
                 {98, {"futex", cfp(&SysCallHandlers::futex_)}},
                 {99, {"set_robust_list", cfp(&SysCallHandlers::setRobustList_)}},
                 {131, {"tgkill", cfp(&SysCallHandlers::tgkill_)}},
                 {135, {"rt_sigprocmask", cfp(&SysCallHandlers::rtSIGProcMask_)}},
                 {160, {"uname", cfp(&SysCallHandlers::uname_)}},
                 {172, {"getpid", cfp(&SysCallHandlers::getpid_)}},
                 {174, {"getuid", cfp(&SysCallHandlers::getuid_)}},
                 {175, {"geteuid", cfp(&SysCallHandlers::geteuid_)}},
                 {176, {"getgid", cfp(&SysCallHandlers::getgid_)}},
                 {177, {"gettid", cfp(&SysCallHandlers::gettid_)}},
                 {178, {"getegid", cfp(&SysCallHandlers::getegid_)}},
                 {214, {"brk", cfp(&SysCallHandlers::brk_)}},
                 {222, {"mmap", cfp(&SysCallHandlers::mmap_)}},
                 {226, {"mprotect", cfp(&SysCallHandlers::mprotect_)}},
                 {261, {"prlimit", cfp(&SysCallHandlers::prlimit_)}},
                 {278, {"getrandom", cfp(&SysCallHandlers::getrandom_)}},
                 {291, {"statx", cfp(&SysCallHandlers::statx_)}},
                 {1024, {"open", cfp(&SysCallHandlers::open_)}},
                 {1039, {"lstat", cfp(&SysCallHandlers::lstat_)}},
                 {2011, {"getmainvars", cfp(&SysCallHandlers::getmainvars_)}}});
        }

        int64_t emulateSystemCall(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF* memory)
        {
            int64_t ret_val = -1;
            const auto sc_call_id = call_stack[0];
            try
            {
                const auto & syscall = supported_sys_calls_.at(sc_call_id);
                ret_val = syscall.handler(call_stack, memory);
            }
            catch (const std::out_of_range &)
            {
                sparta_assert(false, "System call #" << sc_call_id << " is not known");
            }
            return ret_val;
        }

        void setWorkload(const std::string & workload) { workload_ = workload; }

        void setBreakAddress(Addr addr) { brk_address_ = addr; }

        Addr getBreakAddress() const { return brk_address_; }

      private:
        // The system calls
        int64_t getcwd_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t setuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t ioctl_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t faccessat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t openat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t close_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t lseek_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t read_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t write_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t writev_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t pread_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t pwrite_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t readlinkat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t fstatat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t fstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t exit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t setTIDAddress_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t futex_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t setRobustList_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t tgkill_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t rtSIGProcMask_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getpid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t uname_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t geteuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getgid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t gettid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getegid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t brk_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t mmap_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t mprotect_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t prlimit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getrandom_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t statx_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t open_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);
        int64_t getmainvars_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*);

        // The parent emulator
        SystemCallEmulator* emulator_ = nullptr;

        // Callbacks
        using HandlerFunc =
            std::function<int64_t(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)>;

        struct SystemCall
        {
            const std::string name;
            const HandlerFunc handler;
        };

        std::unordered_map<uint64_t, SystemCall> supported_sys_calls_;

        // Memory management for things like mmap, etc
        class MemoryManager
        {
          public:
            MemoryManager(const std::vector<uint64_t> & mmap_params) :
                next_block_addr_(mmap_params.at(0)),
                base_addr_(mmap_params.at(0)),
                total_size_(mmap_params.at(1)),
                page_size_(mmap_params.at(2))
            {
            }

            // Return the guest address
            uint64_t allocate(uint64_t host_addr, uint64_t total_size)
            {
                // Aligned the requesting total_size with page_size_
                total_size = ((total_size - 1) & ~(page_size_ - 1)) + page_size_;
                sparta_assert(((next_block_addr_ + total_size) < (base_addr_ + total_size_)),
                              "Requested memory block larger than what this MemoryManager expects:"
                              "\tAsked total_size: "
                                  << total_size << " expected: " << total_size_);
                sparta_assert(guest_to_host_mapping_.find(next_block_addr_)
                                  == guest_to_host_mapping_.end(),
                              "Double allocations for address: " << HEX16(next_block_addr_));
                guest_to_host_mapping_.insert(std::make_pair(next_block_addr_, host_addr));
                const auto ret_addr = next_block_addr_;
                next_block_addr_ += total_size; // Move to the next block
                return ret_addr;
            }

            // Return the host address
            uint64_t deallocate(uint64_t guest_addr, uint64_t)
            {
                if (guest_to_host_mapping_.find(guest_addr) != guest_to_host_mapping_.end())
                {
                    const auto ret_addr = guest_to_host_mapping_[guest_addr];
                    guest_to_host_mapping_.erase(guest_addr);
                    return ret_addr;
                }
                // Nothing to unmap, return success
                return 0;
            }

          private:
            uint64_t next_block_addr_ = 0;
            const uint64_t base_addr_;
            const uint64_t total_size_;
            const uint16_t page_size_;

            // The mapping from guest address to host address
            std::unordered_map<uint64_t, uint64_t> guest_to_host_mapping_;
        } memory_map_manager_;

        // Logging
        sparta::log::MessageSource & syscall_log_;

        // The workload
        std::string workload_;

        // For a program, if the `brk` system call is made, the
        // program is asking to extend the data segment
        Addr brk_address_ =  0;
    };

    SystemCallEmulator::SystemCallEmulator(
        sparta::TreeNode* my_node, const SystemCallEmulator::SystemCallEmulatorParameters* p) :
        sparta::Unit(my_node),
        memory_map_params_(p->mem_map_params),
        syscall_log_(my_node, "syscall", "System Call Logger")
    {
        sim_ = dynamic_cast<AtlasSim*>(my_node->getRoot()->getSimulation());

        if (p->write_output == "-")
        {
            fd_for_write_ = SystemCallEmulator::DEFAULT_WRITE_FD;
        }
        else if (p->write_output == "1")
        {
            fd_for_write_ = ::dup(1);
        }
        else if (p->write_output == "2")
        {
            fd_for_write_ = ::dup(2);
        }
        else
        {
            file_for_write_ = ::fopen(p->write_output.getValue().c_str(), "w");
            sparta_assert(file_for_write_ != nullptr, "Error: could not open '"
                                                          << p->write_output
                                                          << "' for capturing write system calls");
            fd_for_write_ = ::fileno(file_for_write_);
        }
        callbacks_ = std::make_unique<SysCallHandlers>(this, syscall_log_);
    }

    SystemCallEmulator::~SystemCallEmulator()
    {
        if (nullptr != file_for_write_)
        {
            ::fclose(file_for_write_);
        }
    }

    int64_t SystemCallEmulator::emulateSystemCall(const SystemCallStack & call_stack,
                                                  sparta::memory::BlockingMemoryIF* memory)
    {
        return callbacks_->emulateSystemCall(call_stack, memory);
    }

    int SystemCallEmulator::getFDOverrideForWrite(int caller_fd)
    {
        int fd = caller_fd;
        // User may want to redirect all writes somewhere else...
        if (SPARTA_EXPECT_FALSE(fd_for_write_ != SystemCallEmulator::DEFAULT_WRITE_FD))
        {
            fd = fd_for_write_;
        }
        return fd;
    }

    void SystemCallEmulator::setWorkload(const std::string & workload)
    {
        workload_ = workload;
        callbacks_->setWorkload(workload);

        const auto & symbols = sim_->getAtlasSystem()->getSymbols();
        for (auto symbol : symbols) {
            if (symbol.second == "_end") {
                callbacks_->setBreakAddress(symbol.first);
                break;
            }
        }
        sparta_assert(callbacks_->getBreakAddress() != 0,
                      "Could not find _end symbol in workload for system call emulation");
    }

    ////////////////////////////////////////////////////////////////////////////////
    // The system calls
    int64_t SysCallHandlers::getcwd_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        const auto pbuf = call_stack[1];
        const auto count = call_stack[2];
        std::vector<uint8_t> final_buf(count);
        int64_t ret = (int64_t)getcwd((char*)final_buf.data(), call_stack[2]);
        if (ret != 0)
        {
            size_t slen = strnlen((char*)final_buf.data(), call_stack[2]);
            mem->poke(pbuf, slen, final_buf.data());
        }
        if (ret)
        {
            ret = pbuf;
        }
        return ret;
    }

    int64_t SysCallHandlers::setuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        return 0;
    }

    int64_t SysCallHandlers::getuid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        return ::getuid();
    }

    int64_t SysCallHandlers::geteuid_(const SystemCallStack & call_stack,
                                      sparta::memory::BlockingMemoryIF*memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::getgid_(const SystemCallStack &call_stack,
                                     sparta::memory::BlockingMemoryIF*memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::gettid_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::getegid_(const SystemCallStack & call_stack,
                                      sparta::memory::BlockingMemoryIF*memory)
    {
        return getuid_(call_stack, memory);
    }

    int64_t SysCallHandlers::brk_(const SystemCallStack & call_stack,
                                  sparta::memory::BlockingMemoryIF*)
    {
        if (call_stack[1] != 0) {
            // Set new address
            brk_address_ = call_stack[1];
        }
        // else request for address

        const int64_t ret = brk_address_;

        SYSCALL_LOG(__func__ << "(" << HEX16(ret) << ") -> " << ret);
        return ret;
    }

    int64_t SysCallHandlers::mmap_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF*)
    {
        const auto addr = call_stack[1];
        const auto size = call_stack[2];
        const auto prot = call_stack[3];
        const auto flags = call_stack[4];
        const auto fd = call_stack[5];
        const auto offset = call_stack[6];

        if (addr != 0)
        {
            std::cerr << "ATHENA: WARNING: mmap system call with non-zero starting addr. "
                      << "Not well supported" << std::endl;
        }

        uint64_t guest_addr = 0;

        if ((flags & MAP_ANONYMOUS) == 0)
        {
            sparta_assert(fd != std::numeric_limits<uint64_t>::max(),
                          "Memory map to file -- not supported");
        }

        guest_addr = memory_map_manager_.allocate(0ull, size);

        SYSCALL_LOG("mmap(" << HEX16(addr) << ", " << HEX16(size) << ", " << HEX16(prot) << ", "
                            << HEX16(flags) << ", " << HEX16(fd) << ", " << HEX16(offset) << ", "
                            << ") -> " << guest_addr);

        return guest_addr;
    }

    int64_t SysCallHandlers::mprotect_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::prlimit_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::getrandom_(const SystemCallStack & call_stack,
                                        sparta::memory::BlockingMemoryIF* mem)
    {
        auto buffer = call_stack[1];
        auto buflen = call_stack[2];

        // Use cycles to provide a deterministic answer
        do
        {
            uint64_t cycle = emulator_->getClock()->currentCycle();
            ;
            cycle = cycle * 6364136223846793005 + 1442695040888963407; // knuth random seed

            mem->poke(buffer, std::min(buflen, sizeof(cycle)), (uint8_t*)&cycle);

            if (buflen <= sizeof(cycle))
                break;

            buffer += sizeof(cycle);
            buflen -= sizeof(cycle);
        } while (true);

        return buflen;
    }

    int64_t SysCallHandlers::stime_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::ioctl_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::faccessat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::close_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::lseek_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF*)
    {
        const auto fd = call_stack[1];
        const auto offset = call_stack[2];
        const auto whence = call_stack[3];

        int64_t ret = ::lseek(fd, offset, whence);

        SYSCALL_LOG("lseek(" << HEX16(fd) << ", " << HEX16(offset) << ", " << HEX16(whence) << ", "
                             << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::read_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF* mem)
    {
        const auto fd = call_stack[1];
        const auto buf = call_stack[2];
        const auto count = call_stack[3];

        std::vector<uint8_t> final_buf(count);
        int64_t ret = ::read(fd, (char*)final_buf.data(), count);

        mem->poke(buf, count, final_buf.data());

        SYSCALL_LOG("read(" << HEX16(fd) << ", " << HEX16(buf) << ", " << HEX16(count) << ", "
                            << ") -> " << ret);

        return ret;
    }

    int64_t SysCallHandlers::write_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* mem)
    {
        int fd = emulator_->getFDOverrideForWrite(call_stack[1]);
        const auto string_addr = call_stack[2];
        const auto string_len = call_stack[3];

        // Check that the file descriptor is valid
        int64_t ret = fcntl(fd, F_GETFD);
        if (ret != -1)
        {
            std::vector<uint8_t> string_to_write(string_len);
            if (SPARTA_EXPECT_FALSE(mem->doesAccessSpan(string_addr, string_len)))
            {
                const auto mem_block_size = mem->getBlockSize();

                // Need to break up the read into parts.
                const uint32_t first_half_length =
                    mem_block_size - (string_addr & (mem_block_size - 1));
                mem->peek(string_addr, first_half_length, &string_to_write[0]);
                mem->peek(string_addr + first_half_length, string_len - first_half_length,
                          &string_to_write[0] + first_half_length);
            }
            else
            {
                mem->peek(string_addr, string_len, &string_to_write[0]);
            }

            // send the string to the file descriptor
            ret = ::write(fd, &string_to_write[0], string_len);
        }
        SYSCALL_LOG("write(" << fd << ", " << HEX16(string_addr) << ", " << string_len << ") -> "
                             << ret);

        return ret;
    }

    int64_t SysCallHandlers::writev_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        auto fd = emulator_->getFDOverrideForWrite(call_stack[1]);
        const auto iov_addr = call_stack[2];
        const auto iov_cnt = call_stack[3];

        // Grab the iovec structure from mem
        iovec* iov = (iovec*)alloca(sizeof(iovec) * iov_cnt);
        mem->peek(iov_addr, sizeof(iovec) * iov_cnt, (uint8_t*)(iov));

        int64_t ret = 0;
        for (uint32_t i = 0; i < iov_cnt; ++i)
        {
            const uint64_t str_addr = (uint64_t)iov[i].iov_base;
            const auto str_len = iov[i].iov_len;
            std::vector<uint8_t> char_str(str_len);
            if (SPARTA_EXPECT_FALSE(mem->doesAccessSpan(str_addr, str_len)))
            {
                const auto mem_block_size = mem->getBlockSize();

                // Need to break up the read into parts.
                // Math: 0x400 - (0x3fe & 0x3ff) = 2 -- will fetch 2 bytes
                const uint32_t len_first_half = mem_block_size - (str_addr & (mem_block_size - 1));

                mem->peek(str_addr, len_first_half, &char_str[0]);

                mem->peek(str_addr + len_first_half, str_len - len_first_half,
                          &char_str[0] + len_first_half);
            }
            else
            {
                mem->peek(str_addr, str_len, &char_str[0]);
            }

            ret += ::write(fd, &char_str[0], str_len);
        }
        SYSCALL_LOG("writev(" << fd << ", " << HEX16(iov_addr) << ", " << iov_cnt << ") -> "
                              << ret);

        return ret;
    }

    int64_t SysCallHandlers::pread_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::pwrite_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::readlinkat_(const SystemCallStack & call_stack,
                                         sparta::memory::BlockingMemoryIF* mem)
    {
        int64_t ret = -1;
        const auto dirfd = call_stack[1];
        const auto pathname = call_stack[2];
        const auto buf = call_stack[3];
        const auto bufsize = call_stack[4];

        std::string pathname_in_memory;
        uint8_t one_char = 0;
        uint32_t next_byte = 0;
        do
        {
            mem->peek(pathname + next_byte, 1, &one_char);
            if (one_char != 0)
            {
                pathname_in_memory += one_char;
            }
            else
            {
                // end of string
                break;
            }
            ++next_byte;
        } while (true);

        std::vector<uint8_t> final_resolved_path(bufsize, '\0');
        auto final_resolved_path_ptr = (char*)final_resolved_path.data();

        ret = 0;
        if ("/proc/self/exe" == pathname_in_memory)
        {
            auto was_resolved = ::realpath(workload_.c_str(), final_resolved_path_ptr);
            if (was_resolved != final_resolved_path_ptr)
            {
                ret = -errno;
            }
            else
            {
                ret = strlen(final_resolved_path_ptr);
            }
        }
        else
        {
            ret = ::readlinkat(dirfd, pathname_in_memory.c_str(), final_resolved_path_ptr, bufsize);
        }
        mem->poke(buf, bufsize, (uint8_t*)final_resolved_path_ptr);

        SYSCALL_LOG("readlinkat(" << HEX16(dirfd) << ", " << HEX16(pathname) << "(\""
                                  << pathname_in_memory << "\"), " << HEX16(buf) << "(\""
                                  << final_resolved_path_ptr << "\"), " << bufsize << ") -> "
                                  << ret);
        if (ret == -1)
        {
            SYSCALL_LOG("\treadlinkat reports FAILED: errno: " << errno << " "
                                                               << ::strerror(errno));
            ret = -errno;
        }

        return ret;
    }

    // Pulled from /usr/riscv64-linux-gnu/include/sys/stat.h and
    // frontend.h from pk
    struct RV_stat
    {
        uint64_t st_dev;
        uint64_t st_ino;
        uint32_t st_mode;
        uint32_t st_nlink;
        uint32_t st_uid;
        uint32_t st_gid;
        uint64_t st_rdev;
        uint64_t __pad1;
        uint64_t st_size;
        uint32_t st_blksize;
        uint32_t __pad2;
        uint64_t st_blocks;
        struct timespec st_atim;
        struct timespec st_mtim;
        struct timespec st_ctim;
        int __glibc_reserved[2];

        RV_stat(struct stat host_stat) :
            st_dev(host_stat.st_dev),
            st_ino(host_stat.st_ino),
            st_mode(host_stat.st_mode),
            st_nlink(host_stat.st_nlink),
            st_uid(host_stat.st_uid),
            st_gid(host_stat.st_gid),
            st_rdev(host_stat.st_rdev),
            st_size(host_stat.st_size),
            st_blksize(host_stat.st_blksize),
            st_blocks(host_stat.st_blocks),
            st_atim(host_stat.st_atim),
            st_mtim(host_stat.st_mtim),
            st_ctim(host_stat.st_ctim)
        {}
    };

    int64_t SysCallHandlers::fstatat_(const SystemCallStack &call_stack,
                                      sparta::memory::BlockingMemoryIF*memory)
    {
        int64_t ret = 0;

        const auto dirfd    = call_stack[1];
        const auto pathname = call_stack[2];
        const auto statbuf  = call_stack[3];
        const auto flags    = call_stack[4];

        std::string pathname_str;
        uint8_t c;
        uint32_t offset = 0;
        do {
            memory->peek(pathname + offset, 1, &c);
            if(c != 0) {
                pathname_str += c;
            }
            ++offset;
        } while (c != 0);

        // Use the host's stat
        struct stat host_stat;
        ret = ::fstatat(dirfd, pathname_str.c_str(), &host_stat, flags);

        SYSCALL_LOG(__func__ << "("
                    << HEX16(dirfd) << ", "
                    << HEX16(pathname) << ", "
                    << HEX16(statbuf) << ", "
                    << HEX16(flags) << ", "
                    << ") -> " << ret);

        // Now create a RV stat and populate
        RV_stat rv_stat(host_stat);

        memory->poke(statbuf, sizeof(struct RV_stat), reinterpret_cast<uint8_t*>(&rv_stat));

        return ret;
    }

    int64_t SysCallHandlers::fstat_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF*memory)
    {
        int64_t ret = 0;

        const auto fd       = call_stack[1];
        const auto statbuf  = call_stack[3];


        // Use the host's stat
        struct stat host_stat;
        ret = ::fstat(fd, &host_stat);

        // Now create a RV stat and populate
        RV_stat rv_stat(host_stat);
        SYSCALL_LOG(__func__ << "("
                    << HEX16(fd) << ", "
                    << HEX16(statbuf) << ", "
                    << ") -> " << ret);

        memory->poke(statbuf, sizeof(struct RV_stat), reinterpret_cast<uint8_t*>(&rv_stat));
        return ret;
    }

    int64_t SysCallHandlers::setTIDAddress_(const SystemCallStack &,
                                            sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::futex_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::setRobustList_(const SystemCallStack &,
                                            sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::tgkill_(const SystemCallStack & call_stack,
                                     sparta::memory::BlockingMemoryIF* mem)
    {
        return exit_(call_stack, mem);
    }

    int64_t SysCallHandlers::rtSIGProcMask_(const SystemCallStack &,
                                            sparta::memory::BlockingMemoryIF*)
    {
        SYSCALL_LOG(__func__ << "(...) -> 0 # ignored");
        return 0;
    }

    int64_t SysCallHandlers::getpid_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        auto ret = ::getpid();
        return ret;
    }

    int64_t SysCallHandlers::uname_(const SystemCallStack & call_stack,
                                    sparta::memory::BlockingMemoryIF* memory)
    {
        const auto uname_data_ptr = call_stack[1];
        const auto UTS_CHAR_LENGTH = 64 + 1; // standard

        struct uname_info
        {
            const char sysname[UTS_CHAR_LENGTH] = "Atlas Core Emulator";
            const char nodename[UTS_CHAR_LENGTH] = "";
            const char release[UTS_CHAR_LENGTH] = "4.15.0";
            const char version[UTS_CHAR_LENGTH] = "";
            const char machine[UTS_CHAR_LENGTH] = "";
            const char domainname[UTS_CHAR_LENGTH] = ""; // Domainname (if exists)
        } atlas_uname_info;

        memory->poke(uname_data_ptr, sizeof(uname_info),
                     reinterpret_cast<uint8_t*>(&atlas_uname_info));
        SYSCALL_LOG("uname(" << HEX16(uname_data_ptr) << ") -> 0");
        return 0;
    }

    int64_t SysCallHandlers::exit_(const SystemCallStack & call_stack,
                                   sparta::memory::BlockingMemoryIF*)
    {
        const int64_t exit_code = call_stack[1];
        SYSCALL_LOG("exit(" << exit_code << ");");
        emulator_->getAtlasSim()->endSimulation(exit_code);
        return exit_code;
    }

    int64_t SysCallHandlers::statx_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::open_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::lstat_(const SystemCallStack &, sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }

    int64_t SysCallHandlers::getmainvars_(const SystemCallStack &,
                                          sparta::memory::BlockingMemoryIF*)
    {
        sparta_assert(false, __func__ << " returning -1, i.e. not implemented");
        int64_t ret = -1;
        return ret;
    }
} // namespace atlas
