#include "core/PegasusState.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "mavis/Mavis.h"
#include "sparta/simulation/RootTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/SpartaTester.hpp"

class FetchTester
{
  public:
    FetchTester()
    {
        root_tn_.setClock(&clk_);

        // top.allocators
        allocators_tn_.reset(new pegasus::PegasusAllocators(&root_tn_));

        // top.core
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(&root_tn_, "core0", "cores", 0,
                                                                 "Core State", &state_factory_));
        sparta::TreeNode* core_tn = tns_to_delete_.back().get();
        // top.core.fetch
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Fetch Unit", &fetch_factory_));
        sparta::TreeNode* fetch_tn = tns_to_delete_.back().get();
        // top.core.execute
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Execute Unit", &execute_factory_));
        // top.core.translate
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));
        // top.core.exception
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));

        root_tn_.enterConfiguring();
        root_tn_.enterFinalized();
        root_tn_.bindTreeEarly();
        root_tn_.bindTreeLate();
        root_tn_.validatePreRun();

        state_.reset(core_tn->getResourceAs<pegasus::PegasusState*>());
        fetch_.reset(fetch_tn->getResourceAs<pegasus::Fetch*>());
    }

    ~FetchTester() { root_tn_.enterTeardown(); }

  private:
    // Sparta components
    sparta::Scheduler scheduler_;
    sparta::Clock clk_{"clock", &scheduler_};
    sparta::RootTreeNode root_tn_;
    sparta::ResourceFactory<pegasus::PegasusState, pegasus::PegasusState::PegasusStateParameters>
        state_factory_;
    sparta::ResourceFactory<pegasus::Fetch, pegasus::Fetch::FetchParameters> fetch_factory_;
    sparta::ResourceFactory<pegasus::Execute, pegasus::Execute::ExecuteParameters> execute_factory_;
    sparta::ResourceFactory<pegasus::Translate, pegasus::Translate::TranslateParameters>
        translate_factory_;
    sparta::ResourceFactory<pegasus::Exception, pegasus::Exception::ExceptionParameters>
        exception_factory_;
    std::unique_ptr<pegasus::PegasusAllocators> allocators_tn_;
    std::vector<std::unique_ptr<sparta::ResourceTreeNode>> tns_to_delete_;

    // PegasusState
    std::unique_ptr<pegasus::PegasusState> state_;

    // Fetch
    std::unique_ptr<pegasus::Fetch> fetch_;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    FetchTester tester;

    REPORT_ERROR;
    return ERROR_CODE;
}
