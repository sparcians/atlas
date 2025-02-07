#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"
#include "arch/register_macros.hpp"
#include "system/AtlasSystem.hpp"
#include "core/Snapshotters.hpp"
#include "core/observers/SimController.hpp"

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

    mavis::FileNameListType getUArchFiles(const std::string & uarch_file_path, const uint64_t xlen)
    {
        const std::string xlen_str = std::to_string(xlen);
        const std::string xlen_uarch_file_path = uarch_file_path + "/rv" + xlen_str;
        const mavis::FileNameListType uarch_files = {
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "i.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "m.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "a.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "f.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "d.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zicsr.json",
            xlen_uarch_file_path + "/atlas_uarch_rv" + xlen_str + "zifencei.json"};
        return uarch_files;
    }

    AtlasState::AtlasState(sparta::TreeNode* core_tn, const AtlasStateParameters* p) :
        sparta::Unit(core_tn),
        hart_id_(p->hart_id),
        isa_string_(p->isa_string),
        xlen_(getXlenFromIsaString_(isa_string_)),
        supported_isa_string_(std::string("rv" + std::to_string(xlen_) + "g_zicsr_zifencei")),
        isa_file_path_(p->isa_file_path),
        uarch_file_path_(p->uarch_file_path),
        csr_values_json_(p->csr_values),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            supported_isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"),
            isa_file_path_)),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        inst_logger_(core_tn, "inst", "Atlas Instruction Logger"),
        finish_action_group_("finish_inst"),
        stop_sim_action_group_("stop_sim")
    {
        sparta_assert(xlen_ == extension_manager_.getXLEN());

        auto json_dir = (xlen_ == 32) ? REG32_JSON_DIR : REG64_JSON_DIR;
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
    AtlasState::~AtlasState() {}

    void AtlasState::onBindTreeEarly_()
    {
        auto core_tn = getContainer();
        fetch_unit_ = core_tn->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = core_tn->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = core_tn->getChild("translate")->getResourceAs<Translate*>();
        exception_unit_ = core_tn->getChild("exception")->getResourceAs<Exception*>();

        // Initialize Mavis
        mavis_ = std::make_unique<MavisType>(
            extension_manager_.constructMavis<
                AtlasInst, AtlasExtractor, AtlasInstAllocatorWrapper<AtlasInstAllocator>,
                AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>>(
                getUArchFiles(uarch_file_path_, xlen_),
                AtlasInstAllocatorWrapper<AtlasInstAllocator>(
                    sparta::notNull(AtlasAllocators::getAllocators(core_tn))->inst_allocator),
                AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>(
                    sparta::notNull(AtlasAllocators::getAllocators(core_tn))->extractor_allocator,
                    this)));

        extension_manager_.setISA(isa_string_);
        std::vector<std::string> enabled_extensions;
        for (auto & ext : extension_manager_.getEnabledExtensions())
        {
            enabled_extensions.emplace_back(ext.first);
        }

        const mavis::MatchSet<mavis::Pattern> inclusions{enabled_extensions};
        mavis_->makeContext("boot", extension_manager_.getJSONs(),
                            getUArchFiles(uarch_file_path_, xlen_), {}, {}, inclusions, {});
        mavis_->switchContext("boot");

        // Connect finish ActionGroup to Fetch
        finish_action_group_.setNextActionGroup(fetch_unit_->getActionGroup());

        // FIXME: Does Sparta have a callback notif for when debug icount is reached?
        if (inst_logger_.observed())
        {
            addObserver(std::make_unique<InstructionLogger>(inst_logger_));
        }
    }

    void AtlasState::onBindTreeLate_()
    {
        // Write initial values to CSR registers
        std::ifstream ifs(csr_values_json_);
        nlohmann::json csr_values_json = nlohmann::json::parse(ifs);
        for (const auto & [csr_name, hex_str] : csr_values_json.items())
        {
            sparta::Register* csr_reg = findRegister(csr_name);
            if (csr_reg)
            {
                sparta_assert(csr_reg->getGroupNum()
                                  == (sparta::RegisterBase::group_num_type)RegType::CSR,
                              "Provided initial value for not-CSR register: " << csr_name);
                const uint64_t csr_val = std::stoull(std::string(hex_str), nullptr, 16);
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
    }

    ActionGroup* AtlasState::preExecute_(AtlasState* state)
    {
        // TODO cnyce: Package up all rs1/rs2/rd registers, pc, opcode, etc.
        // and change the observers' preExecute() to take both AtlasState
        // and ObserverContainer as arguments.
        //
        // class ObserverContainer {
        // public:
        //     void preExecute(AtlasState* state) {
        //         for (const auto & observer : observers_) {
        //             observer->preExecute(state, this);
        //         }
        //     }
        //
        //     uint64_t getPc() const;
        //     uint64_t getOpcode() const;
        //
        //     bool hasRs1() const;
        //     const Observer::SrcReg & getRs1() const;
        //
        //     bool hasRs2() const;
        //     const Observer::SrcReg & getRs2() const;
        //
        //     bool hasRs3() const;
        //     const Observer::SrcReg & getRs3() const;
        //
        //     bool hasRd() const;
        //     const Observer::DestReg & getRd() const;
        //
        //     bool hasImmediate() const;
        //     uint64_t getImmediate() const;
        //
        //     TrapCause getTrapCause() const;
        //     std::string getMnemonic() const;
        //     std::string getDasmString() const;
        //
        //     enum class InstResult { PASS, INVALID_PC, INVALID_REG_VAL, UNIMPLEMENTED };
        //     InstResult getResult() const;
        // };
        //
        // AtlasState.hpp:
        //     std::unique_ptr<ObserverContainer> observer_container_;

        ActionGroup* fail_action_group = nullptr;
        for (const auto & observer : observers_)
        {
            fail_action_group = observer->preExecute(state);
            if (SPARTA_EXPECT_FALSE(fail_action_group))
            {
                return fail_action_group;
            }
        }

        return nullptr;
    }

    ActionGroup* AtlasState::postExecute_(AtlasState* state)
    {
        // TODO cnyce: See comments in preExecute_()
        ActionGroup* fail_action_group = nullptr;
        for (const auto & observer : observers_)
        {
            fail_action_group = observer->postExecute(state);
            if (SPARTA_EXPECT_FALSE(fail_action_group))
            {
                return fail_action_group;
            }
        }

        return nullptr;
    }

    ActionGroup* AtlasState::preException_(AtlasState* state)
    {
        // TODO cnyce: See comments in preExecute_()
        ActionGroup* fail_action_group = nullptr;
        for (const auto & observer : observers_)
        {
            fail_action_group = observer->preException(state);
            if (SPARTA_EXPECT_FALSE(fail_action_group))
            {
                return fail_action_group;
            }
        }

        return nullptr;
    }

    // Check all PC/reg/csr values against our cosim comparator,
    // and return the result code as follows:
    //
    //   success            0x00
    //   exception          0x1x (x encodes the exception cause)
    //   pc mismatch        0x2- (- means ignored)
    //   reg val mismatch   0x3-
    //   unimplemented inst 0x4-
    //
    // At the end of this method, all PC/reg/csr values will be
    // synced with the other simulation ("truth").
    int AtlasState::compareWithCoSimAndSync_()
    {
        sparta::utils::ValidValue<int> rc;
        const auto & insn = getCurrentInst();
        if (insn->unimplemented())
        {
            // status 0x4 means "unimplemented but passing reg val checks"
            rc = 0x4 << 16;
        }

        const auto & exc = exception_unit_->getUnhandledException();
        if (!rc.isValid() && exc.isValid())
        {
            rc = (1 << 16) | static_cast<int>(exc.getValue());
        }

        auto atlas_pc = getPc();
        auto cosim_pc = cosim_query_->getExpectedPC(getHartId());
        if (atlas_pc != cosim_pc)
        {
            if (!rc.isValid())
            {
                rc = 0x2 << 16;
            }
            else if (insn->unimplemented())
            {
                // status 0x5 means "unimplemented and fails reg val checks"
                rc = 0x5 << 16;
            }
            // Force overwrite to keep the cosim going.
            pc_ = cosim_pc;
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumIntRegisters(); ++reg_idx)
        {
            auto atlas_reg = int_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getIntRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val)
            {
                if (!rc.isValid())
                {
                    rc = 0x3 << 16;
                }
                else if (insn->unimplemented())
                {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumFpRegisters(); ++reg_idx)
        {
            auto atlas_reg = fp_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getFpRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val)
            {
                if (!rc.isValid())
                {
                    rc = 0x3 << 16;
                }
                else if (insn->unimplemented())
                {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < cosim_query_->getNumVecRegisters(); ++reg_idx)
        {
            auto atlas_reg = vec_rset_->getRegister(reg_idx);
            auto atlas_val = atlas_reg->dmiRead<uint64_t>();
            auto cosim_val = cosim_query_->getVecRegValue(getHartId(), reg_idx);
            if (atlas_val != cosim_val)
            {
                if (!rc.isValid())
                {
                    rc = 0x3 << 16;
                }
                else if (insn->unimplemented())
                {
                    // status 0x5 means "unimplemented and fails reg val checks"
                    rc = 0x5 << 16;
                }
                // Force overwrite to keep the cosim going.
                atlas_reg->dmiWrite(cosim_val);
            }
        }

        for (uint32_t reg_idx = 0; reg_idx < csr_rset_->getNumRegisters(); ++reg_idx)
        {
            if (auto atlas_reg = csr_rset_->getRegister(reg_idx))
            {
                if (cosim_query_->isCsrImplemented(atlas_reg->getName()))
                {
                    auto atlas_val = atlas_reg->dmiRead<uint64_t>();
                    auto cosim_val =
                        cosim_query_->getCsrRegValue(getHartId(), atlas_reg->getName());

                    if (atlas_val != cosim_val)
                    {
                        // We should only count the CSR mismatches during exceptions
                        if (!rc.isValid() && exc.isValid())
                        {
                            rc = 0x3 << 16;
                        }
                        else if (insn->unimplemented())
                        {
                            // status 0x5 means "unimplemented and fails reg val checks"
                            rc = 0x5 << 16;
                        }
                        // Force overwrite to keep the cosim going.
                        atlas_reg->dmiWrite(cosim_val);
                    }
                }
            }
        }

        if (rc.isValid())
        {
            return rc;
        }
        return 0;
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
        ILOG("Memory read to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
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

        ILOG("Memory write to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
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

    ActionGroup* AtlasState::incrementPc_(AtlasState*)
    {
        // Set PC
        pc_ = next_pc_;
        DLOG("PC: 0x" << std::hex << pc_);

        // Set Privilege Mode
        priv_mode_ = next_priv_mode_;
        DLOG("Privilege Mode: " << (uint32_t)priv_mode_);

        // Increment instruction count
        ++sim_state_.inst_count;

        return nullptr;
    }

    void AtlasState::enableCoSimDebugger(std::shared_ptr<simdb::ObjectManager> db,
                                         std::shared_ptr<CoSimQuery> query,
                                         const std::vector<RegisterInfo> & reg_info)
    {
        cosim_db_ = db;
        cosim_query_ = query;

        auto tbl = cosim_db_->getTable("Registers");

        for (const auto & tup : reg_info)
        {
            const auto & reg_name = std::get<0>(tup);
            const auto group_num = std::get<1>(tup);
            const auto reg_id = std::get<2>(tup);
            const auto expected_val = std::get<3>(tup);
            const auto actual_val = std::get<4>(tup);

            tbl->createObjectWithArgs("HartId", (int)getHartId(), "RegName", reg_name, "RegType",
                                      (int)group_num, "RegIdx", (int)reg_id, "ExpectedInitVal",
                                      expected_val, "ActualInitVal", actual_val);

            switch (group_num)
            {
                case 0:
                    getIntRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 1:
                    getFpRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 2:
                    getVecRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                case 3:
                    getCsrRegisterSet()->getRegister(reg_id)->dmiWrite(expected_val);
                    break;
                default:
                    throw sparta::SpartaException("Invalid group num!");
            }
        }
    }

    void AtlasState::snapshotAndSyncWithCoSim()
    {
        if (SPARTA_EXPECT_TRUE(!cosim_db_))
        {
            return;
        }

        const AtlasInstPtr & insn = getCurrentInst();

        const std::string rs1_name = insn->hasRs1() ? insn->getRs1Reg()->getName() : "";
        uint64_t rs1_val = insn->hasRs1() ? insn->getRs1Reg()->dmiRead<uint64_t>() : 0;
        const std::string rs2_name = insn->hasRs2() ? insn->getRs2Reg()->getName() : "";
        uint64_t rs2_val = insn->hasRs2() ? insn->getRs2Reg()->dmiRead<uint64_t>() : 0;
        const std::string rd_name = insn->hasRd() ? insn->getRdReg()->getName() : "";

        uint64_t rd_val_before = 0;
        uint64_t rd_val_after = 0;
        uint64_t cosim_rd_val_after = 0;

        if (insn->hasRd())
        {
            Observer* obs = !observers_.empty() ? observers_.front().get() : nullptr;
            sparta_assert(obs, "No observers enabled, nothing to debug!");

            auto rd_reg = insn->getRdReg();
            rd_val_before = obs->getPrevRdValue();
            rd_val_after = rd_reg->dmiRead<uint64_t>();

            switch (rd_reg->getGroupNum())
            {
                case 0:
                    // INT
                    cosim_rd_val_after = cosim_query_->getIntRegValue(getHartId(), rd_reg->getID());
                    break;
                case 1:
                    // FP
                    cosim_rd_val_after = cosim_query_->getFpRegValue(getHartId(), rd_reg->getID());
                    break;
                case 2:
                    // VEC
                    cosim_rd_val_after = cosim_query_->getVecRegValue(getHartId(), rd_reg->getID());
                    break;
                case 3:
                    // Let this go to the default case assert. CSRs should not be written to in RD.
                default:
                    sparta_assert(false, "Invalid register group num!");
                    break;
            }
        }

        int has_imm = insn->hasImmediate() ? 1 : 0;
        uint64_t imm = insn->hasImmediate() ? insn->getImmediate() : 0;
        std::string disasm = insn->dasmString();
        std::string mnemonic = insn->getMnemonic();
        uint64_t opcode = insn->getOpcode();
        uint64_t pc = getPc();
        int priv = (int)priv_mode_;

        // Capture the CSR values from both simulators after processing an exception.
        std::vector<std::tuple<std::string, uint64_t, uint64_t>> all_csr_vals;
        if (exception_unit_->getUnhandledException().isValid())
        {
            for (uint32_t reg_idx = 0; reg_idx < csr_rset_->getNumRegisters(); ++reg_idx)
            {
                if (auto atlas_reg = csr_rset_->getRegister(reg_idx))
                {
                    if (cosim_query_->isCsrImplemented(atlas_reg->getName()))
                    {
                        auto atlas_val = atlas_reg->dmiRead<uint64_t>();
                        auto cosim_val =
                            cosim_query_->getCsrRegValue(getHartId(), atlas_reg->getName());
                        all_csr_vals.push_back(
                            std::make_tuple(atlas_reg->getName(), atlas_val, cosim_val));
                    }
                }
            }
        }

        int result_code = compareWithCoSimAndSync_();

        std::unique_ptr<simdb::WorkerTask> task(
            new InstSnapshotter(cosim_db_.get(), hart_id_, rs1_name, rs1_val, rs2_name, rs2_val,
                                rd_name, rd_val_before, rd_val_after, cosim_rd_val_after, has_imm,
                                imm, disasm, mnemonic, opcode, pc, priv, result_code));

        cosim_db_->getTaskQueue()->addWorkerTask(std::move(task));

        if (!all_csr_vals.empty())
        {
            task.reset(new CsrValuesSnapshotter(cosim_db_.get(), hart_id_, pc, all_csr_vals));
            cosim_db_->getTaskQueue()->addWorkerTask(std::move(task));
        }
    }

    void AtlasState::boot()
    {
        std::cout << "Booting hartid " << std::dec << hart_id_ << std::endl;
        {
            AtlasState* state = this;

            POKE_CSR_REG(MHARTID, hart_id_);

            // TODO: Initiali MISA CSR with XLEN and enabled extensions
            const uint64_t xlen_val = (xlen_ == 64) ? 2 : 1;
            POKE_CSR_FIELD(MISA, mxl, xlen_val);

            // Initialize MSTATUS/STATUS with User and Supervisor mode XLEN
            POKE_CSR_FIELD(MSTATUS, uxl, xlen_val);
            POKE_CSR_FIELD(MSTATUS, sxl, xlen_val);
            POKE_CSR_FIELD(SSTATUS, uxl, xlen_val);

            std::cout << state->getCsrRegister(MHARTID) << std::endl;
            std::cout << state->getCsrRegister(MISA) << std::endl;
            std::cout << state->getCsrRegister(MSTATUS) << std::endl;
            std::cout << state->getCsrRegister(SSTATUS) << std::endl;
        }

        if (interactive_mode_)
        {
            auto observer = std::make_unique<SimController>();
            sim_controller_ = observer.get();
            addObserver(std::move(observer));
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
