#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/VectorState.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"
#include "system/AtlasSystem.hpp"
#include "core/observers/SimController.hpp"
#include "core/observers/InstructionLogger.hpp"

#include "mavis/mavis/Mavis.h"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace atlas
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

    AtlasState::AtlasState(sparta::TreeNode* core_tn, const AtlasStateParameters* p) :
        sparta::Unit(core_tn),
        hart_id_(p->hart_id),
        isa_string_(p->isa_string),
        xlen_(getXlenFromIsaString_(isa_string_)),
        supported_isa_string_(std::string("rv" + std::to_string(xlen_) + "gcbv_zicsr_zifencei")),
        isa_file_path_(p->isa_file_path),
        uarch_file_path_(p->uarch_file_path),
        csr_values_json_(p->csr_values),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            supported_isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"),
            isa_file_path_)),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        hypervisor_enabled_(extension_manager_.isEnabled("h")),
        vector_config_ptr_(new VectorConfig()),
        inst_logger_(core_tn, "inst", "Atlas Instruction Logger"),
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
        vec_rset_ =
            RegisterSet::create(core_tn, json_dir + std::string("/reg_vec.json"), "vec_regs");
        csr_rset_ =
            RegisterSet::create(core_tn, json_dir + std::string("/reg_csr.json"), "csr_regs");

        for (const auto & kvp : int_rset_->getRegistersByName())
        {
            registers_by_name_[kvp.first] = kvp.second;
        }
        for (const auto & kvp : fp_rset_->getRegistersByName())
        {
            registers_by_name_[kvp.first] = kvp.second;
        }
        for (const auto & kvp : vec_rset_->getRegistersByName())
        {
            registers_by_name_[kvp.first] = kvp.second;
        }
        for (const auto & kvp : csr_rset_->getRegistersByName())
        {
            registers_by_name_[kvp.first] = kvp.second;
        }

        // Increment PC Action
        increment_pc_action_ =
            atlas::Action::createAction<&AtlasState::incrementPc_>(this, "increment pc");

        // Add increment PC Action to finish ActionGroup
        finish_action_group_.addAction(increment_pc_action_);

        // Create Action to stop simulation
        stop_action_ = atlas::Action::createAction<&AtlasState::stopSim_>(this, "stop sim");
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);
    }

    // Not default -- defined in source file to reduce massive inlining
    AtlasState::~AtlasState()
    {
        if (vector_config_ptr_)
        {
            delete vector_config_ptr_;
        }
    }

    void AtlasState::onBindTreeEarly_()
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
                AtlasInst, AtlasExtractor, AtlasInstAllocatorWrapper<AtlasInstAllocator>,
                AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>>(
                getUArchFiles_(), mavis_uid_list_, {}, // annotation overrides
                {},                                    // inclusions
                {},                                    // exclusions
                AtlasInstAllocatorWrapper<AtlasInstAllocator>(
                    sparta::notNull(AtlasAllocators::getAllocators(core_tn))->inst_allocator),
                AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>(
                    sparta::notNull(AtlasAllocators::getAllocators(core_tn))->extractor_allocator,
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

    void AtlasState::onBindTreeLate_()
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

        for (auto & obs : observers_)
        {
            obs->registerReadWriteMemCallbacks(atlas_system_->getSystemMemory());
            for (auto reg : csr_rset_->getRegisters())
            {
                obs->registerReadWriteCsrCallbacks(reg);
            }
        }
    }

    void AtlasState::changeMavisContext()
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

    template <typename XLEN> void AtlasState::changeMMUMode()
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

    mavis::FileNameListType AtlasState::getUArchFiles_() const
    {
        const std::string xlen_str = std::to_string(xlen_);
        const std::string xlen_uarch_file_path = uarch_file_path_ + "/rv" + xlen_str;
        const mavis::FileNameListType uarch_files = {
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "i.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "m.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "a.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "f.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "d.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zba.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zbb.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zbc.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zbs.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zve64x.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zve64d.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zve32x.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zve32f.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zicsr.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zifencei.json"};
        return uarch_files;
    }

    Action::ItrType AtlasState::preExecute_(AtlasState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType AtlasState::postExecute_(AtlasState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->postExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType AtlasState::preException_(AtlasState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preException(state);
        }

        return ++action_it;
    }

    template <typename XLEN> uint32_t AtlasState::getMisaExtFieldValue_() const
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

    void AtlasState::enableInteractiveMode()
    {
        sparta_assert(sim_controller_ == nullptr, "Interactive mode is already enabled");
        auto observer = std::make_unique<SimController>();
        sim_controller_ = observer.get();
        addObserver(std::move(observer));
    }

    void AtlasState::useSpikeFormatting()
    {
        for (auto & obs : observers_)
        {
            if (auto inst_logger = dynamic_cast<InstructionLogger*>(obs.get()))
            {
                inst_logger->useSpikeFormatting();
            }
        }
    }

    sparta::Register* AtlasState::findRegister(const std::string & reg_name, bool must_exist) const
    {
        auto iter = registers_by_name_.find(reg_name);
        auto reg = (iter != registers_by_name_.end()) ? iter->second : nullptr;
        sparta_assert(!must_exist || reg, "Failed to find register: " << reg_name);
        return reg;
    }

    template <typename MemoryType> MemoryType AtlasState::readMemory(const Addr paddr)
    {
        auto* memory = atlas_system_->getSystemMemory();

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
    void AtlasState::writeMemory(const Addr paddr, const MemoryType value)
    {
        auto* memory = atlas_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const bool success = memory->tryWrite(paddr, size, buffer.data());
        sparta_assert(success, "Failed to write to memory at address 0x" << std::hex << paddr);

        ILOG("Memory write (" << std::dec << size << "B) to 0x" << std::hex << paddr << ": 0x"
                              << (uint64_t)value);
    }

    template int8_t AtlasState::readMemory<int8_t>(const Addr);
    template uint8_t AtlasState::readMemory<uint8_t>(const Addr);
    template int16_t AtlasState::readMemory<int16_t>(const Addr);
    template uint16_t AtlasState::readMemory<uint16_t>(const Addr);
    template int32_t AtlasState::readMemory<int32_t>(const Addr);
    template uint32_t AtlasState::readMemory<uint32_t>(const Addr);
    template int64_t AtlasState::readMemory<int64_t>(const Addr);
    template uint64_t AtlasState::readMemory<uint64_t>(const Addr);
    template void AtlasState::writeMemory<uint8_t>(const Addr, const uint8_t);
    template void AtlasState::writeMemory<uint16_t>(const Addr, const uint16_t);
    template void AtlasState::writeMemory<uint32_t>(const Addr, const uint32_t);
    template void AtlasState::writeMemory<uint64_t>(const Addr, const uint64_t);

    void AtlasState::addObserver(std::unique_ptr<Observer> observer)
    {
        if (observers_.empty())
        {
            pre_execute_action_ =
                atlas::Action::createAction<&AtlasState::preExecute_>(this, "pre execute");
            post_execute_action_ =
                atlas::Action::createAction<&AtlasState::postExecute_>(this, "post execute");
            pre_exception_action_ =
                atlas::Action::createAction<&AtlasState::preException_>(this, "pre exception");

            finish_action_group_.addAction(post_execute_action_);
            exception_unit_->getActionGroup()->insertActionBefore(pre_exception_action_,
                                                                  ActionTags::EXCEPTION_TAG);
        }

        observers_.emplace_back(std::move(observer));
    }

    void AtlasState::insertExecuteActions(ActionGroup* action_group)
    {
        if (pre_execute_action_)
        {
            action_group->insertActionBefore(pre_execute_action_, ActionTags::EXECUTE_TAG);
        }
    }

    Action::ItrType AtlasState::incrementPc_(AtlasState*, Action::ItrType action_it)
    {
        // Set PC
        prev_pc_ = pc_;
        pc_ = next_pc_;
        DLOG("PC: 0x" << std::hex << pc_);

        // Increment instruction count
        ++sim_state_.inst_count;

        return ++action_it;
    }

    void AtlasState::boot()
    {
        std::cout << "Booting hartid " << std::dec << hart_id_ << std::endl;
        {
            AtlasState* state = this;

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

            std::cout << "AtlasState::boot()\n";
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

    void AtlasState::cleanup()
    {
        if (sim_controller_)
        {
            sim_controller_->onSimulationFinished(this);
        }
    }

} // namespace atlas
