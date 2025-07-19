import os, time
from backend.sim_wrapper import SimWrapper
from backend.sim_api import *
from backend.pegasus_dtypes import *
from backend.c_dtypes import *
from backend.trap import *
from pathlib import Path
import subprocess, sqlite3, tempfile, re

## Base report class (show a report file, show a wx.Window, etc.)
class Report:
    def Show(self):
        pass

## Simple file report.
class FileReport(Report):
    def __init__(self, filename):
        self.filename = filename

    def Show(self):
        os.system('notepad.exe {}'.format(self.filename))

## Base observer class.
class Observer:
    def __init__(self):
        self.reg_info_query = None

    def BreakOnPreExecute(self):
        return True

    def BreakOnPreException(self):
        return True

    def BreakOnPostExecute(self):
        return True

    def OnPreSimulation(self, endpoint):
        pass

    def OnPreExecute(self, endpoint):
        pass

    def OnPreException(self, endpoint):
        pass

    def OnPostExecute(self, endpoint):
        pass

    def OnSimulationStuck(self, endpoint):
        pass

    def OnSimFinished(self, endpoint):
        pass

    def OnSimulationDead(self, pc):
        pass

    def CreateReport(self):
        return None

## Simple observer which does not overwrite any PegasusState / PegasusInst values.
class SanityCheckObserver(Observer):
    def __init__(self, logfile):
        self.fout = open(logfile, 'w')
        self.logfile = logfile
        self.prev_rd_vals_by_inst_uid = {}
        self.csrs_before_exception_handling = None

    def OnPreSimulation(self, endpoint):
        starting_pc = pegasus_pc(endpoint)
        self.fout.write('BEGIN SIMULATION (starting pc: {})\n'.format(starting_pc))

    def OnPreExecute(self, endpoint):
        pc = pegasus_pc(endpoint)
        inst = pegasus_current_inst(endpoint)
        dasm = inst.dasmString()
        kvpairs = [('pc', pc), ('dasm', dasm)]

        rs1 = inst.getRs1()
        if rs1:
            # rs1(x7):0xff
            key = 'rs1({})'.format(rs1.getName())
            val = rs1.read()
            kvpairs.append((key, val))

        rs2 = inst.getRs2()
        if rs2:
            # rs2(x7):0xff
            key = 'rs2({})'.format(rs2.getName())
            val = rs2.read()
            kvpairs.append((key, val))

        rd = inst.getRd()
        if rd:
            # rd(x7):0xff
            key = 'rd({})'.format(rd.getName())
            val = rd.read()
            kvpairs.append((key, val))

        imm = inst.getImmediate()
        if imm is not None:
            # imm:0xff
            key = 'imm'
            val = imm
            kvpairs.append((key, val))

        # OnPreExecute (pc: 0xff, dasm: add x0, x0, x0, rs1(x0):0xff, rs2(x0):0xff, rd(x0):0xff)
        kvpairs_str = ', '.join(['{}:{}'.format(k, v) for k, v in kvpairs])
        self.fout.write('--> OnPreExecute ({})\n'.format(kvpairs_str))
        self.csrs_before_exception_handling = None

    def OnPreException(self, endpoint):
        trap_cause = pegasus_inst_active_exception(endpoint)
        assert trap_cause >= 0
        cause = TRAP_CAUSES[trap_cause]

        # ----> OnPreException (cause: ILLEGAL_INSTRUCTION)
        self.fout.write('----> OnPreException (cause: {})\n'.format(cause))

        # Track CSR values before/after exception handling.
        inst = pegasus_current_inst(endpoint)
        if not inst:
            return

        state = inst.getPegasusState()
        csr_rset = state.getCsrRegisterSet()

        self.csrs_before_exception_handling = {}
        for reg_id in range(csr_rset.getNumRegisters()):
            reg = csr_rset.getRegister(reg_id)
            if reg:
                reg_name = reg.getName()
                reg_value = reg.read()
                self.csrs_before_exception_handling[reg_name] = FormatHex(reg_value)

    def OnPostExecute(self, endpoint):
        inst = pegasus_current_inst(endpoint)
        if not inst:
            return

        uid = inst.getUid()

        kvpairs = []
        if uid in self.prev_rd_vals_by_inst_uid:
            rd_before = self.prev_rd_vals_by_inst_uid[uid]
            rd_after = inst.getRd().read()
            rd_before_hex = '0x{:08x}'.format(rd_before)
            rd_after_hex = '0x{:08x}'.format(rd_after)
            rd_name = inst.getRd().getName()

            # rd(x7):0xff -> 0x100
            key = 'rd({})'.format(rd_name)
            val = '{} -> {}'.format(rd_before_hex, rd_after_hex)
            kvpairs.append((key, val))

        if self.csrs_before_exception_handling is not None:
            for csr_name, before_val in self.csrs_before_exception_handling.items():
                after_val = pegasus_reg_value(endpoint, csr_name)
                if before_val != after_val:
                    # mstatus:0x0 -> 0x8
                    key = csr_name
                    val = '{} -> {}'.format(before_val, after_val)
                    kvpairs.append((key, val))

        # ----> OnPostExecute (rd(x7):0xff -> 0x100, mstatus:0x0 -> 0x8)
        kvpairs_str = ', '.join(['{}:{}'.format(k, v) for k, v in kvpairs])
        self.fout.write('----> OnPostExecute ({})\n'.format(kvpairs_str))
        self.csrs_before_exception_handling = None

    def OnSimulationStuck(self, endpoint):
        pc = pegasus_pc(endpoint)
        self.fout.write('----> Infinite loop detected at pc: {}\n'.format(pc))

    def OnSimFinished(self, endpoint):
        workload_exit_code = pegasus_exit_code(endpoint)
        test_passed = pegasus_test_passed(endpoint)
        inst_count = pegasus_inst_count(endpoint)

        # END SIMULATION RESULTS:
        # ----> workload_exit_code: 0
        # ----> test_passed: True
        # ----> inst_count: 100
        self.fout.write('END SIMULATION RESULTS:\n')
        self.fout.write('----> workload_exit_code: {}\n'.format(workload_exit_code))
        self.fout.write('----> test_passed: {}\n'.format(test_passed))
        self.fout.write('----> inst_count: {}\n'.format(inst_count))

    def OnSimulationDead(self, pc):
        self.fout.write('----> Simulation dead at pc: 0x{:08x}\n'.format(pc))

    def CreateReport(self):
        self.close()
        return FileReport(self.logfile)

    def close(self):
        if self.fout:
            self.fout.close()
            self.fout = None

    def __del__(self):
        self.close()

## Utility class which runs an observer on an Pegasus simulation running in the background.
class ObserverSim:
    def __init__(self, riscv_tests_dir, sim_exe_path, test_name):
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path
        self.test_name = test_name

        self.reg_info_query = None
        spike_program_path = Path(__file__).parent.parent.parent / 'spike' / 'build' / 'spike'
        spike_commit_log = '.spike_commit_log'

        if os.path.isfile(spike_program_path):
            if os.path.isfile(spike_commit_log):
                os.remove(spike_commit_log)

            with open(spike_commit_log, 'w') as f:
                process = subprocess.Popen(
                    [spike_program_path, '--log-commits', os.path.join(riscv_tests_dir, test_name)],
                    stdout=subprocess.PIPE,
                    stderr=f,
                    text=True
                )

                process.communicate()
                if os.path.isfile(spike_commit_log):
                    self.reg_info_query = RegInfoQuery(spike_commit_log)

    def RunObserver(self, obs, timeout=None):
        obs.reg_info_query = self.reg_info_query

        # You can ping the simulation as long as the SimWrapper is in scope.
        # See all the available APIs in IDE.backend.sim_api
        with SimWrapper(self.riscv_tests_dir, self.sim_exe_path, self.test_name) as sim:
            obs.OnPreSimulation(sim.endpoint)

            # Always put a pre_execute breakpoint so we can skip an instruction and
            # go right to simulation finish in the event of an infinite loop.
            pegasus_break_action(sim.endpoint, 'pre_execute')

            if obs.BreakOnPreException():
                pegasus_break_action(sim.endpoint, 'pre_exception')

            if obs.BreakOnPostExecute():
                pegasus_break_action(sim.endpoint, 'post_execute')

            # Give the observer a chance to do something at the end of simulation.
            pegasus_break_action(sim.endpoint, 'sim_finish')

            # Look at the pre_execute PC and skip the instruction to terminate
            # the simulation if the same PC is executing twice (about to go into
            # an infinite loop).
            last_pre_execute_pc = -1

            def NOP(endpoint):
                pass

            pre_execute_callback   = obs.OnPreExecute if obs.BreakOnPreExecute() else NOP
            pre_exception_callback = obs.OnPreException if obs.BreakOnPreException() else NOP
            post_execute_callback  = obs.OnPostExecute if obs.BreakOnPostExecute() else NOP
            sim_finished_callback  = obs.OnSimFinished
            sim_dead_callback      = obs.OnSimulationDead

            callbacks = {
                'pre_execute':   pre_execute_callback,
                'pre_exception': pre_exception_callback,
                'post_execute':  post_execute_callback,
                'sim_finished':  sim_finished_callback,
                'sim_dead':      sim_dead_callback
            }

            def GetCallback(action):
                return callbacks.get(action, NOP)

            def GetCallbackArg(action):
                return last_pre_execute_pc if action == 'sim_dead' else sim.endpoint

            start_time = time.time()

            while True:
                current_action = pegasus_continue(sim.endpoint)

                if current_action == 'pre_execute':
                    curr_pre_execute_pc = pegasus_pc(sim.endpoint)
                    if curr_pre_execute_pc == last_pre_execute_pc:
                        obs.OnSimulationStuck(sim.endpoint)
                        current_action = pegasus_kill_sim(sim.endpoint, 555)
                    else:
                        last_pre_execute_pc = curr_pre_execute_pc

                GetCallback(current_action)(GetCallbackArg(current_action))
                if current_action in ('sim_dead', 'sim_finished'):
                    break

                if timeout and time.time() - start_time > timeout:
                    obs.OnSimulationStuck(sim.endpoint)
                    pegasus_kill_sim(sim.endpoint, 555)
                    break

        return obs.CreateReport()

class RegInfoQuery:
    def __init__(self, commit_log):
        # Open a sqlite3 database in the tempdir
        self.db_path = tempfile.mktemp()
        self.conn = sqlite3.connect(self.db_path)
        self.cursor = self.conn.cursor()

        class Transaction:
            def __init__(self, cursor):
                self.cursor = cursor

            def __enter__(self):
                self.cursor.execute('BEGIN TRANSACTION')

            def __exit__(self, type, value, traceback):
                if type is None:
                    self.cursor.execute('COMMIT')
                else:
                    self.cursor.execute('ROLLBACK')

        self.cursor.execute('''
            CREATE TABLE RegisterChanges (
                Hart INT,
                Priv INT,
                PC TEXT,
                RegName TEXT,
                RegVal TEXT
            )
        ''')

        # Create index on the (PC,RegName) columns for faster lookups
        self.cursor.execute('CREATE INDEX idx_pc_regname ON RegisterChanges (PC, RegName)')

        # Log file format:
        #
        # core   0: 1 0x0000000080000288 (0x140525f3) x11 0x000000000bad0000 c320_sscratch 0x000000000badbeef
        #        |  | |                   |           |   |                  |             |
        #        |  | |                   |           |   |                  |             +--------> regvalue
        #        |  | |                   |           |   |                  +----------------------> regname
        #        |  | |                   |           |   +-----------------------------------------> regvalue
        #        |  | |                   |           +---------------------------------------------> regname
        #        |  | |                   +---------------------------------------------------------> (opcode, ignored)
        #        |  | +-----------------------------------------------------------------------------> PC
        #        |  +-------------------------------------------------------------------------------> Priv
        #        +----------------------------------------------------------------------------------> Hart
        #
        # We want to extract the hart (0), priv (1), PC (0x0000000080000288)
        # We also can ignore the opcode (0x140525f3)
        # We want to extract the remaining <regname> <regval> pairs if they exist.
        self.pattern = re.compile(r'core\s+(\d+):\s+(\d+)\s+(0x[0-9a-fA-F]+)\s+\(0x[0-9a-fA-F]+\)\s+(.*)')

        # Since there is no telling how many instructions are in the log, we will
        # have to loop over one by one and insert the values into the sqlite3 database.
        with Transaction(self.cursor):
            with open(commit_log, 'r') as f:
                for line in f.readlines():
                    self.__ParseCommitLogLine(line)

    def __ParseCommitLogLine(self, line):
        match = self.pattern.match(line)
        if not match:
            return

        hart, priv, pc, regvals = match.groups()
        pc = FormatHex(pc)

        if not regvals:
            cmd = 'INSERT INTO RegisterChanges (Hart, Priv, PC) VALUES ({}, {}, "{}")'
            cmd = cmd.format(hart, priv, pc)
            self.cursor.execute(cmd)
            return

        hart = int(hart)
        priv = int(priv)

        # regvals is of the form:
        # x11 0x000000000bad0000 c320_sscratch 0x000000000badbeef
        #
        # We want to extract the register name and value pairs.
        regvals = regvals.split()
        for i in range(0, len(regvals)-1, 2):
            regname = regvals[i]
            regval = regvals[i+1]
            regval = FormatHex(regval)

            if regname.startswith('c'):
                # Extract 'sscratch' from 'c320_sscratch' etc.
                regname = regname.split('_')[1]
            else:
                # Assert it is an INT/FP/VEC register, or a memory write
                assert regname[0] in ('x', 'f', 'v') or regname == 'mem'

            cmd = 'INSERT INTO RegisterChanges VALUES ({}, {}, "{}", "{}", "{}")'
            cmd = cmd.format(hart, priv, pc, regname, regval)
            self.cursor.execute(cmd)

    class RegChange:
        def __init__(self, regname, regvalue):
            self.regname = regname
            self.regvalue = FormatHex(regvalue)

    # {'pc': 0xPC', 'priv': 3, 'reg_changes': [RegChange, RegChange, ...]}
    def GetRegisterInfoAtPC(self, pc, hart=0):
        pc = FormatHex(pc)
        cmd = 'SELECT RegName,RegVal FROM RegisterChanges WHERE PC="{}" AND Hart={}'.format(pc, hart)
        self.cursor.execute(cmd)

        rows = self.cursor.fetchall()
        if len(rows) == 0:
            return {'pc': pc, 'priv': None, 'reg_changes': []}

        reg_changes = []
        for regname, regval in rows:
            reg_changes.append(self.RegChange(regname, regval))

        return {'pc': pc, 'priv': rows[0][1], 'reg_changes': reg_changes}
