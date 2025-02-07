#include "AtlasSim.hpp"
#include "include/ActionTags.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include "arch/register_macros.hpp"
#include <filesystem>

// Core database headers
#include "simdb/ObjectManager.hpp"
#include "simdb/ObjectRef.hpp"
#include "simdb/TableRef.hpp"
#include "simdb/utils/ObjectQuery.hpp"
#include "simdb/utils/BlobHelpers.hpp"
#include "simdb/async/AsyncTaskEval.hpp"

// SQLite-specific headers
#include "simdb/impl/sqlite/SQLiteConnProxy.hpp"
#include "simdb/impl/sqlite/TransactionUtils.hpp"
#include "simdb/impl/sqlite/Errors.hpp"
#include <sqlite3.h>

namespace atlas
{
    AtlasSim::AtlasSim(sparta::Scheduler* scheduler, const std::string & workload, uint64_t ilimit,
                       bool interactive) :
        sparta::app::Simulation("AtlasSim", scheduler),
        workload_(workload),
        ilimit_(ilimit),
        interactive_(interactive)
    {
    }

    AtlasSim::~AtlasSim()
    {
        for (auto state : state_)
        {
            state->cleanup();
        }

        getRoot()->enterTeardown();

        if (cosim_db_)
        {
            cosim_db_->getTaskQueue()->flushQueue();
            cosim_db_->getTaskQueue()->stopThread();

            const std::string cmd = "mv " + cosim_db_->getDatabaseFile() + " "
                                    + std::filesystem::path(workload_).filename().string() + ".wdb";
            cosim_db_.reset();
            (void)system(cmd.c_str());
        }
    }

    void AtlasSim::run(uint64_t run_time)
    {
        for (auto state : state_)
        {
            state->boot();
        }

        getSimulationConfiguration()->scheduler_exacting_run = true;
        getSimulationConfiguration()->scheduler_measure_run_time = false;
        auto start = std::chrono::system_clock::system_clock::now();
        sparta::app::Simulation::run(run_time);
        auto end = std::chrono::system_clock::system_clock::now();
        auto sim_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        const HartId hart_id = 0;
        const AtlasState* state = state_.at(hart_id);
        const uint64_t inst_count = state->getSimState()->inst_count;
        std::cout << "Instructions executed: " << std::dec << inst_count << std::endl;
        std::cout << "Raw time (seconds): " << std::dec << (sim_time / 1000000.0) << std::endl;
        std::cout << "MIPS: " << std::dec << (inst_count / (sim_time / 1000000.0)) << std::endl;

        // TODO: mem usage, workload exit code
    }

    void AtlasSim::enableCoSimDebugger(std::unique_ptr<CoSimQuery> query)
    {
        using RegisterInfo = AtlasState::RegisterInfo;
        using RegisterInfoByHart = std::unordered_map<uint32_t, std::vector<RegisterInfo>>;
        RegisterInfoByHart reg_info_by_hart;

        for (uint32_t hart = 0; hart < state_.size(); ++hart)
        {
            auto state = state_.at(hart);

            auto int_rset = state->getIntRegisterSet();
            for (uint32_t reg_idx = 0; reg_idx < query->getNumIntRegisters(); ++reg_idx)
            {
                auto reg = int_rset->getRegister(reg_idx);
                const uint64_t expected = query->getIntRegValue(hart, reg->getID());
                const uint64_t actual = reg->dmiRead<uint64_t>();
                reg_info_by_hart[hart].push_back(std::make_tuple(reg->getName(), reg->getGroupNum(),
                                                                 reg->getID(), expected, actual));
            }

            auto fp_rset = state->getFpRegisterSet();
            for (uint32_t reg_idx = 0; reg_idx < query->getNumFpRegisters(); ++reg_idx)
            {
                auto reg = fp_rset->getRegister(reg_idx);
                const uint64_t expected = query->getFpRegValue(hart, reg->getID());
                const uint64_t actual = reg->dmiRead<uint64_t>();
                reg_info_by_hart[hart].push_back(std::make_tuple(reg->getName(), reg->getGroupNum(),
                                                                 reg->getID(), expected, actual));
            }

            auto vec_rset = state->getVecRegisterSet();
            for (uint32_t reg_idx = 0; reg_idx < query->getNumVecRegisters(); ++reg_idx)
            {
                auto reg = vec_rset->getRegister(reg_idx);
                const uint64_t expected = query->getVecRegValue(hart, reg->getID());
                const uint64_t actual = reg->dmiRead<uint64_t>();
                reg_info_by_hart[hart].push_back(std::make_tuple(reg->getName(), reg->getGroupNum(),
                                                                 reg->getID(), expected, actual));
            }

            auto csr_rset = state->getCsrRegisterSet();
            for (uint32_t reg_idx = 0; reg_idx < csr_rset->getNumRegisters(); ++reg_idx)
            {
                if (auto reg = csr_rset->getRegister(reg_idx))
                {
                    if (query->isCsrImplemented(reg->getName()))
                    {
                        const uint64_t expected = query->getCsrRegValue(hart, reg->getName());
                        const uint64_t actual = reg->dmiRead<uint64_t>();
                        reg_info_by_hart[hart].push_back(std::make_tuple(
                            reg->getName(), reg->getGroupNum(), reg->getID(), expected, actual));
                    }
                }
            }
        }

        cosim_query_.reset(query.release());

        using dt = simdb::ColumnDataType;
        simdb::Schema schema;

        schema.addTable("Registers")
            .addColumn("HartId", dt::int32_t)
            .addColumn("RegName", dt::string_t)
            .addColumn("RegType", dt::int32_t)
            .addColumn("RegIdx", dt::int32_t)
            .addColumn("ExpectedInitVal", dt::uint64_t)
            .addColumn("ActualInitVal", dt::uint64_t);

        schema.addTable("Instructions")
            .addColumn("HartId", dt::int32_t)
            .addColumn("Rs1", dt::string_t)
            .addColumn("Rs1Val", dt::uint64_t)
            .addColumn("Rs2", dt::string_t)
            .addColumn("Rs2Val", dt::uint64_t)
            .addColumn("Rd", dt::string_t)
            .addColumn("RdValBefore", dt::uint64_t)
            .addColumn("RdValAfter", dt::uint64_t)
            .addColumn("TruthRdValAfter", dt::uint64_t)
            .addColumn("HasImm", dt::int32_t)
            .addColumn("Imm", dt::uint64_t)
            .addColumn("Disasm", dt::string_t)
            .addColumn("Mnemonic", dt::string_t)
            .addColumn("Opcode", dt::uint64_t)
            .addColumn("PC", dt::uint64_t)
            .addColumn("Priv", dt::int32_t)
            .addColumn("ResultCode", dt::int32_t);

        schema.addTable("PostInstCsrVals")
            .addColumn("HartId", dt::int32_t)
            .addColumn("PC", dt::uint64_t)
            .addColumn("CsrName", dt::string_t)
            .addColumn("AtlasCsrVal", dt::uint64_t)
            .addColumn("CosimCsrVal", dt::uint64_t);

        cosim_db_.reset(new simdb::ObjectManager);
        cosim_db_->disableWarningMessages();
        cosim_db_->createDatabaseFromSchema(schema, std::make_unique<simdb::SQLiteConnProxy>());

        cosim_db_->safeTransaction(
            [&]()
            {
                for (uint32_t hart = 0; hart < state_.size(); ++hart)
                {
                    auto state = state_.at(hart);
                    state->enableCoSimDebugger(cosim_db_, cosim_query_, reg_info_by_hart[hart]);
                }
            });
    }

    void AtlasSim::buildTree_()
    {
        auto root_tn = getRoot();

        // top.allocators
        allocators_tn_.reset(new AtlasAllocators(getRoot()));

        // top.system
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(root_tn, "system", "Atlas System", &system_factory_));

        // top.core
        sparta::TreeNode* core_tn = nullptr;
        const std::string core_name = "core0";
        const uint32_t core_idx = 0;
        tns_to_delete_.emplace_back(
            core_tn = new sparta::ResourceTreeNode(root_tn, core_name, "cores", core_idx,
                                                   "Core State", &state_factory_));

        // top.core.fetch
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Fetch Unit", &fetch_factory_));

        // top.core.translate
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));

        // top.core.execute
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Execute Unit", &execute_factory_));

        // top.core.exception
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));
    }

    void AtlasSim::configureTree_()
    {
        // Set AtlasSystem workload parameter
        auto system_workload =
            getRoot()->getChildAs<sparta::ParameterBase>("system.params.workload");
        system_workload->setValueFromString(workload_);
    }

    void AtlasSim::bindTree_()
    {
        system_ = getRoot()->getChild("system")->getResourceAs<atlas::AtlasSystem>();

        const uint32_t num_harts = 1;
        for (uint32_t hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            // Get AtlasState and Fetch for each hart
            const std::string core_name = "core" + std::to_string(hart_id);
            state_.emplace_back(getRoot()->getChild(core_name)->getResourceAs<AtlasState*>());
            // Give AtlasState a pointer to AtlasSystem for accessing memory
            AtlasState* state = state_.back();
            state->setAtlasSystem(system_);
            state->setPc(system_->getStartingPc());

            if (interactive_)
            {
                state->enableInteractiveMode();
            }
        }
    }
} // namespace atlas
