#pragma once

#include "core/ActionGroup.hpp"
#include "core/PegasusAllocatorWrapper.hpp"
#include "core/PegasusInst.hpp"
#include "core/observers/Observer.hpp"
#include "core/CoSimQuery.hpp"

#include "arch/RegisterSet.hpp"
#include "include/PegasusTypes.hpp"
#include "include/CSRBitMasks64.hpp"
#include "include/CSRHelpers.hpp"

#include "sim/PegasusAllocators.hpp"

#include "mavis/mavis/extension_managers/RISCVExtensionManager.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

#ifndef REG32_JSON_DIR
#error "REG32_JSON_DIR must be defined"
#endif

#ifndef REG64_JSON_DIR
#error "REG64_JSON_DIR must be defined"
#endif

template <class InstT, class ExtenT, class InstTypeAllocator, class ExtTypeAllocator> class Mavis;

namespace pegasus
{
    class PegasusInst;
    using PegasusInstPtr = sparta::SpartaSharedPointer<PegasusInst>;
    class PegasusSystem;
    class Fetch;
    class Execute;
    class Translate;
    class Exception;
    class SimController;
    class VectorState;
    class STFLogger;
    class SystemCallEmulator;
    class VectorConfig;

    using MavisType =
        Mavis<PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
              PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>;

    class PegasusState : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "PegasusState";
        using base_type = PegasusState;

        class PegasusStateParameters : public sparta::ParameterSet
        {
          public:
            PegasusStateParameters(sparta::TreeNode* node) : sparta::ParameterSet(node)
            {
                vlen.addDependentValidationCallback(&PegasusStateParameters::validateVlen_,
                                                    "VLEN constraint");
            }

            PARAMETER(uint32_t, hart_id, 0, "Hart ID")
            PARAMETER(std::string, isa_string, "rv64gcbv_zicsr_zifencei_zicond_zcb", "ISA string")
            PARAMETER(uint32_t, vlen, 128, "Vector register size in bits")
            PARAMETER(std::string, isa_file_path, "mavis_json", "Where are the Mavis isa files?")
            PARAMETER(std::string, uarch_file_path, "arch", "Where are the Pegasus uarch files?")
            PARAMETER(std::string, csr_values, "arch/default_csr_values.json",
                      "Provides initial values of CSRs")
            PARAMETER(bool, stop_sim_on_wfi, false, "Executing a WFI instruction stops simulation")
            PARAMETER(std::string, stf_filename, "",
                      "STF Trace file name (when not given, STF tracing is disabled)")

          private:
            static bool validateVlen_(uint32_t & vlen_val, const sparta::TreeNode*)
            {
                const std::vector<uint32_t> valid_vlen_values{128, 256, 512, 1024, 2048};
                return std::find(valid_vlen_values.begin(), valid_vlen_values.end(), vlen_val)
                       != valid_vlen_values.end();
            }
        };

        PegasusState(sparta::TreeNode* core_node, const PegasusStateParameters* p);

        // Not default -- defined in source file to reduce massive inlining
        virtual ~PegasusState();

        HartId getHartId() const { return hart_id_; }

        uint64_t getXlen() const { return xlen_; }

        mavis::extension_manager::riscv::RISCVExtensionManager & getExtensionManager()
        {
            return extension_manager_;
        }

        MavisType* getMavis() { return mavis_.get(); }

        enum MavisUIDs : mavis::InstructionUniqueID
        {
            MAVIS_UID_CSRRW = 1,
            MAVIS_UID_CSRRS,
            MAVIS_UID_CSRRC,
            MAVIS_UID_CSRRWI,
            MAVIS_UID_CSRRSI,
            MAVIS_UID_CSRRCI
        };

        std::set<std::string> & getMavisInclusions() { return inclusions_; }

        void changeMavisContext();

        bool getStopSimOnWfi() const { return stop_sim_on_wfi_; }

        void setPc(Addr pc) { pc_ = pc; }

        Addr getPc() const { return pc_; }

        Addr getPrevPc() const { return prev_pc_; }

        void setNextPc(Addr next_pc) { next_pc_ = next_pc; }

        Addr getNextPc() const { return next_pc_; }

        uint64_t getPcAlignment() const { return pc_alignment_; }

        uint64_t getPcAlignmentMask() const { return pc_alignment_mask_; }

        PrivMode getPrivMode() const { return priv_mode_; }

        PrivMode getLdstPrivMode() const { return ldst_priv_mode_; }

        bool getVirtualMode() const { return virtual_mode_; }

        void setPrivMode(PrivMode priv_mode, bool virt_mode)
        {
            virtual_mode_ = virt_mode && (priv_mode != PrivMode::MACHINE);
            priv_mode_ = priv_mode;
        }

        using Reservation = sparta::utils::ValidValue<Addr>;

        Reservation & getReservation() { return reservation_; }

        const Reservation & getReservation() const { return reservation_; }

        template <typename XLEN> void changeMMUMode();

        struct SimState
        {
            uint32_t current_opcode = 0;
            uint64_t current_uid = 0;
            PegasusInstPtr current_inst = nullptr;
            uint64_t inst_count = 0;
            bool sim_stopped = false;
            bool test_passed = true;
            int64_t workload_exit_code = 0;

            void reset()
            {
                current_opcode = 0;
                current_inst.reset();
            }
        };

        const SimState* getSimState() const { return &sim_state_; }

        SimState* getSimState() { return &sim_state_; }

        const VectorConfig* getVectorConfig() const { return vector_config_.get(); }

        VectorConfig* getVectorConfig() { return vector_config_.get(); }

        const PegasusInstPtr & getCurrentInst() { return sim_state_.current_inst; }

        void setCurrentInst(PegasusInstPtr inst)
        {
            inst->setUid(sim_state_.current_uid);
            sim_state_.current_inst = inst;
        }

        // Is the "H" extension enabled?
        bool hasHypervisor() const { return hypervisor_enabled_; }

        PegasusTranslationState* getFetchTranslationState() { return &fetch_translation_state_; }

        PegasusSystem* getPegasusSystem() const { return pegasus_system_; }

        void setPegasusSystem(PegasusSystem* pegasus_system) { pegasus_system_ = pegasus_system; }

        void enableInteractiveMode();

        void useSpikeFormatting();

        void setSystemCallEmulator(SystemCallEmulator* emulator)
        {
            system_call_emulator_ = emulator;
        }

        // Emulate ecall.  This function will determine the route to
        // send the emulation.  The return value is the return code
        // from the call.
        int64_t emulateSystemCall(const SystemCallStack &);

        Fetch* getFetchUnit() const { return fetch_unit_; }

        Execute* getExecuteUnit() const { return execute_unit_; }

        Translate* getTranslateUnit() const { return translate_unit_; }

        pegasus::RegisterSet* getIntRegisterSet() { return int_rset_.get(); }

        pegasus::RegisterSet* getFpRegisterSet() { return fp_rset_.get(); }

        pegasus::RegisterSet* getVecRegisterSet() { return vec_rset_.get(); }

        pegasus::RegisterSet* getCsrRegisterSet() { return csr_rset_.get(); }

        sparta::Register* getIntRegister(uint32_t reg_num)
        {
            return int_rset_->getRegister(reg_num);
        }

        sparta::Register* getFpRegister(uint32_t reg_num) { return fp_rset_->getRegister(reg_num); }

        sparta::Register* getVecRegister(uint32_t reg_num)
        {
            return vec_rset_->getRegister(reg_num);
        }

        sparta::Register* getCsrRegister(uint32_t reg_num)
        {
            return csr_rset_->getRegister(reg_num);
        }

        sparta::Register* findRegister(const std::string & reg_name, bool must_exist = true) const;

        template <typename MemoryType> MemoryType readMemory(const Addr paddr);

        template <typename MemoryType> void writeMemory(const Addr paddr, const MemoryType value);

        void addObserver(std::unique_ptr<Observer> observer);

        void insertExecuteActions(ActionGroup* action_group);

        ActionGroup* getFinishActionGroup() { return &finish_action_group_; }

        ActionGroup* getStopSimActionGroup() { return &stop_sim_action_group_; }

        Exception* getExceptionUnit() const { return exception_unit_; }

        void stopSim(const int64_t exit_code)
        {
            sim_state_.workload_exit_code = exit_code;
            sim_state_.test_passed = (exit_code == 0) ? true : false;
            sim_state_.sim_stopped = true;

            finish_action_group_.setNextActionGroup(&stop_sim_action_group_);
        }

        // Initialze a program stack (argc, argv, envp, auxv, etc)
        void setupProgramStack(const std::vector<std::string> & program_arguments);

        // For standalone Pegasus simulations, this method will be
        // called at the top of PegasusSim::run()
        void boot();

        // One-time cleanup phase after simulation end.
        void cleanup();

      private:
        void onBindTreeEarly_() override;
        void onBindTreeLate_() override;

        Action::ItrType preExecute_(PegasusState* state, Action::ItrType action_it);
        Action::ItrType postExecute_(PegasusState* state, Action::ItrType action_it);
        Action::ItrType preException_(PegasusState* state, Action::ItrType action_it);

        Action pre_execute_action_;
        Action post_execute_action_;
        Action pre_exception_action_;

        Action::ItrType stopSim_(PegasusState*, Action::ItrType action_it)
        {
            for (auto & obs : observers_)
            {
                obs->stopSim();
            }

            return ++action_it;
        }

        //! Hart ID
        const HartId hart_id_;

        // ISA string
        const std::string isa_string_;

        // VLEN (128, 256, 512, 1024 or 2048 bits)
        const uint32_t vlen_;

        // XLEN (either 32 or 64 bit)
        uint64_t xlen_ = 64;

        // Supported ISA string
        const std::string supported_isa_string_;

        template <typename XLEN> uint32_t getMisaExtFieldValue_() const;

        // Path to Mavis
        const std::string isa_file_path_;

        // Path to Pegasus
        const std::string uarch_file_path_;

        // Get Pegasus arch JSONs for Mavis
        mavis::FileNameListType getUArchFiles_() const;

        // CSR Initial Values JSON
        const std::string csr_values_json_;

        // Mavis extension manager
        mavis::extension_manager::riscv::RISCVExtensionManager extension_manager_;

        // Mavis
        std::unique_ptr<MavisType> mavis_;

        static inline mavis::InstUIDList mavis_uid_list_{
            {"csrrw", MAVIS_UID_CSRRW},   {"csrrs", MAVIS_UID_CSRRS},
            {"csrrc", MAVIS_UID_CSRRC},   {"csrrwi", MAVIS_UID_CSRRWI},
            {"csrrsi", MAVIS_UID_CSRRSI}, {"csrrci", MAVIS_UID_CSRRCI}};

        // Mavis list of included extension tags
        std::set<std::string> inclusions_;

        //! Stop simulatiion on WFI
        const bool stop_sim_on_wfi_;

        // STF Trace Filename
        const std::string stf_filename_;

        //! Do we have hypervisor?
        const bool hypervisor_enabled_;

        //! Current pc
        Addr pc_ = 0x0;

        //! Next pc
        Addr next_pc_ = 0x0;

        //! Previous pc
        Addr prev_pc_ = 0x0;

        //! PC alignment
        uint64_t pc_alignment_ = 4;

        //! PC alignment
        uint64_t pc_alignment_mask_ = ~(pc_alignment_ - 1);

        void setPcAlignment_(uint64_t pc_alignment)
        {
            sparta_assert(pc_alignment == 2 || pc_alignment == 4,
                          "Invalid PC alignment value! " << pc_alignment);
            pc_alignment_ = pc_alignment;
            pc_alignment_mask_ = ~(pc_alignment - 1);
        }

        //! Current privilege mode
        PrivMode priv_mode_ = PrivMode::MACHINE;

        //! Current privilege mode for LS translation
        PrivMode ldst_priv_mode_ = PrivMode::MACHINE;

        //! Current virtual translation mode
        bool virtual_mode_ = false;

        //! LR/SC Reservations
        Reservation reservation_;

        //! Simulation state
        SimState sim_state_;

        //! Vector state
        std::unique_ptr<VectorConfig> vector_config_;

        // Increment PC Action
        Action::ItrType incrementPc_(PegasusState* state, Action::ItrType action_it);
        pegasus::Action increment_pc_action_;

        // Translation/MMU state
        PegasusTranslationState fetch_translation_state_;

        //! PegasusSystem for accessing memory
        PegasusSystem* pegasus_system_ = nullptr;

        //! System Call Emulator for ecall emulation
        SystemCallEmulator* system_call_emulator_ = nullptr;

        // Fetch Unit
        Fetch* fetch_unit_ = nullptr;

        // Execute Unit
        Execute* execute_unit_ = nullptr;

        // Translate Unit
        Translate* translate_unit_ = nullptr;

        // Exception Unit
        Exception* exception_unit_ = nullptr;

        // Register set holding all Sparta registers from all generated JSON files
        std::unique_ptr<RegisterSet> int_rset_;
        std::unique_ptr<RegisterSet> fp_rset_;
        std::unique_ptr<RegisterSet> vec_rset_;
        std::unique_ptr<RegisterSet> csr_rset_;

        // Cached registers by name
        std::unordered_map<std::string, sparta::Register*> registers_by_name_;

        // Observers
        std::vector<std::unique_ptr<Observer>> observers_;

        // MessageSource used for InstructionLogger
        sparta::log::MessageSource inst_logger_;

        // Finish ActionGroup for post-execute simulator Actions
        ActionGroup finish_action_group_;

        // Stop simulation Action
        Action stop_action_;
        ActionGroup stop_sim_action_group_;

        // Co-simulation debug utils
        std::shared_ptr<CoSimQuery> cosim_query_;
        std::unordered_map<std::string, int> reg_ids_by_name_;
        SimController* sim_controller_ = nullptr;
    };

    template <typename XLEN>
    static inline XLEN READ_INT_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return (reg_ident == 0) ? 0 : state->getIntRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void WRITE_INT_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if (reg_ident != 0)
        {
            state->getIntRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
        }
    }

    template <typename XLEN> static inline XLEN READ_FP_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return state->getFpRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void WRITE_FP_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        state->getFpRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
    }

    template <typename VLEN>
    static inline VLEN READ_VEC_REG(PegasusState* state, uint32_t reg_ident)
    {
        return state->getVecRegister(reg_ident)->dmiRead<VLEN>();
    }

    template <typename VLEN>
    static inline void WRITE_VEC_REG(PegasusState* state, uint32_t reg_ident, VLEN reg_value)
    {
        state->getVecRegister(reg_ident)->dmiWrite<VLEN>(reg_value);
    }

    template <typename Elem>
    static inline Elem READ_VEC_ELEM(PegasusState* state, uint32_t reg_ident, uint32_t idx)
    {
        return state->getVecRegister(reg_ident)->dmiRead<Elem>(idx);
    }

    template <typename Elem>
    static inline void WRITE_VEC_ELEM(PegasusState* state, uint32_t reg_ident, Elem value,
                                      uint32_t idx)
    {
        state->getVecRegister(reg_ident)->dmiWrite<Elem>(value, idx);
    }

    template <typename XLEN>
    static inline XLEN READ_CSR_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return state->getCsrRegister(reg_ident)->read<XLEN>();
    }

    template <typename XLEN>
    static inline void WRITE_CSR_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if (const auto mask = pegasus::getCsrBitMask<XLEN>(reg_ident);
            mask != std::numeric_limits<XLEN>::max())
        {
            auto reg = state->getCsrRegister(reg_ident);
            const auto old_value = reg->dmiRead<XLEN>();
            const auto write_val = (old_value & ~mask) | (reg_value & mask);
            reg->write<XLEN>(write_val);
        }
        else
        {
            state->getCsrRegister(reg_ident)->write<XLEN>(reg_value);
        }
    }

    template <typename XLEN>
    static inline XLEN PEEK_CSR_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return state->getCsrRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void POKE_CSR_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        state->getCsrRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
    }

    template <typename XLEN>
    static inline XLEN READ_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                      const char* field_name)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            return state->getCsrRegister(reg_ident)->dmiRead<XLEN>();
        }
        else
        {
            return ((state->getCsrRegister(reg_ident)->dmiRead<XLEN>() >> field_lsb)
                    & ((XLEN(1) << (field_msb - field_lsb + 1)) - 1));
        }
    }

    template <typename XLEN>
    static inline void WRITE_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                       const char* field_name, uint64_t field_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        XLEN csr_value = READ_CSR_REG<XLEN>(state, reg_ident);

        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            WRITE_CSR_REG<XLEN>(state, reg_ident, field_value);
        }
        else
        {
            const XLEN mask = ((XLEN(1) << (field_msb - field_lsb + 1)) - 1) << field_lsb;
            csr_value &= ~mask;

            const XLEN new_field_value = field_value << field_lsb;
            csr_value |= new_field_value;

            WRITE_CSR_REG<XLEN>(state, reg_ident, csr_value);
        }
    }

    template <typename XLEN>
    static inline void POKE_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                      const char* field_name, uint64_t field_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        XLEN csr_value = READ_CSR_REG<XLEN>(state, reg_ident);

        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            POKE_CSR_REG<XLEN>(state, reg_ident, field_value);
        }
        else
        {
            const XLEN mask = ((XLEN(1) << (field_msb - field_lsb + 1)) - 1) << field_lsb;
            csr_value &= ~mask;

            const XLEN new_field_value = field_value << field_lsb;
            csr_value |= new_field_value;

            POKE_CSR_REG<XLEN>(state, reg_ident, csr_value);
        }
    }
} // namespace pegasus
