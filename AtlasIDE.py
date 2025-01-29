import os, sys, wx, sqlite3, re, copy, json
import wx.grid
import wx.py.shell
from enum import IntEnum

cause_strs = [
    'MISSALIGNED_FETCH',
    'FETCH_ACCESS',
    'ILLEGAL_INSTRUCTION',
    'BREAKPOINT',
    'MISSALIGNED_LOAD',
    'LOAD_ACCESS',
    'MISSALIGNED_STORE',
    'STORE_ACCESS',
    'USER_ECALL',
    'SUPERVISOR_ECALL',
    'VIRTUAL_SUPERVISOR_ECALL',
    'MACHINE_ECALL',
    'FETCH_PAGE_FAULT',
    'LOAD_PAGE_FAULT',
    'STORE_PAGE_FAULT',
    'DOUBLE_TRAP',
    'SOFTWARE_CHECK_FAULT',
    'HARDWARE_ERROR_FAULT',
    'FETCH_GUEST_PAGE_FAULT',
    'LOAD_GUEST_PAGE_FAULT',
    'VIRTUAL_INSTRUCTION',
    'STORE_GUEST_PAGE_FAULT'
]

class InstReplayer:
    def CreateRegsReplayer(self, wdb, test_id, pc):
        return InstReplayer.RegsReplayer(wdb, test_id, pc)

    @staticmethod
    def GetRegNames(wdb, test_id):
        cmd = 'SELECT RegName FROM Registers WHERE TestId={} AND HartId=0'.format(test_id)
        wdb.cursor.execute(cmd)
        return [row[0] for row in wdb.cursor.fetchall()]

    @staticmethod
    def Replay(wdb, test_id, pc, registers_table):
        cmd = 'SELECT RegName,ActualInitVal FROM Registers WHERE TestId={} AND HartId=0'.format(test_id)
        wdb.cursor.execute(cmd)

        for reg_name, actual_init_val in wdb.cursor.fetchall():
            registers_table[reg_name] = actual_init_val

        cmd = 'SELECT PC,Rd,RdValAfter,ResultCode FROM Instructions WHERE PC<={} AND TestId={} AND HartId=0 ORDER BY PC ASC'.format(pc, test_id)
        wdb.cursor.execute(cmd)

        did_reach_end = False
        for inst_pc, rd, rd_val, result_code in wdb.cursor.fetchall():
            did_reach_end = inst_pc == pc

            # Update the register table for successful instructions.
            if result_code == 0 and rd:
                registers_table[rd] = rd_val

            # Update the CSRs after exceptions.
            if result_code >> 16 == 0x1:
                cmd = 'SELECT CsrName,AtlasCsrVal FROM PostInstCsrVals WHERE PC={} AND TestId={} AND HartId=0'.format(inst_pc, test_id)
                wdb.cursor.execute(cmd)

                for csr_name, csr_val in wdb.cursor.fetchall():
                    registers_table[csr_name] = csr_val

            # Do not advance on any inst failure.
            #  0x2: PC INVALID
            #  0x3: REG VAL INVALID
            #  0x5: UNIMPLEMENTED (but passing anyway)
            if result_code >> 16 in (0x2,0x3,0x5):
                break

        if not did_reach_end:
            raise ValueError(f"Could not replay the instructions all the way to PC {pc}!")

    class RegsReplayer:
        def __init__(self, wdb, test_id, pc):
            self.wdb = wdb
            self.test_id = test_id
            self.pc = pc

            self.reg_names = []
            for reg_name in InstReplayer.GetRegNames(wdb, test_id):
                self.reg_names.append(reg_name)

            self.reg_names.sort()

        def __getitem__(self, reg_name):
            if reg_name not in self.reg_names:
                raise AttributeError(f"Register {reg_name} not found")

            registers_table = {}
            InstReplayer.Replay(self.wdb, self.test_id, self.pc, registers_table)
            return registers_table[reg_name]

        @property
        def names(self):
            return copy.deepcopy(self.reg_names)

        @property
        def csrs(self):
            csr_names = []
            for reg_name in self.reg_names:
                if reg_name[0] in ('x', 'f', 'v'):
                    rest = reg_name[1:]
                    try:
                        csr_num = int(rest)
                        # If we got here, it is not a CSR
                        continue
                    except:
                        csr_names.append(reg_name)
                else:
                    csr_names.append(reg_name)

            return csr_names

inst_replayer = InstReplayer()
GROUP_NUMS_BY_REG_NAME = {}
REG_IDS_BY_REG_NAME = {}

class AtlasIDE(wx.Frame):
    def __init__(self, wdb_file):
        wx.Frame.__init__(self, None, -1, "Atlas IDE", size=wx.DisplaySize())
        self.wdb = WorkloadsDB(wdb_file)

        # Create a notebook with two tabs: "Test Debugger" and "Test Results"
        self.notebook = wx.Notebook(self, -1, style=wx.NB_TOP)
        self.test_debugger = TestDebugger(self.notebook, self.wdb)
        self.test_results = TestResults(self.notebook, self.wdb)
        self.notebook.AddPage(self.test_debugger, "Test Debugger")
        self.notebook.AddPage(self.test_results, "Test Results")

class AtlasPanel(wx.Panel):
    def __init__(self, *args, **kwargs):
        wx.Panel.__init__(self, *args, **kwargs)
        self.frame = self.GetTopLevelParent()

class TestDebugger(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.test_selector = TestSelector(self, wdb)
        self.test_viewer = TestViewer(self, wdb)

        self.sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer.Add(self.test_selector, 0, wx.EXPAND)
        self.sizer.Add(wx.StaticLine(self, -1, style=wx.LI_VERTICAL), 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 5)
        self.sizer.Add(self.test_viewer, 1, wx.EXPAND)
        self.SetSizer(self.sizer)

    def LoadTest(self, test_name):
        self.test_viewer.LoadTest(test_name)

class TestSelector(wx.TreeCtrl):
    def __init__(self, parent, wdb):
        wx.TreeCtrl.__init__(self, parent, -1, style=wx.TR_HIDE_ROOT|wx.TR_HAS_BUTTONS|wx.NO_BORDER, size=(240, parent.GetSize().GetHeight()))
        self.wdb = wdb

        self.test_debugger = parent
        self.root = self.AddRoot("Root")
        self.passing_root = self.AppendItem(self.root, "Passing Tests")
        self.failing_root = self.AppendItem(self.root, "Failing Tests")

        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.__OnSelChanged)

        for test_name in self.wdb.GetPassingTestNames():
            self.AppendItem(self.passing_root, test_name)

        for test_name in self.wdb.GetFailingTestNames():
            self.AppendItem(self.failing_root, test_name)

        self.Collapse(self.passing_root)
        self.Expand(self.failing_root)

    def __OnSelChanged(self, evt):
        item = evt.GetItem()
        if item and item.IsOk():
            if item == self.passing_root or item == self.failing_root:
                return

            test = self.GetItemText(item)
            self.test_debugger.LoadTest(test)

class TestViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb
        self.top_splitter = wx.SplitterWindow(self)
        self.top_splitter.SetMinimumPaneSize(50)
        self.top_panel = wx.Panel(self.top_splitter, -1)

        self.initial_diffs_viewer = InitialDiffsViewer(self.top_panel, wdb)
        self.inst_viewer = InstructionViewer(self.top_panel, wdb)
        self.post_exception_csr_vals_viewer = PostExceptionCsrValsViewer(self.top_panel, wdb)

        self.initial_diffs_viewer.Hide()
        self.inst_viewer.Hide()
        self.post_exception_csr_vals_viewer.Hide()

        self.inst_list_panel = InstructionListPanel(self.top_panel)
        self.python_terminal = PythonTerminal(self.top_splitter)

        top_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.top_panel.SetSizer(top_panel_sizer)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def LoadTest(self, test_name):
        self.frame.SetTitle(test_name)
        self.initial_diffs_viewer.OnLoadTest(test_name)
        self.inst_viewer.OnLoadTest(test_name)
        self.post_exception_csr_vals_viewer.OnLoadTest(test_name)
        self.python_terminal.OnLoadTest(test_name)
        self.inst_list_panel.OnLoadTest(test_name)

    def ShowInitialState(self):
        self.sizer.Clear()
        self.initial_diffs_viewer.Show()
        self.inst_viewer.Hide()
        self.post_exception_csr_vals_viewer.Hide()

        top_panel_sizer = self.top_panel.GetSizer()
        top_panel_sizer.Clear()
        top_panel_sizer.Add(self.initial_diffs_viewer, 1, wx.EXPAND)
        top_panel_sizer.AddStretchSpacer(1)
        top_panel_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer.Clear()
        self.top_splitter.SplitHorizontally(self.top_panel, self.python_terminal)
        self.sizer.Add(self.top_splitter, 1, wx.EXPAND)

        self.top_splitter.Bind(wx.EVT_SPLITTER_DCLICK, lambda evt: None)
        self.top_splitter.SetSashPosition(int(0.75 * self.GetSize().GetHeight()))

        self.Layout()

    def ShowInstruction(self, pc):
        self.sizer.Clear()
        self.initial_diffs_viewer.Hide()
        self.inst_viewer.Show()
        self.post_exception_csr_vals_viewer.Hide()

        top_panel_sizer = self.top_panel.GetSizer()
        top_panel_sizer.Clear()
        top_panel_sizer.Add(self.inst_viewer, 1, wx.EXPAND)
        top_panel_sizer.AddStretchSpacer(1)
        top_panel_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer.Clear()
        self.top_splitter.SplitHorizontally(self.top_panel, self.python_terminal)
        self.sizer.Add(self.top_splitter, 1, wx.EXPAND)

        self.inst_viewer.ShowInstruction(pc)
        self.python_terminal.ShowInstruction(pc)

        self.top_splitter.Bind(wx.EVT_SPLITTER_DCLICK, lambda evt: None)
        self.top_splitter.SetSashPosition(int(0.75 * self.GetSize().GetHeight()))

        self.Layout()

    def ShowCsrValues(self, pc):
        self.sizer.Clear()
        self.initial_diffs_viewer.Hide()
        self.inst_viewer.Hide()
        self.post_exception_csr_vals_viewer.Show()

        top_panel_sizer = self.top_panel.GetSizer()
        top_panel_sizer.Clear()
        top_panel_sizer.Add(self.post_exception_csr_vals_viewer, 1, wx.EXPAND)
        top_panel_sizer.AddStretchSpacer(1)
        top_panel_sizer.Add(self.inst_list_panel, 0, wx.EXPAND)

        self.sizer.Clear()
        self.top_splitter.SplitHorizontally(self.top_panel, self.python_terminal)
        self.sizer.Add(self.top_splitter, 1, wx.EXPAND)

        self.post_exception_csr_vals_viewer.ShowInstruction(pc)
        self.python_terminal.ShowInstruction(pc)

        self.top_splitter.Bind(wx.EVT_SPLITTER_DCLICK, lambda evt: None)
        self.top_splitter.SetSashPosition(int(0.75 * self.GetSize().GetHeight()))

        self.Layout()

class TestResults(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.help = wx.StaticText(self, -1, "Test Results")
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.SetSizer(self.sizer)

class InitialDiffsViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb
        self.grid = None

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def OnLoadTest(self, test_name):
        if self.grid:
            self.sizer.Clear()
            self.grid.Destroy()
            self.grid = None

        self.grid = wx.grid.Grid(self, -1)
        test_id = self.wdb.GetTestId(test_name)

        # Algo:
        #  1. Go through all the appropriate rows in the Registers table.
        #  2. For each register, get its initial value and expected value.
        #  3. Append the row info and use the "actual!=expected" test to set the background color.

        # diffs.append({'group_num': group_num, 'reg_idx': reg_idx, 'expected_val': expected_val, 'actual_val': actual_val})
        init_reg_diffs = self.wdb.GetInitialDiffs(test_id)

        # reg_vals.append({'reg_type': reg_type, 'reg_idx': reg_idx, 'init_val': init_val})
        init_reg_vals = self.wdb.GetInitialRegVals(test_id)

        row_content = []
        for reg_val in init_reg_vals:
            group_num = reg_val['reg_type']
            reg_idx = reg_val['reg_idx']
            actual_val = reg_val['init_val']
            expected_val = actual_val

            for diff in init_reg_diffs:
                if diff['group_num'] == group_num and diff['reg_idx'] == reg_idx:
                    expected_val = diff['expected_val']
                    break

            reg_name = self.wdb.GetRegisterName(group_num, reg_idx)
            row_content.append({'reg_name': reg_name, 'expected_val': expected_val, 'actual_val': actual_val})

        self.grid.CreateGrid(len(row_content), 3)
        self.grid.SetColLabelValue(0, "Register")
        self.grid.SetColLabelValue(1, "Expected Value")
        self.grid.SetColLabelValue(2, "Actual Value")

        for row, content in enumerate(row_content):
            self.grid.SetCellValue(row, 0, content['reg_name'])
            self.grid.SetCellValue(row, 1, hex(content['expected_val'] & 0xFFFFFFFFFFFFFFFF))
            self.grid.SetCellValue(row, 2, hex(content['actual_val'] & 0xFFFFFFFFFFFFFFFF))

            if content['expected_val'] != content['actual_val']:
                for col in range(3):
                    self.grid.SetCellBackgroundColour(row, col, 'pink')

        self.sizer.Add(self.grid, 1, wx.EXPAND)
        self.Layout()
        self.grid.AutoSize()

class InstructionViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb

        self.inst_info_panel = InstructionInfoPanel(self)
        self.register_info_panel = RegisterInfoPanel(self)
        self.helpers_code_panel = HelpersCodePanel(self)
        self.atlas_experimental_code_panel = AtlasExperimentalCodePanel(self)
        self.atlas_cpp_code_panel = AtlasCppCodePanel(self)

        info_sizer = wx.BoxSizer(wx.HORIZONTAL)
        info_sizer.Add(self.inst_info_panel, 0, wx.EXPAND)
        info_sizer.Add(self.register_info_panel, 0, wx.EXPAND|wx.LEFT, 20)

        grid_sizer = wx.FlexGridSizer(2, 2, 30, 30)
        grid_sizer.Add(info_sizer, 1, wx.EXPAND)
        grid_sizer.Add(self.helpers_code_panel, 1, wx.EXPAND)
        grid_sizer.Add(self.atlas_experimental_code_panel, 1, wx.EXPAND)
        grid_sizer.Add(self.atlas_cpp_code_panel, 1, wx.EXPAND)

        self.SetSizer(grid_sizer)
        self.Layout()

    def OnLoadTest(self, test_name):
        self.inst_info_panel.OnLoadTest(test_name)
        self.register_info_panel.OnLoadTest(test_name)
        self.helpers_code_panel.OnLoadTest(test_name)
        self.atlas_experimental_code_panel.OnLoadTest(test_name)
        self.atlas_cpp_code_panel.OnLoadTest(test_name)

    def ShowInstruction(self, pc):
        self.inst_info_panel.ShowInstruction(pc)
        self.register_info_panel.ShowInstruction(pc)
        self.helpers_code_panel.ShowInstruction(pc)
        self.atlas_experimental_code_panel.ShowInstruction(pc)
        self.atlas_cpp_code_panel.ShowInstruction(pc)

class PostExceptionCsrValsViewer(AtlasPanel):
    def __init__(self, parent, wdb):
        AtlasPanel.__init__(self, parent, -1)
        self.wdb = wdb
        self.grid = None

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def OnLoadTest(self, test_name):
        self.test_id = self.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        if self.grid:
            self.sizer.Clear()
            self.grid.Destroy()
            self.grid = None

        cmd = 'SELECT CsrName,AtlasCsrVal,CosimCsrVal FROM PostInstCsrVals WHERE PC={} AND TestId={} AND HartId=0 ORDER BY CsrName ASC'.format(pc, self.test_id)
        self.wdb.cursor.execute(cmd)

        rows = len(self.wdb.cursor.fetchall())
        cols = 3

        self.grid = wx.grid.Grid(self, -1)
        self.grid.CreateGrid(rows, cols)

        self.grid.SetColLabelValue(0, "CSR")
        self.grid.SetColLabelValue(1, "Expected Value")
        self.grid.SetColLabelValue(2, "Actual Value")

        self.wdb.cursor.execute(cmd)
        for idx, (csr_name, atlas_val, cosim_val) in enumerate(self.wdb.cursor.fetchall()):
            self.grid.SetCellValue(idx, 0, csr_name)
            self.grid.SetCellValue(idx, 1, hex(atlas_val & 0xFFFFFFFFFFFFFFFF))
            self.grid.SetCellValue(idx, 2, hex(cosim_val & 0xFFFFFFFFFFFFFFFF))

            if atlas_val != cosim_val:
                for col in range(cols):
                    self.grid.SetCellBackgroundColour(idx, col, 'pink')

        self.sizer.Add(self.grid, 1, wx.EXPAND)
        self.Layout()
        self.grid.AutoSize()

class InstructionInfoPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction Info")
        self.help.SetFont(mono12bold)

        self.pc_label = wx.StaticText(self, -1, "PC:")
        self.pc_label.SetFont(mono10)

        self.pc = wx.StaticText(self, -1, "")
        self.pc.SetFont(mono10)

        self.mnemonic_label = wx.StaticText(self, -1, "Inst:")
        self.mnemonic_label.SetFont(mono10)

        self.mnemonic = wx.StaticText(self, -1, "")
        self.mnemonic.SetFont(mono10)

        self.opcode_label = wx.StaticText(self, -1, "Opcode:")
        self.opcode_label.SetFont(mono10)

        self.opcode = wx.StaticText(self, -1, "")
        self.opcode.SetFont(mono10)

        self.priv_label = wx.StaticText(self, -1, "Priv:")
        self.priv_label.SetFont(mono10)

        self.priv = wx.StaticText(self, -1, "")
        self.priv.SetFont(mono10)

        self.result_label = wx.StaticText(self, -1, "Result:")
        self.result_label.SetFont(mono10)

        self.result = wx.StaticText(self, -1, "")
        self.result.SetFont(mono10)

        self.__ClearInfoPanel()

        # Use a grid sizer to lay out the instruction fields
        gridsizer = wx.FlexGridSizer(0, 2, 5, 5)
        gridsizer.AddGrowableCol(1)
        gridsizer.Add(self.pc_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.pc, 1, wx.EXPAND)
        gridsizer.Add(self.mnemonic_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.mnemonic, 1, wx.EXPAND)
        gridsizer.Add(self.opcode_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.opcode, 1, wx.EXPAND)
        gridsizer.Add(self.priv_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.priv, 1, wx.EXPAND)
        gridsizer.Add(self.result_label, 0, wx.ALIGN_LEFT)
        gridsizer.Add(self.result, 1, wx.EXPAND)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(gridsizer, 0, wx.EXPAND)

        self.SetSizer(self.sizer)
        self.Layout()

    def OnLoadTest(self, test_name):
        self.__ClearInfoPanel()
        self.test_id = self.frame.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        self.pc.SetLabel('')
        self.mnemonic.SetLabel('')
        self.opcode.SetLabel('')
        self.priv.SetLabel('')
        self.result.SetLabel('')

        cmd = 'SELECT Mnemonic,Opcode,Priv,ResultCode FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        mnemonic, opcode, priv, result_code = self.frame.wdb.cursor.fetchone()

        self.pc.SetLabel("0x{:08X}".format(pc))
        self.mnemonic.SetLabel(mnemonic)
        self.opcode.SetLabel("0x{:08X}".format(opcode))
        self.priv.SetLabel(["U({})", "S({})", "H({})", "M({})", "VU({})", "VS({})"][priv].format(priv))

        if result_code == 0:
            self.result.SetLabel('OKAY')
        elif result_code >> 16 == 0x1:
            exception_cause = result_code & 0xFFFF
            self.result.SetLabel(cause_strs[exception_cause])
        elif result_code >> 16 == 0x2:
            self.result.SetLabel('PC INVALID')
        elif result_code >> 16 == 0x3:
            self.result.SetLabel('REG VAL INVALID')
        elif result_code >> 16 == 0x4:
            self.result.SetLabel('UNIMPLEMENTED')

        self.Update()
        self.Layout()

    def __ClearInfoPanel(self):
        self.pc.SetLabel("")
        self.mnemonic.SetLabel("")
        self.opcode.SetLabel("")
        self.priv.SetLabel("")
        self.result.SetLabel("")

class RegisterInfoPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Register Info")
        self.help.SetFont(mono12bold)

        self.reg_info_text = wx.StaticText(self, -1, "")
        self.reg_info_text.SetFont(mono10)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.reg_info_text, 1, wx.EXPAND)

        self.SetSizer(self.sizer)
        self.Layout()

    def OnLoadTest(self, test_name):
        self.test_id = self.frame.wdb.GetTestId(test_name)
        self.reg_info_text.SetLabel('')

    def ShowInstruction(self, pc):
        self.reg_info_text.SetLabel('')

        cmd = 'SELECT Rs1,Rs1Val,Rs2,Rs2Val,Rd,RdValBefore,RdValAfter,TruthRdValAfter,HasImm,Imm,Disasm FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)

        row = self.frame.wdb.cursor.fetchone()
        rs1, rs1_val, rs2, rs2_val, rd, rd_val_before, rd_val_after, truth_rd_val_after, has_imm, imm, disasm = row

        # Example:
        #
        # rs1: x7        0xefefefef
        # rs2: x10       0x12345678
        # rd:  x5
        #      before:   0xdeadbeef
        #      after:    0xbaadf00d
        #      expected: 0xbaadf00d

        # Example:
        #
        # rs1: x7        0xefefefef
        # imm:           0x1234
        # rd:  x7
        #      before:   0xdeadbeef
        #      after:    0xbaadf00d
        #      expected: 0xdeadbeef

        lines = []
        if rs1 not in (None, ''):
            lines.append(['rs1:', rs1, f"0x{rs1_val:016X}"])

        if rs2 not in (None, ''):
            lines.append(['rs2:', rs2, f"0x{rs2_val:016X}"])

        if has_imm:
            lines.append(['imm:', '', f"0x{imm:016X}"])

        if disasm.find('CSR=') != -1:
            csr_num = int(re.search(r'CSR=0x([0-9a-fA-F]+)', disasm).group(1), 16)
            lines.append(['csr:', str(csr_num), ''])

        if rd not in (None, ''):
            lines.append(['rd:', rd, ''])
            lines.append(['', 'before:', f"0x{rd_val_before:016X}"])
            lines.append(['', 'after:', f"0x{rd_val_after:016X}"])
            lines.append(['', 'expected:', f"0x{truth_rd_val_after:016X}"])

        text = ''
        if len(lines):
            max_col0_len = max(len(line[0]) for line in lines)
            max_col1_len = max(len(line[1]) for line in lines)
            max_col2_len = max(len(line[2]) for line in lines)
            for line in lines:
                text += line[0].ljust(max_col0_len) + ' ' + line[1].ljust(max_col1_len) + ' ' + line[2].ljust(max_col2_len) + '\n'

        self.reg_info_text.SetLabel(text)

class InstructionListPanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Instruction List")
        self.help.SetFont(mono12bold)

        self.inst_list_ctrl = wx.ListCtrl(self, -1, style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_NO_HEADER|wx.LC_HRULES)
        self.inst_list_ctrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__OnItemSelected)
        self.inst_list_ctrl.InsertColumn(0, "Instruction", width=200)
        self.inst_list_ctrl.SetFont(mono10)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(self.sizer)

        self.SetMinSize((-1, self.frame.GetSize().GetHeight()-150))
        self.Layout()

    def OnLoadTest(self, test_name, hart_id=0):
        self.inst_list_ctrl.DeleteAllItems()
        self.inst_list_ctrl.InsertItem(0, "")
        self.inst_list_ctrl.SetItem(0, 0, "Initial State")

        # Add the remaining rows: colored square in the first column, and the disassembled instruction in the second column.
        test_id = self.frame.wdb.GetTestId(test_name)
        cmd = 'SELECT Disasm,PC,ResultCode FROM Instructions WHERE TestId={} AND HartId={} ORDER BY PC ASC'.format(test_id, hart_id)
        cursor = self.frame.wdb.cursor
        cursor.execute(cmd)
        idx = 0
        for disasm, pc, result_code in cursor.fetchall():
            if result_code == 0:
                status = InstStatus.PASS
            elif result_code >> 16 == 0x1:
                cmd = 'SELECT * FROM PostInstCsrVals WHERE PC={} AND TestId={} AND HartId={} AND AtlasCsrVal != CosimCsrVal'.format(pc, test_id, hart_id)
                cursor.execute(cmd)
                failed = False
                for row in cursor.fetchall():
                    failed = True
                    break

                if failed:
                    status = InstStatus.EXCEPTION_FAILING
                else:
                    status = InstStatus.EXCEPTION_PASSING
            elif result_code >> 16 == 0x2:
                status = InstStatus.MISMATCH_PC
            elif result_code >> 16 == 0x3:
                status = InstStatus.MISMATCH_REG_VAL
            elif result_code >> 16 == 0x4:
                status = InstStatus.UNIMPL_PASSING
            elif result_code >> 16 == 0x5:
                status = InstStatus.UNIMPL_FAILING
            else:
                status = InstStatus.UNSPECIFIED

            bg_color = GetInstStatusColor(status)
            self.inst_list_ctrl.InsertItem(idx+1, "")
            self.inst_list_ctrl.SetItemBackgroundColour(idx+1, bg_color)
            self.inst_list_ctrl.SetItem(idx+1, 0, disasm.replace('\t', ' '))
            self.inst_list_ctrl.SetItemData(idx+1, pc)
            idx += 1

            # Add another listctrl item in the event of an exception. Clicking this
            # item will show the CSR values after exception handling.
            if result_code >> 16 == 0x1:
                exception_cause = result_code & 0xFFFF
                self.inst_list_ctrl.InsertItem(idx+1, "")
                self.inst_list_ctrl.SetItem(idx+1, 0, '  ' + cause_strs[exception_cause])
                self.inst_list_ctrl.SetItemData(idx+1, pc)

                if status == InstStatus.EXCEPTION_FAILING:
                    bg_color = GetInstStatusColor(InstStatus.EXCEPTION_FAILING)
                    self.inst_list_ctrl.SetItemBackgroundColour(idx+1, bg_color)

                idx += 1

        self.__AlignInstListItems()
        self.inst_list_ctrl.Select(0)

    def __OnItemSelected(self, evt):
        idx = self.inst_list_ctrl.GetFirstSelected()
        if idx == 0:
            self.frame.test_debugger.test_viewer.ShowInitialState()
        else:
            pc = self.inst_list_ctrl.GetItemData(idx)
            text = self.inst_list_ctrl.GetItemText(idx)
            if text.strip() in cause_strs:
                self.frame.test_debugger.test_viewer.ShowCsrValues(pc)
            else:
                self.frame.test_debugger.test_viewer.ShowInstruction(pc)

        self.__AlignInstListItems()

    def __AlignInstListItems(self):
        mnemonics = set()
        for idx in range(self.inst_list_ctrl.GetItemCount()):
            if idx == 0:
                continue
            mnemonic = self.inst_list_ctrl.GetItemText(idx).split(' ')[0]
            mnemonics.add(mnemonic)

        max_mnemonic_len = max(len(mnemonic) for mnemonic in mnemonics) + 4
        for idx in range(self.inst_list_ctrl.GetItemCount()):
            if idx == 0:
                continue
            disasm = self.inst_list_ctrl.GetItemText(idx)
            mnemonic = disasm.split(' ')[0]
            disasm = disasm.replace(mnemonic, '').lstrip()
            disasm = mnemonic.ljust(max_mnemonic_len) + disasm.replace('  ', ' ')
            self.inst_list_ctrl.SetItem(idx, 0, disasm)

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.Layout()

class HelpersCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Helpers")
        self.help.SetFont(mono12bold)

        self.helpers_text = wx.StaticText(self, -1, "")
        self.helpers_text.SetFont(mono10)

        self.helpers_hardcoded_text = '''// ...macros...
#define sext32(x) ((sreg_t)(int32_t)(x))
#define zext32(x) ((reg_t)(uint32_t)(x))
#define sext(x, pos) (((sreg_t)(x) << (64 - (pos))) >> (64 - (pos)))
#define zext(x, pos) (((reg_t)(x) << (64 - (pos))) >> (64 - (pos)))
#define sext_xlen(x) sext(x, xlen)
#define zext_xlen(x) zext(x, xlen)

// ...code...'''

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.helpers_text, 1, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.Layout()

        self.helper_code_by_mnemonic = {}

    def OnLoadTest(self, test_name):
        self.test_id = self.frame.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        cmd = 'SELECT mnemonic FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        mnemonic = self.frame.wdb.cursor.fetchone()[0]
        if mnemonic not in self.helper_code_by_mnemonic:
            self.helper_code_by_mnemonic[mnemonic] = self.__GetHelperCode(mnemonic)

        self.helpers_text.SetLabel(self.helper_code_by_mnemonic[mnemonic])

    def __GetHelperCode(self, mnemonic):
        atlas_root = os.path.dirname(__file__)
        spike_root = os.path.join(atlas_root, 'spike')
        insns_root = os.path.join(spike_root, 'riscv', 'insns')
        impl_file = os.path.join(insns_root, mnemonic + '.h')

        helper_code = ''
        if os.path.exists(impl_file):
            with open(impl_file, 'r') as f:
                helper_code = f.read()

        return self.helpers_hardcoded_text + '\n' + helper_code

class AtlasExperimentalCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas Experimental Code")
        self.help.SetFont(mono12bold)

        self.editor = wx.TextCtrl(self, -1, style=wx.TE_MULTILINE)
        self.editor.SetFont(mono10)
        self.editor.SetValue('# Implement instruction handlers here in Python!\n\n')
        self.editor.Bind(wx.EVT_TEXT, self.__OnEdits)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.editor, 1, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.Layout()

        self.experimental_code_by_mnemonic = {}
        self.active_mnemonic = None

    def OnLoadTest(self, test_name):
        self.test_id = self.frame.wdb.GetTestId(test_name)
        self.editor.SetValue('')

    def ShowInstruction(self, pc):
        cmd = 'SELECT Disasm FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        disasm = self.frame.wdb.cursor.execute(cmd).fetchone()[0]
        mnemonic = disasm.replace('\t', ' ').split(' ')[0]

        if mnemonic not in self.experimental_code_by_mnemonic:
            self.experimental_code_by_mnemonic[mnemonic] = '# Implement instruction handlers here in Python!\n\n'

        self.editor.ChangeValue(self.experimental_code_by_mnemonic[mnemonic])
        self.active_mnemonic = mnemonic

    def __OnEdits(self, evt):
        self.experimental_code_by_mnemonic[self.active_mnemonic] = self.editor.GetValue()

class AtlasCppCodePanel(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        self.help = wx.StaticText(self, -1, "Atlas C++ Code")
        self.help.SetFont(mono12bold)

        self.atlas_cpp_code_text = wx.StaticText(self, -1, "")
        self.atlas_cpp_code_text.SetFont(mono10)
        self.atlas_cpp_code_by_mnemonic = {}

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.help, 0, wx.EXPAND)
        self.sizer.Add(self.atlas_cpp_code_text, 1, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.Layout()

    def OnLoadTest(self, test_name):
        self.test_id = self.frame.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        cmd = 'SELECT mnemonic,ResultCode FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        mnemonic, result_code = self.frame.wdb.cursor.fetchone()

        if mnemonic not in self.atlas_cpp_code_by_mnemonic:
            self.atlas_cpp_code_by_mnemonic[mnemonic] = self.__GetAtlasCppCode(mnemonic, result_code)

        self.atlas_cpp_code_text.SetLabel(self.atlas_cpp_code_by_mnemonic[mnemonic])

    def __GetAtlasCppCode(self, mnemonic, result_code):
        # First check the result_code to see if this instruction is unimplemented.
        if result_code >> 16 == 0x4:
            return f'// "{mnemonic}" is unimplemented in Atlas.'

        atlas_root = os.path.dirname(__file__)
        inst_handler_root = os.path.join(atlas_root, 'core', 'inst_handlers', 'rv64')
        lookfor = f"ActionGroup* {mnemonic}_64_handler(atlas::AtlasState* state);"

        # find the .hpp file under isnt_handler_root that has this <lookfor> string
        hpp_file = None
        for root, dirs, files in os.walk(inst_handler_root):
            for file in files:
                if file.endswith('.hpp'):
                    with open(os.path.join(root, file), 'r') as f:
                        if lookfor in f.read():
                            hpp_file = os.path.join(root, file)
                            break

        impl_file = hpp_file.replace('.hpp', '.cpp')

        cpp_code = []
        if os.path.exists(impl_file):
            lookfor = f"::{mnemonic}_64_handler(atlas::AtlasState* state)"
            other_mnemonic_lookfor = '_64_handler(atlas::AtlasState* state)'
            copy_line = False
            with open(impl_file, 'r') as f:
                for line in f.readlines():
                    # Stop copying lines when we get to the next function signature
                    if line.find(other_mnemonic_lookfor) != -1 and copy_line:
                        break

                    # Start copying over the code when we get to our exact function signature for this mnemonic
                    if line.find(lookfor) != -1:
                        copy_line = True

                    if copy_line:
                        cpp_code.append(line)

        # Remove 4 whitespaces from the front of each line
        cpp_code = [line[4:] for line in cpp_code]
        return ''.join(cpp_code)

# Function to extract the register value from any register
def GetRegVal(reg):
    if isinstance(reg, PythonImmediate):
        return reg.int
    elif isinstance(reg, int):
        return reg
    else:
        return reg.regval

# Function to simulate a cast to int32_t (signed 32-bit integer)
def int32(x):
    return (x & 0xFFFFFFFF) - (1 << 32) if x & (1 << 31) else x & 0xFFFFFFFF

# Function to simulate a cast to uint32_t (unsigned 32-bit integer)
def uint32(x):
    return x & 0xFFFFFFFF

# Function to simulate a cast to int64_t (signed 64-bit integer)
def int64(x):
    return (x & 0xFFFFFFFFFFFFFFFF) - (1 << 64) if x & (1 << 63) else x & 0xFFFFFFFFFFFFFFFF

# Function to simulate a cast to uint64_t (unsigned 64-bit integer)
def uint64(x):
    return x & 0xFFFFFFFFFFFFFFFF

# Function to sign-extend a 32-bit value (sext32)
def sext32(x):
    reg_name = x.regname
    x = GetRegVal(x)
    x = int64(int32(x))
    return PythonPrettyRegister(reg_name, x)

# Function to zero-extend a 32-bit value (zext32)
def zext32(x):
    reg_name = x.regname
    x = GetRegVal(x)
    x = uint64(uint32(x))
    return PythonPrettyRegister(reg_name, x)

# Function to sign-extend a value by shifting (sext)
def sext(x, pos):
    reg_name = x.regname
    x = GetRegVal(x)
    x = int64(((x) << (64 - pos)) >> (64 - pos))
    return PythonPrettyRegister(reg_name, x)

# Function to zero-extend a value by shifting (zext)
def zext(x, pos):
    reg_name = x.regname
    x = GetRegVal(x)
    x = uint64(((x) << (64 - pos)) >> (64 - pos))
    return PythonPrettyRegister(reg_name, x)

# Function to sign-extend a value for xlen (64 bits)
def sext_xlen(x):
    reg_name = x.regname
    return PythonPrettyRegister(reg_name, GetRegVal(sext(x, 64)))

# Function to zero-extend a value for xlen (64 bits)
def zext_xlen(x):
    reg_name = x.regname
    return PythonPrettyRegister(reg_name, GetRegVal(zext(x, 64)))

class PythonTerminal(AtlasPanel):
    def __init__(self, parent):
        AtlasPanel.__init__(self, parent, -1)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        mono12bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, "Monospace")

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        self.shell = wx.py.shell.Shell(self)
        self.shell.SetFont(mono10)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.shell, 1, wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetMinSize((wx.DisplaySize()[0], 150))
        self.Layout()

    def OnLoadTest(self, test_name):
        self.test_id = self.frame.wdb.GetTestId(test_name)

    def ShowInstruction(self, pc):
        cmd = 'SELECT Rs1,Rs1Val,Rs2,Rs2Val,Rd,RdValBefore,RdValAfter,TruthRdValAfter,HasImm,Imm FROM Instructions WHERE PC={} AND TestId={} AND HartId=0'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)

        row = self.frame.wdb.cursor.fetchone()
        rs1, rs1_val, rs2, rs2_val, rd, rd_val_before, rd_val_after, truth_rd_val_after, has_imm, imm = row

        vars = {}
        if rs1 not in (None, ''):
            vars['rs1'] = PythonSourceRegister(rs1, rs1_val)

        if rs2 not in (None, ''):
            vars['rs2'] = PythonSourceRegister(rs2, rs2_val)

        if has_imm:
            vars['imm'] = PythonImmediate(imm)

        if rd not in (None, ''): 
            vars['rd'] = PythonDestRegister(rd, rd_val_before, rd_val_after, truth_rd_val_after)

        cmd = 'SELECT CsrName,AtlasCsrVal FROM PostInstCsrVals WHERE PC<={} AND TestId={} AND HartId=0 ORDER BY PC ASC'.format(pc, self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        for csr_name, atlas_csr_val in self.frame.wdb.cursor.fetchall():
            vars[csr_name] = PythonPrettyRegister(csr_name, atlas_csr_val)

        cmd = 'SELECT RegName,ActualInitVal FROM Registers WHERE TestId={} AND HartId=0 AND RegType=3'.format(self.test_id)
        self.frame.wdb.cursor.execute(cmd)
        for csr_name, atlas_csr_val in self.frame.wdb.cursor.fetchall():
            if csr_name not in vars:
                vars[csr_name] = PythonPrettyRegister(csr_name, atlas_csr_val)

        vars['whos'] = self.whos
        vars['helpers'] = self.helpers
        vars['sext32'] = sext32
        vars['zext32'] = zext32
        vars['sext'] = sext
        vars['zext'] = zext
        vars['sext_xlen'] = sext_xlen
        vars['zext_xlen'] = zext_xlen
        vars['regs'] = inst_replayer.CreateRegsReplayer(self.frame.wdb, self.test_id, pc)
        vars['pc'] = pc
        self.vars = vars

        if self.shell:
            self.GetSizer().Clear()
            self.shell.destroy()
            self.shell = None

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Monospace")
        self.shell = wx.py.shell.Shell(self, locals=vars)
        self.shell.SetFont(mono10)
        self.sizer.Add(self.shell, 1, wx.EXPAND)
        self.Layout()

    def whos(self):
        return [varname for varname in list(self.vars.keys()) if varname not in ('whos', '__builtins__', 'shell') and varname not in self.helpers()]

    def helpers(self):
        return ['sext32', 'zext32', 'sext', 'zext', 'sext_xlen', 'zext_xlen']

    @property
    def regs(self):
        return inst_replayer.regs

    @property
    def csrs(self):
        return inst_replayer.csrs

class PythonSourceRegister:
    def __init__(self, reg_name, reg_val):
        self.reg_name = reg_name
        self.reg_val = reg_val

    def __repr__(self):
        rep = self.reg_name + '\n'
        rep += '    int val: ' + str(self.reg_val) + '\n'
        rep += '    hex val: ' + f'0x{self.reg_val:016X}' + '\n'
        return rep

    def __str__(self):
        return f'0x{self.reg_val:016X}'

    @property
    def regname(self):
        return self.reg_name

    @property
    def regval(self):
        return self.reg_val

    @property
    def hex(self):
        return f'0x{self.reg_val:016X}'

class MaskedRegister:
    def __init__(self, reg_name):
        self.reg_name = reg_name
        self._mask = None

    @property
    def mask(self):
        if self._mask is None:
            group_num = GROUP_NUMS_BY_REG_NAME[self.reg_name]
            reg_idx = REG_IDS_BY_REG_NAME[self.reg_name]
            if group_num == 0:
                self._mask = 0xFFFFFFFFFFFFFFFF if reg_idx > 0 else 0x0
            elif group_num in (1,2):
                self._mask = 0xFFFFFFFFFFFFFFFF
            else:
                atlas_root = os.path.dirname(__file__)
                with open(os.path.join(atlas_root, 'arch', 'rv64', 'reg_csr.json'), 'r') as fin:
                    all_json = json.load(fin)
                    for reg_json in all_json:
                        if reg_json['name'] == self.reg_name:
                            fields = reg_json['fields']
                            bit_mask = 0
                            for _, field_defn in fields.items():
                                if not field_defn['readonly']:
                                    low_bit = field_defn['low_bit']
                                    high_bit = field_defn['high_bit']

                                    # Set the bits from (low_bit, high_bit) in bit_mask to 1 (inclusive)
                                    for i in range(low_bit, high_bit + 1):
                                        bit_mask |= (1 << i)

                            self._mask = bit_mask
                            break

        return PythonPrettyInteger('mask', self._mask)

class PythonDestRegister(MaskedRegister):
    def __init__(self, reg_name, rd_val_before, rd_val_after, truth_rd_val_after):
        MaskedRegister.__init__(self, reg_name)
        self.rd_val_before = rd_val_before
        self.rd_val_after = rd_val_after
        self.truth_rd_val_after = truth_rd_val_after

    def __repr__(self):
        rep = self.reg_name + '\n'
        rep += '    rd val before: ' + f'0x{self.rd_val_before:016X}' + '\n'
        rep += '    rd val after:  ' + f'0x{self.rd_val_after:016X}' + '\n'
        if self.rd_val_after != self.truth_rd_val_after:
            rep += '    expected:      ' + f'0x{self.truth_rd_val_after:016X}' + '\n'

        return rep

    def __str__(self):
        return f'0x{self.rd_val_after:016X}'

    @property
    def regname(self):
        return self.reg_name

    @property
    def regval(self):
        return self.rd_val_after

    @property
    def hex(self):
        return f'0x{self.rd_val_after:016X}'

class PythonPrettyInteger:
    def __init__(self, reg_name, reg_val):
        self.reg_name = reg_name
        self.reg_val = reg_val

    def __repr__(self):
        rep = self.reg_name + '\n'
        rep += '    int val: ' + str(self.reg_val) + '\n'
        rep += '    hex val: ' + f'0x{self.reg_val:016X}' + '\n'
        return rep

    def __str__(self):
        return f'0x{self.reg_val:016X}'

    @property
    def hex(self):
        return f'0x{self.reg_val:016X}'

    @property
    def int(self):
        return self.reg_val

class PythonPrettyRegister(MaskedRegister):
    def __init__(self, reg_name, reg_val):
        MaskedRegister.__init__(self, reg_name)
        self.reg_val = reg_val

    def __repr__(self):
        rep = self.reg_name + '\n'
        rep += '    reg val (int): ' + f'{self.reg_val}' + '\n'
        rep += '    reg val (hex): ' + f'0x{self.reg_val:016X}' + '\n'
        return rep

    def __str__(self):
        return f'0x{self.reg_val:016X}'

    @property
    def regname(self):
        return self.reg_name

    @property
    def regval(self):
        return self.reg_val

    @property
    def hex(self):
        return f'0x{self.reg_val:016X}'

class PythonImmediate:
    def __init__(self, imm_val):
        self.imm_val = imm_val

    def __repr__(self):
        rep = 'imm\n'
        rep += '    int val: ' + str(self.imm_val) + '\n'
        rep += '    hex val: ' + f'0x{self.imm_val:016X}' + '\n'
        return rep

    def __str__(self):
        return f'0x{self.imm_val:016X}'

    @property
    def int(self):
        return self.imm_val

    @property
    def hex(self):
        return f'0x{self.imm_val:016X}'

class WorkloadsDB:
    def __init__(self, wdb_file):
        self.conn = sqlite3.connect(wdb_file)
        self.cursor = self.conn.cursor()

        self.cursor.execute('SELECT Id,TestName FROM RiscvTests')
        self.test_names_by_id = dict(self.cursor.fetchall())
        self.test_ids_by_name = dict((v,k) for k,v in self.test_names_by_id.items())

        self.cursor.execute('SELECT TestId,ResultCode FROM Instructions')
        self.failing_test_ids = set()
        for test_id, result_code in self.cursor.fetchall():
            # 2: pc invalid
            # 3: reg val invalid
            # 4: unimplemented inst
            if result_code >> 16 in (2,3,4):
                self.failing_test_ids.add(test_id)

        self.cursor.execute('SELECT RegName,RegType,RegIdx FROM Registers')
        self.reg_names_by_key = {}
        for reg_name, reg_type, reg_idx in self.cursor.fetchall():
            self.reg_names_by_key[(reg_type, reg_idx)] = reg_name
            GROUP_NUMS_BY_REG_NAME[reg_name] = reg_type
            REG_IDS_BY_REG_NAME[reg_name] = reg_idx

    def GetTestId(self, test_name):
        return self.test_ids_by_name[test_name]

    def GetTestName(self, test_id):
        return self.test_names_by_id[test_id]

    def IsFailing(self, test_id):
        if isinstance(test_id, str):
            test_id = self.GetTestId(test_id)
        return test_id in self.failing_test_ids

    def GetPassingTestNames(self):
        test_names = [test_name for test_id, test_name in self.test_names_by_id.items() if not self.IsFailing(test_id)]
        test_names.sort()
        return test_names

    def GetFailingTestNames(self):
        test_names = [test_name for test_id, test_name in self.test_names_by_id.items() if self.IsFailing(test_id)]
        test_names.sort()
        return test_names

    def GetInitialDiffs(self, test_id, hart_id=0):
        self.cursor.execute('SELECT RegType,RegIdx,ExpectedInitVal,ActualInitVal FROM Registers WHERE TestId=? AND HartId=?', (test_id, hart_id))
        diffs = []
        for group_num, reg_idx, expected_val, actual_val in self.cursor.fetchall():
            diffs.append({'group_num': group_num, 'reg_idx': reg_idx, 'expected_val': expected_val, 'actual_val': actual_val})

        return diffs

    def GetInitialRegVals(self, test_id, hart_id=0):
        self.cursor.execute('SELECT RegType,RegIdx,ActualInitVal FROM Registers WHERE TestId=? AND HartId=?', (test_id, hart_id))
        reg_vals = []
        for reg_type, reg_idx, init_val in self.cursor.fetchall():
            reg_vals.append({'reg_type': reg_type, 'reg_idx': reg_idx, 'init_val': init_val})

        return reg_vals

    def GetRegisterName(self, group_num, reg_idx):
        return self.reg_names_by_key[(group_num, reg_idx)]

    def GetGroupNum(self, reg_name):
        for key, name in self.reg_names_by_key.items():
            if name == reg_name:
                return key[0]

        return None

    def GetRegIdx(self, reg_name):
        for key, name in self.reg_names_by_key.items():
            if name == reg_name:
                return key[1]

        return None

class InstStatus(IntEnum):
    PASS = 0
    MISMATCH_PC = 1
    MISMATCH_REG_VAL = 2
    UNIMPL_PASSING = 3
    UNIMPL_FAILING = 4
    EXCEPTION_PASSING = 5
    EXCEPTION_FAILING = 6
    UNSPECIFIED = 7

def GetInstStatusColor(status):
    if status in (InstStatus.PASS, InstStatus.EXCEPTION_PASSING):
        return wx.WHITE
    elif status in (InstStatus.MISMATCH_PC, InstStatus.MISMATCH_REG_VAL, InstStatus.UNIMPL_FAILING, InstStatus.EXCEPTION_FAILING):
        return wx.RED
    elif status == InstStatus.UNIMPL_PASSING:
        return wx.YELLOW
    else:
        return wx.GRAY

if __name__ == "__main__":
    app = wx.App()

    # Enable frame inspection
    #import wx.lib.inspection
    #wx.lib.inspection.InspectionTool().Show()

    frame = AtlasIDE(sys.argv[1])
    frame.Show()
    app.MainLoop()
