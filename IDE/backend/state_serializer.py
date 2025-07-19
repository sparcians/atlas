from backend.observers import Observer
from backend.sim_api import *
from backend.pegasus_dtypes import *
import json, re

class StateSerializer(Observer):
    def __init__(self, state_db, msg_queue, rv):
        Observer.__init__(self)
        self.state_db = state_db
        self.msg_queue = msg_queue
        self.insts_by_uid = {}
        self.csr_names = None
        self.infinite_loop_pc = None

        arch_json = os.path.join(os.path.dirname(__file__), '..', '..', 'arch', rv, 'reg_csr.json')
        with open(arch_json) as fin:
            self.csr_num_to_name = {}
            for info in json.load(fin):
                csr_name = info['name']
                csr_num = info['num']
                self.csr_num_to_name[csr_num] = csr_name

    class Instruction:
        def __init__(self, pc, opcode, priv, dasm, reg_info_query):
            self.pc = pc
            self.opcode = opcode
            self.priv = priv
            self.dasm = dasm
            self.reg_info_query = reg_info_query
            self.tracked_reg_values = {'resv_priv': priv}

        def AddChangable(self, endpoint, reg_name):
            self.tracked_reg_values[reg_name] = pegasus_reg_value(endpoint, reg_name)

        def Finalize(self, endpoint, state_db):
            uid = pegasus_current_uid(endpoint)
            state_db.AppendInstruction(self.pc, self.opcode, self.dasm, uid)

            # {'pc': 0xPC', 'priv': 3, 'reg_changes': [RegChange, RegChange, ...]}
            expected_priv = None
            expected_reg_changes = {}

            if self.reg_info_query:
                reg_info = self.reg_info_query.GetRegisterInfoAtPC(self.pc)
                expected_priv = reg_info['priv']
                for reg_change in reg_info['reg_changes']:
                    expected_reg_changes[reg_change.regname] = reg_change.regvalue

            for reg_name, prev_val in self.tracked_reg_values.items():
                if reg_name == 'resv_priv':
                    cur_val = pegasus_inst_priv(endpoint)
                else:
                    cur_val = pegasus_reg_value(endpoint, reg_name)

                if cur_val != prev_val:
                    expected_val = None
                    if reg_name == 'resv_priv':
                        expected_val = expected_priv
                    elif reg_name in expected_reg_changes:
                        expected_val = expected_reg_changes[reg_name]

                    state_db.AppendRegChange(uid, reg_name, cur_val, expected_val)

            mem_reads = pegasus_mem_reads(endpoint)
            mem_writes = pegasus_mem_writes(endpoint)
            if mem_reads or mem_writes:
                state_db.AppendMemAccesses(uid, mem_reads, mem_writes)

    def OnPreSimulation(self, endpoint):
        for reg_id in range(pegasus_num_regs_in_group(endpoint, 0)):
            reg_name = 'x' + str(reg_id)
            reg_val = pegasus_reg_value(endpoint, reg_name)
            if isinstance(reg_val, str):
                self.state_db.SetInitRegValue(reg_name, reg_val)

        for reg_id in range(pegasus_num_regs_in_group(endpoint, 1)):
            reg_name = 'f' + str(reg_id)
            reg_val = pegasus_reg_value(endpoint, reg_name)
            if isinstance(reg_val, str):
                self.state_db.SetInitRegValue(reg_name, reg_val)

        for reg_id in range(pegasus_num_regs_in_group(endpoint, 3)):
            reg_name = pegasus_csr_name(endpoint, reg_id)
            if reg_name:
                reg_val = pegasus_reg_value(endpoint, reg_name)
                if isinstance(reg_val, str):
                    self.state_db.SetInitRegValue(reg_name, reg_val)

        init_priv = pegasus_inst_priv(endpoint)
        self.state_db.SetInitRegValue('resv_priv', init_priv)

    def OnPreExecute(self, endpoint):
        # Note that there are cases where the current instruction is None,
        # due to hitting exceptions during fetch/translate/decode. In those
        # cases, we will enter OnPreException() first.
        inst = pegasus_current_inst(endpoint)
        assert inst is not None

        pc = pegasus_pc(endpoint)
        mnemonic = inst.getMnemonic()
        opcode = inst.getOpcode()
        priv = inst.getPrivMode()
        dasm = inst.dasmString()
        rd = inst.getRd()
        uid = pegasus_current_uid(endpoint)

        # Use regex to look for "CSR=<csr_hex>" in the dasm
        csr_match = re.search(r'CSR=0x([0-9a-fA-F]+)', dasm)
        if csr_match:
            csr_hex = csr_match.group(1)
            csr_name = self.csr_num_to_name.get(int(csr_hex, 16))
            dasm = re.sub(r'CSR=0x[0-9a-fA-F]+', 'CSR=' + csr_name, dasm)

        inst = self.Instruction(pc, opcode, priv, dasm, self.reg_info_query)
        if rd:
            inst.AddChangable(endpoint, rd.getName())

        self.insts_by_uid[uid] = inst

        # We not only snapshot CSR values on exceptions, but also on
        # select instructions that can change them too.
        if mnemonic in ('ecall', 'ebreak', 'fence', 'fence.i', 'mret', 'sret') or mnemonic.startswith('csrr'):
            self.__AddCsrChangables(endpoint, inst)

        # Add fcsr, frm, and fflags to the list of tracked registers
        # if the instruction is a floating-point instruction.
        inst_type = pegasus_inst_type(endpoint)
        is_fp = isinstance(inst_type, int) and inst_type == 1 << 1
        if is_fp:
            inst.AddChangable(endpoint, 'fcsr')
            inst.AddChangable(endpoint, 'frm')
            inst.AddChangable(endpoint, 'fflags')

        # Track any changes to this instruction's CSR ("special field").
        # Equivalent C++ under the hood:
        #
        #     auto inst = state->getCurrentInst();
        #     auto info = inst->getMavisOpcodeInfo()
        #     auto csr = info->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
        #     auto csr_name = state->getCsrRegister(csr)->getName();
        csr_name = pegasus_inst_csr(endpoint)
        if csr_name:
            inst.AddChangable(endpoint, csr_name)

    def OnPreException(self, endpoint):
        inst = pegasus_current_inst(endpoint)
        if inst is None:
            # This can occur when mavis::UnknownOpcode is thrown during
            # fetch/translate/decode. We don't have a current instruction.
            # We need to create a new instruction object in order to track any
            # CSR changes that may have occurred during the exception. Note that
            # if we don't have a current instruction, we cannot reliably ask for
            # the PC, opcode, etc. right now. We will try to fill in that data
            # in OnPostExecute().
            inst = self.Instruction(None, None, None, None, self.reg_info_query)
            uid = pegasus_current_uid(endpoint)
            self.insts_by_uid[uid] = inst
        else:
            uid = inst.getUid()
            if uid not in self.insts_by_uid:
                # A bad CSR was encountered during decode. Just like the
                # scenario above where we don't have a current instruction,
                # we need to create a new instruction object to track any
                # CSR changes that may have occurred during the exception.
                inst = self.Instruction(None, None, None, None, self.reg_info_query)
                self.insts_by_uid[uid] = inst
            else:
                # An exception occurred during instruction execution.
                inst = self.insts_by_uid[uid]

        self.__AddCsrChangables(endpoint, inst)

    def OnPostExecute(self, endpoint):
        uid = pegasus_current_uid(endpoint)
        if uid not in self.insts_by_uid:
            return

        msg = 'Executed {} instructions...'.format(uid+1)
        self.msg_queue.put(msg)

        inst = self.insts_by_uid[uid]
        if inst.pc is None:
            # The reason we have to take the previous PC and not the current PC
            # is due to the fact that incrementPc_() is called before the observers'
            # postExecute_() method is called.
            inst.pc = pegasus_prev_pc(endpoint)

        inst.Finalize(endpoint, self.state_db)
        del self.insts_by_uid[uid]

    def OnSimulationStuck(self, endpoint):
        self.infinite_loop_pc = pegasus_pc(endpoint)
        self.msg_queue.put('SIM_STUCK:{}'.format(self.infinite_loop_pc))

    def OnSimFinished(self, endpoint):
        self.msg_queue.put('SIM_FINISHED')

    def OnSimulationDead(self, pc):
        self.msg_queue.put('SIM_DEAD')

    def __AddCsrChangables(self, endpoint, inst):
        csr_names = self.__GetCsrNames(endpoint)
        for csr_name in csr_names:
            inst.AddChangable(endpoint, csr_name)

    def __GetCsrNames(self, endpoint):
        if self.csr_names is not None:
            return self.csr_names

        self.csr_names = []
        for reg_id in range(pegasus_num_regs_in_group(endpoint, 3)):
            reg_name = pegasus_csr_name(endpoint, reg_id)
            if reg_name:
                self.csr_names.append(reg_name)

        return self.csr_names
