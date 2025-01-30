from backend.observers import Observer
from backend.sim_api import *
from backend.atlas_dtypes import *

class StateSerializer(Observer):
    def __init__(self, state_db, msg_queue):
        Observer.__init__(self)
        self.state_db = state_db
        self.msg_queue = msg_queue
        self.insts_by_uid = {}
        self.csr_names = None
        self.infinite_loop_pc = None

    class Instruction:
        def __init__(self, pc, opcode, priv, dasm):
            self.pc = pc
            self.opcode = opcode
            self.priv = priv
            self.dasm = dasm
            self.tracked_reg_values = {'resv_priv': priv}

        def AddChangable(self, endpoint, reg_name):
            self.tracked_reg_values[reg_name] = atlas_reg_value(endpoint, reg_name)

        def Finalize(self, endpoint, state_db):
            inst = atlas_current_inst(endpoint)
            uid = inst.getUid()
            state_db.AppendInstruction(self.pc, self.opcode, self.dasm, uid)

            for reg_name, prev_val in self.tracked_reg_values.items():
                if reg_name == 'resv_priv':
                    cur_val = atlas_inst_priv(endpoint)
                else:
                    cur_val = atlas_reg_value(endpoint, reg_name)

                if cur_val != prev_val:
                    state_db.AppendRegChange(uid, reg_name, cur_val)

    def OnPreSimulation(self, endpoint):
        for reg_id in range(atlas_num_regs_in_group(endpoint, 0)):
            reg_name = 'x' + str(reg_id)
            reg_val = atlas_reg_value(endpoint, reg_name)
            if isinstance(reg_val, int):
                self.state_db.SetInitRegValue(reg_name, reg_val)

        for reg_id in range(atlas_num_regs_in_group(endpoint, 1)):
            reg_name = 'f' + str(reg_id)
            reg_val = atlas_reg_value(endpoint, reg_name)
            if isinstance(reg_val, int):
                self.state_db.SetInitRegValue(reg_name, reg_val)

        for reg_id in range(atlas_num_regs_in_group(endpoint, 3)):
            reg_name = atlas_csr_name(endpoint, reg_id)
            if reg_name:
                reg_val = atlas_reg_value(endpoint, reg_name)
                self.state_db.SetInitRegValue(reg_name, reg_val)

        init_priv = atlas_inst_priv(endpoint)
        self.state_db.SetInitRegValue('resv_priv', init_priv)

    def OnPreExecute(self, endpoint):
        inst = atlas_current_inst(endpoint)
        if not inst:
            return

        pc = atlas_pc(endpoint)
        mnemonic = inst.getMnemonic()
        opcode = inst.getOpcode()
        priv = inst.getPrivMode()
        dasm = inst.dasmString()
        rd = inst.getRd()
        uid = inst.getUid()

        inst = self.Instruction(pc, opcode, priv, dasm)
        if rd:
            inst.AddChangable(endpoint, rd.getName())

        self.insts_by_uid[uid] = inst

        # We not only snapshot CSR values on exceptions, but also on
        # select instructions that can change them too.
        if mnemonic in ('ecall', 'ebreak', 'fence', 'fence.i', 'mret', 'sret') or mnemonic.startswith('csrr'):
            self.__AddCsrChangables(endpoint)

        # Add fcsr, frm, and fflags to the list of tracked registers
        # if the instruction is a floating-point instruction.
        inst_type = atlas_inst_type(endpoint)
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
        csr_name = atlas_inst_csr(endpoint)
        if csr_name:
            inst.AddChangable(endpoint, csr_name)

    def OnPreException(self, endpoint):
        self.__AddCsrChangables(endpoint)

    def OnPostExecute(self, endpoint):
        inst = atlas_current_inst(endpoint)
        if not inst:
            return

        uid = inst.getUid()
        msg = 'Executed {} instructions...'.format(uid+1)
        self.msg_queue.put(msg)

        inst = self.insts_by_uid[uid]
        inst.Finalize(endpoint, self.state_db)
        del self.insts_by_uid[uid]

    def OnSimulationStuck(self, endpoint):
        self.infinite_loop_pc = atlas_pc(endpoint)
        self.msg_queue.put('SIM_STUCK:0x{:08x}'.format(self.infinite_loop_pc))

    def OnSimFinished(self, endpoint):
        self.msg_queue.put('SIM_FINISHED')

    def OnSimulationDead(self, pc):
        self.msg_queue.put('SIM_DEAD')

    def __AddCsrChangables(self, endpoint):
        inst = atlas_current_inst(endpoint)
        if not inst:
            return

        uid = inst.getUid()
        inst = self.insts_by_uid[uid]

        csr_names = self.__GetCsrNames(endpoint)
        for csr_name in csr_names:
            inst.AddChangable(endpoint, csr_name)

    def __GetCsrNames(self, endpoint):
        if self.csr_names is not None:
            return self.csr_names

        self.csr_names = []
        for reg_id in range(atlas_num_regs_in_group(endpoint, 3)):
            reg_name = atlas_csr_name(endpoint, reg_id)
            if reg_name:
                self.csr_names.append(reg_name)

        return self.csr_names
