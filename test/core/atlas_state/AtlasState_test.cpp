#include "core/AtlasState.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "sparta/simulation/RootTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include "core/STFtrace/STFLogger.hpp"

class AtlasStateTester
{
  public:
    AtlasStateTester()
    {
        root_tn_.setClock(&clk_);

        // top.allocators
        allocators_tn_.reset(new atlas::AtlasAllocators(&root_tn_));

        // top.core
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(&root_tn_, "core0", "cores", 0,
                                                                 "Core State", &state_factory_));
        sparta::TreeNode* core_tn = tns_to_delete_.back().get();
        // top.core.fetch
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Fetch Unit", &fetch_factory_));
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

        // top.core.fetch
        state_.reset(core_tn->getResourceAs<atlas::AtlasState*>());
    }

    ~AtlasStateTester() { root_tn_.enterTeardown(); }

    void testRegisterSet()
    {
        // TODO
    }

  private:
    // Sparta components
    sparta::Scheduler scheduler_;
    sparta::Clock clk_{"clock", &scheduler_};
    sparta::RootTreeNode root_tn_;
    sparta::ResourceFactory<atlas::AtlasState, atlas::AtlasState::AtlasStateParameters>
        state_factory_;
    sparta::ResourceFactory<atlas::Fetch, atlas::Fetch::FetchParameters> fetch_factory_;
    sparta::ResourceFactory<atlas::Execute, atlas::Execute::ExecuteParameters> execute_factory_;
    sparta::ResourceFactory<atlas::Translate, atlas::Translate::TranslateParameters>
        translate_factory_;
    sparta::ResourceFactory<atlas::Exception, atlas::Exception::ExceptionParameters>
        exception_factory_;
    std::unique_ptr<atlas::AtlasAllocators> allocators_tn_;
    std::vector<std::unique_ptr<sparta::ResourceTreeNode>> tns_to_delete_;

    // AtlasState
    std::unique_ptr<atlas::AtlasState> state_;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    AtlasStateTester tester;
    tester.testRegisterSet();

    REPORT_ERROR;
    return ERROR_CODE;
}
