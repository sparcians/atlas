#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "core/VecElements.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"
#include "system/PegasusSystem.hpp"
#include "core/observers/SimController.hpp"
#include "core/observers/InstructionLogger.hpp"
#include "core/observers/STFLogger.hpp"

#include "mavis/mavis/Mavis.h"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"

#include "system/SystemCallEmulator.hpp"

namespace pegasus
{
    uint32_t getXlenFromIsaString_(const std::string & isa_string)
    {
        if (isa_string.find("32") != std::string::npos)
        {
            return 32;
        }
        else if (isa_string.find("64") != std::string::npos)
        {
            return 64;
        }
        else
        {
            sparta_assert(false, "Failed to determine XLEN from ISA string: " << isa_string);
        }
    }

    PegasusState::PegasusState(sparta::TreeNode* core_tn, const PegasusStateParameters* p) :
        sparta::Unit(core_tn),
        hart_id_(p->hart_id),
        isa_string_(p->isa_string),
        vlen_(p->vlen),
        xlen_(getXlenFromIsaString_(isa_string_)),
        supported_isa_string_(
            std::string("rv" + std::to_string(xlen_) + "gcbv_zicsr_zifencei_zicond_zcb")),
        isa_file_path_(p->isa_file_path),
        uarch_file_path_(p->uarch_file_path),
        csr_values_json_(p->csr_values),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            supported_isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"),
            isa_file_path_)),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        stf_filename_(p->stf_filename),
        hypervisor_enabled_(extension_manager_.isEnabled("h")),
        vector_config_(std::make_unique<VectorConfig>()),
        inst_logger_(core_tn, "inst", "Pegasus Instruction Logger"),
        finish_action_group_("finish_inst"),
        stop_sim_action_group_("stop_sim")
    {
        sparta_assert(false == hypervisor_enabled_, "Hypervisor is not supported yet");
        sparta_assert(xlen_ == extension_manager_.getXLEN());
        extension_manager_.setISA(isa_string_);

        const auto json_dir = (xlen_ == 32) ? REG32_JSON_DIR : REG64_JSON_DIR;
        int_rset_ =
            RegisterSet::create(core_tn, json_dir + std::string("/reg_int.json"), "int_regs");
        fp_rset_ = RegisterSet::create(core_tn, json_dir + std::string("/reg_fp.json"), "fp_regs");
        const std::string vec_reg_json = "/reg_vec" + std::to_string(vlen_) + ".json";
        vec_rset_ = RegisterSet::create(core_tn, json_dir + vec_reg_json, "vec_regs");
        csr_rset_ =
            RegisterSet::create(core_tn, json_dir + std::string("/reg_csr.json"), "csr_regs");

        auto add_registers = [this](const auto & reg_set)
        {
            for (const auto & kvp : reg_set->getRegistersByName())
            {
                registers_by_name_[kvp.first] = kvp.second;
                for (auto & alias_name : kvp.second->getAliases())
                {
                    registers_by_name_[alias_name] = kvp.second;
                }
            }
        };

        add_registers(int_rset_);
        add_registers(fp_rset_);
        add_registers(vec_rset_);
        add_registers(csr_rset_);

        // Increment PC Action
        increment_pc_action_ =
            pegasus::Action::createAction<&PegasusState::incrementPc_>(this, "increment pc");

        // Add increment PC Action to finish ActionGroup
        finish_action_group_.addAction(increment_pc_action_);

        // Create Action to stop simulation
        stop_action_ = pegasus::Action::createAction<&PegasusState::stopSim_>(this, "stop sim");
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);
    }

    // Not default -- defined in source file to reduce massive inlining
    PegasusState::~PegasusState() {}

    void PegasusState::onBindTreeEarly_()
    {
        auto core_tn = getContainer();
        fetch_unit_ = core_tn->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = core_tn->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = core_tn->getChild("translate")->getResourceAs<Translate*>();
        exception_unit_ = core_tn->getChild("exception")->getResourceAs<Exception*>();

        // Initialize Mavis
        DLOG("Initializing Mavis with ISA string " << isa_string_);

        mavis_ = std::make_unique<MavisType>(
            extension_manager_.constructMavis<
                PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>(
                getUArchFiles_(), mavis_uid_list_, {}, // annotation overrides
                {},                                    // inclusions
                {},                                    // exclusions
                PegasusInstAllocatorWrapper<PegasusInstAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(core_tn))->inst_allocator),
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(core_tn))->extractor_allocator,
                    this)));

        // FIXME: Extension manager should maintain inclusions
        for (auto & ext : extension_manager_.getEnabledExtensions())
        {
            inclusions_.emplace(ext.first);
        }
        inclusions_.erase("g");

        // Connect finish ActionGroup to Fetch
        finish_action_group_.setNextActionGroup(fetch_unit_->getActionGroup());
    }

    void PegasusState::onBindTreeLate_()
    {
        // Write initial values to CSR registers
        const boost::json::array json = mavis::parseJSON(csr_values_json_).as_array();
        for (uint32_t idx = 0; idx < json.size(); idx++)
        {
            const boost::json::object & csr_entry = json.at(idx).as_object();
            const auto csr_name_it = csr_entry.find("name");
            sparta_assert(csr_name_it != csr_entry.end());
            const auto csr_value_it = csr_entry.find("value");
            sparta_assert(csr_value_it != csr_entry.end());

            const std::string csr_name = boost::json::value_to<std::string>(csr_name_it->value());
            sparta::Register* csr_reg = findRegister(csr_name);
            if (csr_reg)
            {
                sparta_assert(csr_reg->getGroupNum()
                                  == (sparta::RegisterBase::group_num_type)RegType::CSR,
                              "Provided initial value for not-CSR register: " << csr_name);
                const std::string csr_hex_str =
                    boost::json::value_to<std::string>(csr_value_it->value());
                const uint64_t csr_val = std::stoull(csr_hex_str, nullptr, 16);
                std::cout << csr_name << ": " << HEX16(csr_val) << std::endl;
                csr_reg->dmiWrite(csr_val);
            }
            else
            {
                std::cout
                    << "WARNING: Provided initial value for CSR register that does not exist! "
                    << csr_name << std::endl;
            }
        }

        // Set up translation
        if (xlen_ == 64)
        {
            changeMMUMode<RV64>();
        }
        else
        {
            changeMMUMode<RV32>();
        }

        // FIXME: Does Sparta have a callback notif for when debug icount is reached?
        if (inst_logger_.observed())
        {
            if (xlen_ == 64)
            {
                addObserver(std::make_unique<InstructionLogger>(inst_logger_, ObserverMode::RV64));
            }
            else
            {
                addObserver(std::make_unique<InstructionLogger>(inst_logger_, ObserverMode::RV32));
            }
        }

        if (!stf_filename_.empty())
        {
            addObserver(std::make_unique<STFLogger>(xlen_, pc_, stf_filename_, this));
        }

        for (auto & obs : observers_)
        {
            obs->registerReadWriteMemCallbacks(pegasus_system_->getSystemMemory());
            for (auto reg : csr_rset_->getRegisters())
            {
                obs->registerReadWriteCsrCallbacks(reg);
            }
        }
    }

    void PegasusState::changeMavisContext()
    {
        const mavis::MatchSet<mavis::Pattern> inclusions{inclusions_};
        const std::string context_name =
            std::accumulate(inclusions_.begin(), inclusions_.end(), std::string(""));
        if (mavis_->hasContext(context_name) == false)
        {
            DLOG("Creating new Mavis context: " << context_name);
            mavis_->makeContext(context_name, extension_manager_.getJSONs(), getUArchFiles_(),
                                mavis_uid_list_, {}, inclusions, {});
        }
        DLOG("Changing Mavis context: " << context_name);
        mavis_->switchContext(context_name);

        const bool compression_enabled = inclusions_.contains("c");
        if (compression_enabled)
        {
            setPcAlignment_(2);
        }
        else
        {
            setPcAlignment_(4);
        }
    }

    template <typename XLEN> void PegasusState::changeMMUMode()
    {
        static const std::vector<MMUMode> satp_mmu_mode_map = {
            MMUMode::BAREMETAL, // mode == 0
            MMUMode::SV32,      // mode == 1 xlen==32
            MMUMode::INVALID,   // mode == 2 - 7 -> reserved
            MMUMode::INVALID,   MMUMode::INVALID, MMUMode::INVALID, MMUMode::INVALID,
            MMUMode::INVALID, // mode ==  7
            MMUMode::SV39,    // mode ==  8, xlen==64
            MMUMode::SV48,    // mode ==  9, xlen==64
            MMUMode::SV57     // mode == 10, xlen==64
        };

        const uint32_t satp_val = READ_CSR_FIELD<XLEN>(this, SATP, "mode");
        sparta_assert(satp_val < satp_mmu_mode_map.size());
        const MMUMode mode = satp_mmu_mode_map[satp_val];

        const uint32_t mprv_val = READ_CSR_FIELD<XLEN>(this, MSTATUS, "mprv");
        const PrivMode prev_priv_mode = (PrivMode)READ_CSR_FIELD<XLEN>(this, MSTATUS, "mpp");
        ldst_priv_mode_ = (mprv_val == 1) ? prev_priv_mode : priv_mode_;
        const MMUMode ls_mode = (ldst_priv_mode_ == PrivMode::MACHINE) ? MMUMode::BAREMETAL : mode;

        DLOG_CODE_BLOCK(DLOG_OUTPUT("MMU Mode: " << mode);
                        DLOG_OUTPUT("MMU LS Mode: " << ls_mode););
        translate_unit_->changeMMUMode<XLEN>(mode, ls_mode);
    }

    mavis::FileNameListType PegasusState::getUArchFiles_() const
    {
        const std::string xlen_str = std::to_string(xlen_);
        const std::string xlen_uarch_file_path = uarch_file_path_ + "/rv" + xlen_str;
        const mavis::FileNameListType uarch_files = {
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "i.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "m.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "a.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "f.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "d.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zba.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zbb.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zbc.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zbs.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zve64x.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zve64d.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zve32x.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zve32f.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zicsr.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zifencei.json",
            xlen_uarch_file_path + "/pegasus_uarch_rv" + xlen_str + "zicond.json"};
        return uarch_files;
    }

    int64_t PegasusState::emulateSystemCall(const SystemCallStack & call_stack)
    {
        return system_call_emulator_->emulateSystemCall(call_stack,
                                                        pegasus_system_->getSystemMemory());
    }

    Action::ItrType PegasusState::preExecute_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType PegasusState::postExecute_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->postExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType PegasusState::preException_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preException(state);
        }

        return ++action_it;
    }

    template <typename XLEN> uint32_t PegasusState::getMisaExtFieldValue_() const
    {
        uint32_t ext_val = 0;
        for (char ext = 'a'; ext <= 'z'; ++ext)
        {
            const std::string ext_str = std::string(1, ext);
            if (inclusions_.contains(ext_str))
            {
                ext_val |= 1 << getCsrBitRange<XLEN>(MISA, ext_str.c_str()).first;
            }
        }

        // FIXME: Assume both User and Supervisor mode are supported
        ext_val |= 1 << CSR::MISA::u::high_bit;
        ext_val |= 1 << CSR::MISA::s::high_bit;

        return ext_val;
    }

    void PegasusState::enableInteractiveMode()
    {
        sparta_assert(sim_controller_ == nullptr, "Interactive mode is already enabled");
        auto observer = std::make_unique<SimController>();
        sim_controller_ = observer.get();
        addObserver(std::move(observer));
    }

    void PegasusState::useSpikeFormatting()
    {
        for (auto & obs : observers_)
        {
            if (auto inst_logger = dynamic_cast<InstructionLogger*>(obs.get()))
            {
                inst_logger->useSpikeFormatting();
            }
        }
    }

    sparta::Register* PegasusState::findRegister(const std::string & reg_name,
                                                 bool must_exist) const
    {
        auto iter = registers_by_name_.find(reg_name);
        auto reg = (iter != registers_by_name_.end()) ? iter->second : nullptr;
        sparta_assert(!must_exist || reg, "Failed to find register: " << reg_name);
        return reg;
    }

    template <typename MemoryType> MemoryType PegasusState::readMemory(const Addr paddr)
    {
        auto* memory = pegasus_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        std::vector<uint8_t> buffer(sizeof(MemoryType) / sizeof(uint8_t), 0);
        const bool success = memory->tryRead(paddr, size, buffer.data());
        sparta_assert(success, "Failed to read from memory at address 0x" << std::hex << paddr);

        const MemoryType value = convertFromByteVector<MemoryType>(buffer);
        ILOG("Memory read (" << std::dec << size << "B) to 0x" << std::hex << paddr << ": 0x"
                             << (uint64_t)value);
        return value;
    }

    template <typename MemoryType>
    void PegasusState::writeMemory(const Addr paddr, const MemoryType value)
    {
        auto* memory = pegasus_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const bool success = memory->tryWrite(paddr, size, buffer.data());
        sparta_assert(success, "Failed to write to memory at address 0x" << std::hex << paddr);

        ILOG("Memory write (" << std::dec << size << "B) to 0x" << std::hex << paddr << ": 0x"
                              << (uint64_t)value);
    }

    template int8_t PegasusState::readMemory<int8_t>(const Addr);
    template uint8_t PegasusState::readMemory<uint8_t>(const Addr);
    template int16_t PegasusState::readMemory<int16_t>(const Addr);
    template uint16_t PegasusState::readMemory<uint16_t>(const Addr);
    template int32_t PegasusState::readMemory<int32_t>(const Addr);
    template uint32_t PegasusState::readMemory<uint32_t>(const Addr);
    template int64_t PegasusState::readMemory<int64_t>(const Addr);
    template uint64_t PegasusState::readMemory<uint64_t>(const Addr);
    template void PegasusState::writeMemory<uint8_t>(const Addr, const uint8_t);
    template void PegasusState::writeMemory<uint16_t>(const Addr, const uint16_t);
    template void PegasusState::writeMemory<uint32_t>(const Addr, const uint32_t);
    template void PegasusState::writeMemory<uint64_t>(const Addr, const uint64_t);

    void PegasusState::addObserver(std::unique_ptr<Observer> observer)
    {
        if (observers_.empty())
        {
            pre_execute_action_ =
                pegasus::Action::createAction<&PegasusState::preExecute_>(this, "pre execute");
            post_execute_action_ =
                pegasus::Action::createAction<&PegasusState::postExecute_>(this, "post execute");
            pre_exception_action_ =
                pegasus::Action::createAction<&PegasusState::preException_>(this, "pre exception");

            finish_action_group_.addAction(post_execute_action_);
            exception_unit_->getActionGroup()->insertActionBefore(pre_exception_action_,
                                                                  ActionTags::EXCEPTION_TAG);
        }

        observers_.emplace_back(std::move(observer));
    }

    void PegasusState::insertExecuteActions(ActionGroup* action_group)
    {
        if (pre_execute_action_)
        {
            action_group->insertActionBefore(pre_execute_action_, ActionTags::EXECUTE_TAG);
        }
    }

    Action::ItrType PegasusState::incrementPc_(PegasusState*, Action::ItrType action_it)
    {
        // Set PC
        prev_pc_ = pc_;
        pc_ = next_pc_;
        DLOG("PC: 0x" << std::hex << pc_);

        // Increment instruction count
        ++sim_state_.inst_count;

        return ++action_it;
    }

    // Initialze a program stack (argc, argv, envp, auxv, etc)
    // Useful info about ELF binaries: https://lwn.net/Articles/631631/
    // This is used mostly for system call emulation
    void PegasusState::setupProgramStack(const std::vector<std::string> & program_arguments)
    {
        if (false == getExecuteUnit()->getSystemCallEmulation())
        {
            // System call emulation is not enabled.  There's a good
            // chance we might be running a bare metal binary so no
            // need to set up prog arguments.  In any event, we better
            // not receive any either.
            sparta_assert(
                program_arguments.size() == 1,
                "System Call emulation is not enabled, but the program is given arguments: "
                    << program_arguments);
            return;
        }

        //
        // Taken from this awesome article:
        //
        // http://articles.manugarg.com/aboutelfauxiliaryvectors.html
        //
        // position          content                       size (bytes) + comment
        //------------------------------------------------------------------------
        // stack pointer ->  [ argc = number of args ]     4
        //                   [ argv[0] (pointer) ]         4   (program name)
        //                   [ argv[1] (pointer) ]         4
        //                   [ argv[..] (pointer) ]        4 * x
        //                   [ argv[n - 1] (pointer) ]     4
        //                   [ argv[n] (pointer) ]         4   (= NULL)
        //                   [ envp[0] (pointer) ]         4
        //                   [ envp[1] (pointer) ]         4
        //                   [ envp[..] (pointer) ]        4
        //                   [ envp[term] (pointer) ]      4   (= NULL)
        //                   [ auxv[0] (Elf32_auxv_t) ]    8
        //                   [ auxv[1] (Elf32_auxv_t) ]    8
        //                   [ auxv[..] (Elf32_auxv_t) ]   8
        //                   [ auxv[term] (Elf32_auxv_t) ] 8   (= AT_NULL vector)
        //                   [ padding ]                   0 - 16
        //                   [ argument ASCIIZ strings ]   >= 0
        //                   [ environment ASCIIZ str. ]   >= 0
        // (0xbffffffc)      [ end marker ]                4   (= NULL)
        // (0xc0000000)      < bottom of stack >           0   (virtual)
        //
        // The argument stack contains env, followed by command line
        // arguments, auxv data, ELF aux vector info, environment,
        // argv, and then argc.  The SP starts at argc and grows
        // towards upward.

        // Get the SP (aka x2)
        const bool MUST_EXIST = true;
        sparta::Register* reg = findRegister("sp", MUST_EXIST);
        uint64_t sp = 0;

        // Typicsl stack pointer is 8KB on most linux systems
        const uint64_t typical_ulimit_stack_size = 8192;
        if (xlen_ == 64)
        {
            sp = reg->dmiRead<RV64>();
            sparta_assert(std::numeric_limits<uint64_t>::max() - sp > typical_ulimit_stack_size,
                          "Stack pointer initial value has a good chance of overflowing");
        }
        else
        {
            sp = reg->dmiRead<RV32>();
            sparta_assert(std::numeric_limits<uint32_t>::max() - (uint32_t)sp
                              > typical_ulimit_stack_size,
                          "Stack pointer initial value has a good chance of overflowing");
        }
        sparta_assert(sp != 0, "The stack pointer (sp aka x2) is set to 0.  Use --reg \"sp <val>\" "
                               "to set it to something...smarter");

        auto* memory = sparta::notNull(pegasus_system_)->getSystemMemory();
        sparta_assert(memory != nullptr, "Got no memory to preload with the argument stack");

        ////////////////////////////////////////////////////////////////////////////////
        // Set up argc
        uint64_t data = program_arguments.size();
        memory->poke(sp, 8, (uint8_t*)&data);
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up argv pointers space -- where the arguments for the
        // program will be found in memory
        auto argv_sp_addr = sp; // Remember this address for later
        sp += 8 * program_arguments.size();
        data = 0;
        // Write (null) or end of argv arguments
        memory->poke(sp, 8, (uint8_t*)&data);
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up envp -- empty for now
        std::vector<std::string> env_vars;
        auto envp_sp_addr = sp;
        sp += 8 * env_vars.size();
        data = 0;
        memory->poke(sp, 8, (uint8_t*)&data); // Write (nil)
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up auxv
        // Magic...
        // https://github.com/pytorch/cpuinfo/blob/6c9eb84ba310f237cea13c478be50102e1128e9b/src/riscv/linux/riscv-isa.c
        const auto AT_HWCAP = 16;
        const auto COMPAT_HWCAP_ISA_I(1 << ('I' - 'A'));
        const auto COMPAT_HWCAP_ISA_M(1 << ('M' - 'A'));
        const auto COMPAT_HWCAP_ISA_A(1 << ('A' - 'A'));
        const auto COMPAT_HWCAP_ISA_F(1 << ('F' - 'A'));
        const auto COMPAT_HWCAP_ISA_D(1 << ('D' - 'A'));
        const auto COMPAT_HWCAP_ISA_C(1 << ('C' - 'A'));
        const auto COMPAT_HWCAP_ISA_V(1 << ('V' - 'A'));

        uint64_t a_val = COMPAT_HWCAP_ISA_I | COMPAT_HWCAP_ISA_M | COMPAT_HWCAP_ISA_A;
        if (extension_manager_.isEnabled("f"))
        {
            a_val |= COMPAT_HWCAP_ISA_F;
        }
        if (extension_manager_.isEnabled("d"))
        {
            a_val |= COMPAT_HWCAP_ISA_D;
        }
        if (extension_manager_.isEnabled("c"))
        {
            a_val |= COMPAT_HWCAP_ISA_C;
        }
        if (extension_manager_.isEnabled("v"))
        {
            a_val |= COMPAT_HWCAP_ISA_V;
        }
        uint64_t auxv[2] = {AT_HWCAP, a_val};
        memory->poke(sp, 16, (uint8_t*)&auxv);
        sp += 16;

        // padding
        memory->poke(sp, 8, (uint8_t*)&data); // Write (null)
        sp += 8;
        memory->poke(sp, 8, (uint8_t*)&data); // Write (null)
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // argv string addresses -- need to pad to 16B boundary
        sp &= ~0xf;
        for (const auto & arg : program_arguments)
        {
            // Add 1 to include the null character
            const auto str_len = arg.size() + 1;
            ILOG("Pushing argv: " << arg);
            memory->poke(sp, str_len, (uint8_t*)arg.data());
            memory->poke(argv_sp_addr, 8, (uint8_t*)&sp);
            sp += str_len;
            argv_sp_addr += 8;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // envp string addresses -- just like argv
        sp += 16;
        sp &= ~0xf;
        for (const auto & envp : env_vars)
        {
            const auto str_len = envp.size() + 1;
            memory->poke(sp, str_len, (uint8_t*)envp.data());
            memory->poke(envp_sp_addr, 8, (uint8_t*)&sp);
            sp += str_len;
            envp_sp_addr += 8;
        }
    }

    void PegasusState::boot()
    {
        std::cout << "Booting hartid " << std::dec << hart_id_ << std::endl;
        {
            PegasusState* state = this;

            if (xlen_ == 64)
            {
                POKE_CSR_REG<RV64>(this, MHARTID, hart_id_);

                const uint64_t xlen_val = 2;
                POKE_CSR_FIELD<RV64>(this, MISA, "mxl", xlen_val);

                const uint32_t ext_val = getMisaExtFieldValue_<RV64>();
                POKE_CSR_FIELD<RV64>(this, MISA, "extensions", ext_val);

                // Initialize MSTATUS/STATUS with User and Supervisor mode XLEN
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "uxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "sxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, SSTATUS, "uxl", xlen_val);
            }
            else
            {
                POKE_CSR_REG<RV32>(this, MHARTID, hart_id_);

                const uint32_t xlen_val = 1;
                POKE_CSR_FIELD<RV32>(this, MISA, "mxl", xlen_val);

                const uint32_t ext_val = getMisaExtFieldValue_<RV32>();
                POKE_CSR_FIELD<RV32>(this, MISA, "extensions", ext_val);
            }

            std::cout << "PegasusState::boot()\n";
            std::cout << std::hex;
            std::cout << "\tMHARTID: 0x" << state->getCsrRegister(MHARTID)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tMISA:    0x" << state->getCsrRegister(MISA)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tMSTATUS: 0x" << state->getCsrRegister(MSTATUS)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tSSTATUS: 0x" << state->getCsrRegister(SSTATUS)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << std::dec;
        }

        if (sim_controller_)
        {
            sim_controller_->postInit(this);
        }
    }

    void PegasusState::cleanup()
    {
        if (sim_controller_)
        {
            sim_controller_->onSimulationFinished(this);
        }
    }

} // namespace pegasus
