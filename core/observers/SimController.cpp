#include "core/observers/SimController.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Exception.hpp"

namespace atlas
{

    class BreakpointManager
    {
      public:
        void breakOnPreExecute() { break_on_pre_execute_ = true; }

        void breakOnPreException() { break_on_pre_exception_ = true; }

        void breakOnPostExecute() { break_on_post_execute_ = true; }

        bool shouldBreakOnPreExecute(AtlasState*) const { return break_on_pre_execute_; }

        bool shouldBreakOnPreException(AtlasState*) const { return break_on_pre_exception_; }

        bool shouldBreakOnPostExecute(AtlasState*) const { return break_on_post_execute_; }

        void deleteAllBreakpoints()
        {
            break_on_pre_execute_ = false;
            break_on_pre_exception_ = false;
            break_on_post_execute_ = false;
        }

      private:
        bool break_on_pre_execute_ = false;
        bool break_on_post_execute_ = false;
        bool break_on_pre_exception_ = false;
    };

    class SimController::SimEndpoint
    {
      public:
        void postInit(AtlasState* state)
        {
            std::cout << "\nATLAS_IDE_READY\n";
            std::cout.flush();
            enterLoop_(state);
        }

        ActionGroup* preExecute(AtlasState* state)
        {
            if (break_on_pre_execute_)
            {
                sendString_("pre_execute");
                return enterLoop_(state);
            }
            return nullptr;
        }

        ActionGroup* preException(AtlasState* state)
        {
            if (break_on_pre_exception_)
            {
                sendString_("pre_exception");
                return enterLoop_(state);
            }
            return nullptr;
        }

        ActionGroup* postExecute(AtlasState* state)
        {
            if (break_on_post_execute_)
            {
                sendString_("post_execute");
                return enterLoop_(state);
            }
            return nullptr;
        }

        void onSimulationFinished(AtlasState* state)
        {
            sendString_("sim_finished");
            enterLoop_(state);
        }

      private:
        enum class SimCommand
        {
            XLEN,
            PC,
            EXIT_CODE,
            TEST_PASSED,
            SIM_STOPPED,
            INST_COUNT,
            INST_UID,
            INST_MNEMONIC,
            INST_DASM_STRING,
            INST_OPCODE,
            INST_PRIV,
            INST_HAS_IMM,
            INST_IMMEDIATE,
            INST_RS1,
            INST_RS2,
            INST_RD,
            INST_CSR,
            INST_TYPE,
            ACTIVE_EXCEPTION,
            NUM_REGS_IN_GROUP,
            CSR_NAME,
            REG_GROUP_NUM,
            REG_ID,
            REG_VALUE,
            REG_WRITE,
            REG_DMI_WRITE,
            BREAKPOINT,
            FINISH_EXECUTE,
            CONTINUE_SIM,
            FINISH_SIM,
            KILL_SIM,
            NOP
        };

        SimCommand getSimCommand_(const std::string & request,
                                  std::vector<std::string> & args) const
        {
            // Use an istringstream to split the string
            std::istringstream stream(request);

            // Extract words (tokens) one by one and push them to the vector
            std::string command_str;
            std::string word;
            while (stream >> word)
            {
                if (command_str.empty())
                {
                    command_str = word;
                }
                else
                {
                    args.push_back(word);
                }
            }

            static const std::unordered_map<std::string, SimCommand> sim_commands = {
                {"state.xlen", SimCommand::XLEN},
                {"state.pc", SimCommand::PC},
                {"state.exit_code", SimCommand::EXIT_CODE},
                {"state.test_passed", SimCommand::TEST_PASSED},
                {"state.sim_stopped", SimCommand::SIM_STOPPED},
                {"state.inst_count", SimCommand::INST_COUNT},
                {"state.sim_stopped", SimCommand::SIM_STOPPED},
                {"inst.uid", SimCommand::INST_UID},
                {"inst.mnemonic", SimCommand::INST_MNEMONIC},
                {"inst.dasm_string", SimCommand::INST_DASM_STRING},
                {"inst.opcode", SimCommand::INST_OPCODE},
                {"inst.priv", SimCommand::INST_PRIV},
                {"inst.has_immediate", SimCommand::INST_HAS_IMM},
                {"inst.immediate", SimCommand::INST_IMMEDIATE},
                {"inst.rs1.name", SimCommand::INST_RS1},
                {"inst.rs2.name", SimCommand::INST_RS2},
                {"inst.rd.name", SimCommand::INST_RD},
                {"inst.csr.name", SimCommand::INST_CSR},
                {"inst.type", SimCommand::INST_TYPE},
                {"inst.active_exception", SimCommand::ACTIVE_EXCEPTION},
                {"state.num_regs_in_group", SimCommand::NUM_REGS_IN_GROUP},
                {"csr.name", SimCommand::CSR_NAME},
                {"reg.group_num", SimCommand::REG_GROUP_NUM},
                {"reg.reg_id", SimCommand::REG_ID},
                {"reg.value", SimCommand::REG_VALUE},
                {"reg.write", SimCommand::REG_WRITE},
                {"reg.dmiwrite", SimCommand::REG_DMI_WRITE},
                {"sim.break", SimCommand::BREAKPOINT},
                {"sim.finish_execute", SimCommand::FINISH_EXECUTE},
                {"sim.continue", SimCommand::CONTINUE_SIM},
                {"sim.finish", SimCommand::FINISH_SIM},
                {"sim.kill", SimCommand::KILL_SIM}};

            if (const auto it = sim_commands.find(command_str); it != sim_commands.end())
            {
                return it->second;
            }

            return SimCommand::NOP;
        }

        std::string receiveRequest_()
        {
            std::string response;
            std::getline(std::cin, response);
            return response;
        }

        void sendJson_(const std::string & message)
        {
            std::cout << "ATLAS_IDE_RESPONSE: " << message << "\n";
            std::cout.flush();
        }

        // Note that the IDE doesn't really do anything with these
        // general-purpose ACKs. The reason we return something is
        // for overall consistency with the other message passing
        // code in python/C++.
        void sendAck_() { sendJson_("{\"response_code\": \"ok\"}"); }

        void sendWarning_(const std::string & warn)
        {
            const auto json =
                "{\"response_code\": \"warn\", \"response_payload\": \"" + warn + "\"}";
            sendJson_(json);
        }

        void sendError_(const std::string & err)
        {
            const auto json = "{\"response_code\": \"err\", \"response_payload\": \"" + err + "\"}";
            sendJson_(json);
        }

        template <typename T> void sendInt_(const T val)
        {
            static_assert(std::is_integral_v<T>);
            const auto json = "{\"response_code\": \"ok\", \"response_payload\": "
                              + std::to_string(val) + ", \"response_type\": \"int\"}";
            sendJson_(json);
        }

        template <typename T> void sendFloat_(const T val)
        {
            static_assert(std::is_floating_point_v<T>);
            const auto json = "{\"response_code\": \"ok\", \"response_payload\": "
                              + std::to_string(val) + ", \"response_type\": \"float\"}";
            sendJson_(json);
        }

        void sendBool_(const bool b)
        {
            const std::string b_str = b ? "true" : "false";
            const auto json = "{\"response_code\": \"ok\", \"response_payload\": " + b_str
                              + ", \"response_type\": \"bool\"}";
            sendJson_(json);
        }

        void sendString_(const std::string & s)
        {
            const auto json = "{\"response_code\": \"ok\", \"response_payload\": \"" + s
                              + "\", \"response_type\": \"str\"}";
            sendJson_(json);
        }

        ActionGroup* enterLoop_(AtlasState* state)
        {
            while (true)
            {
                std::vector<std::string> args;
                const std::string request = receiveRequest_();
                const SimCommand sim_cmd = getSimCommand_(request, args);

                atlas::ActionGroup* fail_action_group = nullptr;
                if (!handleSimCommand_(state, sim_cmd, args, fail_action_group))
                {
                    // sim.finish_execute:
                    //    fail_action_group = state->getFinishActionGroup();
                    //
                    // sim.kill:
                    //    fail_action_group = state->getStopSimActionGroup()
                    //
                    // sim.continue, sim.finish:
                    //    fail_action_group = nullptr
                    return fail_action_group;
                }
            }
        }

        // Returns TRUE if we are supposed to remain in the loop. The caller
        // should return fail_action_group to the simulator if we return false.
        bool handleSimCommand_(AtlasState* state, SimCommand sim_cmd,
                               const std::vector<std::string> & args,
                               ActionGroup*& fail_action_group)
        {
            switch (sim_cmd)
            {
                case SimCommand::XLEN:
                    sendInt_(state->getXlen());
                    return true;

                case SimCommand::PC:
                    sendInt_(state->getPc());
                    return true;

                case SimCommand::EXIT_CODE:
                    sendInt_(state->getSimState()->workload_exit_code);
                    return true;

                case SimCommand::TEST_PASSED:
                    sendBool_(state->getSimState()->test_passed);
                    return true;

                case SimCommand::SIM_STOPPED:
                    sendBool_(state->getSimState()->sim_stopped);
                    return true;

                case SimCommand::INST_COUNT:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendInt_(state->getSimState()->inst_count);
                        return true;
                    }

                case SimCommand::INST_UID:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendInt_(inst->getUid());
                        return true;
                    }

                case SimCommand::INST_MNEMONIC:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendString_(inst->getMnemonic());
                        return true;
                    }

                case SimCommand::INST_DASM_STRING:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendString_(inst->dasmString());
                        return true;
                    }

                case SimCommand::INST_OPCODE:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendInt_(inst->getOpcode());
                        return true;
                    }

                case SimCommand::INST_PRIV:
                    {
                        sendInt_((uint32_t)state->getPrivMode());
                        return true;
                    }

                case SimCommand::INST_HAS_IMM:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else
                            sendBool_(inst->hasImmediate());
                        return true;
                    }

                case SimCommand::INST_IMMEDIATE:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else if (!inst->hasImmediate())
                            sendError_("No immediate");
                        else
                            sendInt_(inst->getImmediate());
                        return true;
                    }

                case SimCommand::INST_RS1:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else if (!inst->hasRs1())
                            sendError_("No rs1");
                        else
                            sendString_(inst->getRs1Reg()->getName());
                        return true;
                    }

                case SimCommand::INST_RS2:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else if (!inst->hasRs2())
                            sendError_("No rs2");
                        else
                            sendString_(inst->getRs2Reg()->getName());
                        return true;
                    }

                case SimCommand::INST_RD:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                            sendError_("No instruction");
                        else if (!inst->hasRd())
                            sendError_("No rd");
                        else
                            sendString_(inst->getRdReg()->getName());
                        return true;
                    }

                case SimCommand::INST_CSR:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                        {
                            sendError_("No instruction");
                            return true;
                        }

                        auto opcode_info = inst->getMavisOpcodeInfo();
                        if (!opcode_info)
                        {
                            sendError_("No opcode info");
                            return true;
                        }

                        uint64_t csr = 0;
                        try
                        {
                            csr =
                                opcode_info->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
                        }
                        catch (...)
                        {
                            sendError_("No opcode info");
                            return true;
                        }

                        sparta::Register* reg = state->getCsrRegister(csr);
                        if (!reg)
                        {
                            sendError_("No csr");
                            return true;
                        }

                        sendString_(reg->getName());
                        return true;
                    }

                case SimCommand::INST_TYPE:
                    {
                        auto inst = state->getCurrentInst();
                        if (!inst)
                        {
                            sendError_("No instruction");
                            return true;
                        }

                        auto opcode_info = inst->getMavisOpcodeInfo();
                        if (!opcode_info)
                        {
                            sendError_("No opcode info");
                            return true;
                        }

                        sendInt_((int)opcode_info->getInstType());
                        return true;
                    }

                case SimCommand::ACTIVE_EXCEPTION:
                    {
                        const auto & exception = state->getExceptionUnit()->getUnhandledException();
                        sendInt_(exception.isValid() ? (int)exception.getValue() : -1);
                        return true;
                    }

                case SimCommand::NUM_REGS_IN_GROUP:
                    {
                        if (args.size() != 1)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto group_num = std::atoi(args[0].c_str());
                        atlas::RegisterSet* rset = nullptr;
                        switch (group_num)
                        {
                            case 0:
                                rset = state->getIntRegisterSet();
                                break;
                            case 1:
                                rset = state->getFpRegisterSet();
                                break;
                            case 2:
                                rset = state->getVecRegisterSet();
                                break;
                            case 3:
                                rset = state->getCsrRegisterSet();
                                break;
                            default:
                                sendError_("Invalid group number");
                                break;
                        }
                        sendInt_(rset ? rset->getNumRegisters() : -1);
                        return true;
                    }

                case SimCommand::CSR_NAME:
                    {
                        if (args.size() != 1)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto csr_num = std::strtoul(args[0].c_str(), nullptr, 0);
                        if (auto reg = state->getCsrRegister(csr_num))
                        {
                            sendString_(reg->getName());
                        }
                        else
                        {
                            sendError_("Invalid CSR register");
                        }
                        return true;
                    }

                case SimCommand::REG_GROUP_NUM:
                    {
                        if (args.size() != 1)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto reg = state->findRegister(args[0], false);
                        if (!reg)
                        {
                            sendError_("Invalid register");
                            break;
                        }
                        sendInt_(reg->getGroupNum());
                        return true;
                    }

                case SimCommand::REG_ID:
                    {
                        if (args.size() != 1)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto reg = state->findRegister(args[0], false);
                        if (!reg)
                        {
                            sendError_("Invalid register");
                            break;
                        }
                        sendInt_(reg->getID());
                        return true;
                    }

                case SimCommand::REG_VALUE:
                    {
                        if (args.size() != 1)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto reg = state->findRegister(args[0], false);
                        if (!reg)
                        {
                            sendError_("Invalid register");
                            break;
                        }
                        sendInt_(reg->dmiRead<uint64_t>());
                        return true;
                    }

                case SimCommand::REG_WRITE:
                    {
                        if (args.size() != 2)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto reg = state->findRegister(args[0], false);
                        if (!reg)
                        {
                            sendError_("Invalid register");
                            break;
                        }

                        std::stringstream ss;
                        ss << args[1];
                        uint64_t val;
                        ss >> val;

                        reg->write(val);
                        sendAck_();
                        return true;
                    }

                case SimCommand::REG_DMI_WRITE:
                    {
                        if (args.size() != 2)
                        {
                            sendError_("Invalid args");
                            break;
                        }
                        auto reg = state->findRegister(args[0], false);
                        if (!reg)
                        {
                            sendError_("Invalid register");
                            break;
                        }

                        std::stringstream ss;
                        ss << args[1];
                        uint64_t val;
                        ss >> val;

                        reg->dmiWrite(val);
                        sendAck_();
                        return true;
                    }

                case SimCommand::BREAKPOINT:
                    if (args.size() != 1)
                    {
                        sendError_("Invalid args");
                        break;
                    }
                    if (args[0] == "pre_execute")
                    {
                        break_on_pre_execute_ = true;
                        sendAck_();
                    }
                    else if (args[0] == "pre_exception")
                    {
                        break_on_pre_exception_ = true;
                        sendAck_();
                    }
                    else if (args[0] == "post_execute")
                    {
                        break_on_post_execute_ = true;
                        sendAck_();
                    }
                    else
                    {
                        sendError_("Invalid action");
                    }
                    return true;

                case SimCommand::FINISH_EXECUTE:
                    fail_action_group = state->getFinishActionGroup();
                    return false;

                case SimCommand::CONTINUE_SIM:
                    fail_action_group = nullptr;
                    return false;

                case SimCommand::FINISH_SIM:
                    break_on_pre_execute_ = false;
                    break_on_pre_exception_ = false;
                    break_on_post_execute_ = false;
                    fail_action_group = nullptr;
                    return false;

                case SimCommand::KILL_SIM:
                    if (args.size() == 1)
                    {
                        const auto exit_code = std::atoi(args[0].c_str());
                        state->getSimState()->workload_exit_code = exit_code;
                    }
                    else if (!args.empty())
                    {
                        sendError_("Invalid args");
                        break;
                    }

                    state->getSimState()->test_passed = false;
                    state->getSimState()->sim_stopped = true;
                    fail_action_group = state->getStopSimActionGroup();
                    return false;

                case SimCommand::NOP:
                    sendError_("Invalid command");
                    break;
            }

            return true;
        }

        bool break_on_pre_execute_ = false;
        bool break_on_post_execute_ = false;
        bool break_on_pre_exception_ = false;
    };

    SimController::SimController() : endpoint_(std::make_shared<SimEndpoint>()) {}

    void SimController::postInit(AtlasState* state) { endpoint_->postInit(state); }

    ActionGroup* SimController::preExecute(AtlasState* state)
    {
        return endpoint_->preExecute(state);
    }

    ActionGroup* SimController::postExecute(AtlasState* state)
    {
        return endpoint_->postExecute(state);
    }

    ActionGroup* SimController::preException(AtlasState* state)
    {
        return endpoint_->preException(state);
    }

    void SimController::onSimulationFinished(AtlasState* state)
    {
        endpoint_->onSimulationFinished(state);
    }

} // namespace atlas
