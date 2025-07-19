import wx, os, json, re
import wx.grid
import wx.py.shell
import wx.lib.scrolledpanel
from backend.c_dtypes import uint64_t, int64_t
from backend.pegasus_dtypes import CSR
from backend.sim_api import FormatHex

class TestDebugger(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame

        hsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.state_viewer = StateViewer(hsplitter, frame)

        locals = {'uint64_t': uint64_t, 'int64_t': int64_t, 'CSR': CSR, 'clear': self.__ClearShell}
        self.shell = PyShell(hsplitter, locals=locals)

        hsplitter.SplitHorizontally(self.state_viewer, self.shell)
        hsplitter.SetMinimumPaneSize(50)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(hsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

        w,h = self.GetSize()
        hsplitter.SetSashPosition(h-200)

    def SetStateQuery(self, state_query):
        self.state_viewer.SetStateQuery(state_query)

    def SetArch(self, arch):
        assert arch in ['rv32', 'rv64']
        xlen = 32 if arch == 'rv32' else 64
        self.state_viewer.SetXLEN(xlen)
        self.shell.SetXLEN(xlen)

    def LoadInst(self, pc, inst):
        self.state_viewer.LoadInst(pc, inst)

    def LoadInitialState(self):
        self.state_viewer.LoadInitialState()

    def __ClearShell(self):
        self.shell.clear()

class PyShell(wx.py.shell.Shell):
    def __init__(self, *args, **kwargs):
        wx.py.shell.Shell.__init__(self, *args, **kwargs)
        self.Bind(wx.EVT_KEY_DOWN, self.__OnKeyPress)
        self.command_history = []
        self.command_history_cursor = None

    def SetXLEN(self, xlen):
        self.push('xlen = {}'.format(xlen), silent=True)

    def __OnKeyPress(self, event):
        if event.GetKeyCode() == wx.WXK_RETURN:
            command = self.getCommand()
            if self.command_history and command == self.command_history[-1]:
                self.command_history.pop()
            self.command_history.append(command)
            self.command_history_cursor = len(self.command_history)
            event.Skip()
        elif event.GetKeyCode() == wx.WXK_UP and self.command_history:
            if self.command_history_cursor > 0:
                self.command_history_cursor -= 1
                self.clearCommand()
                self.write(self.command_history[self.command_history_cursor])
                self.SetInsertionPointEnd()
        elif event.GetKeyCode() == wx.WXK_DOWN and self.command_history:
            if self.command_history_cursor < len(self.command_history) - 1:
                self.command_history_cursor += 1
                self.clearCommand()
                self.write(self.command_history[self.command_history_cursor])
                self.SetInsertionPointEnd()
        else:
            event.Skip()

class StateViewer(wx.Panel):
    def __init__(self, parent, frame):
        wx.Panel.__init__(self, parent)
        self.frame = frame
        self.state_query = None
        self.SetBackgroundColour('white')

        meta_panel = wx.Panel(self)
        self.pc_label = wx.StaticText(meta_panel, label="pc:0x0")
        self.priv_label = wx.StaticText(meta_panel, label="priv:")
        self.changes_label = wx.StaticText(meta_panel, label="changes:")

        self.pc_label.Bind(wx.EVT_RIGHT_DOWN, self.__OnRightClick)

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.pc_label.SetFont(mono10)
        self.priv_label.SetFont(mono10)
        self.changes_label.SetFont(mono10)

        row1_sizer = wx.BoxSizer(wx.HORIZONTAL)
        row1_sizer.Add(self.pc_label)
        row1_sizer.AddSpacer(25)
        row1_sizer.Add(self.priv_label)
        row1_sizer.AddSpacer(25)
        row1_sizer.Add(self.changes_label)
        meta_panel.SetSizer(row1_sizer)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(meta_panel, 0, wx.EXPAND)
        sizer.Add(wx.StaticLine(self), 0, wx.EXPAND)

        pegasus_root = os.path.join(os.path.dirname(__file__), '..', '..')

        int_json = os.path.join(pegasus_root, 'arch', 'rv64', 'reg_int.json')
        with open(int_json, 'r') as fin:
            int_names = [reg['name'] for reg in json.load(fin)]
            int_names = [name for name in int_names if name.startswith('x')]

        fp_json = os.path.join(pegasus_root, 'arch', 'rv64', 'reg_fp.json')
        with open(fp_json, 'r') as fin:
            fp_names = [reg['name'] for reg in json.load(fin)]

        csr_json = os.path.join(pegasus_root, 'arch', 'rv64', 'reg_csr.json')
        with open(csr_json, 'r') as fin:
            csr_names = [reg['name'] for reg in json.load(fin)]

        self.regval_grids = {
            'x':   RegisterGrid(self, int_names, color_nonzero=True),
            'f':   RegisterGrid(self, fp_names, color_nonzero=True),
            'csr': RegisterGrid(self, csr_names)
        }

        cpp_notebook = wx.Notebook(self)

        def AddTab(notebook, tab_name, handler_subdir=None, handler_file=None):
            if handler_subdir is None and handler_file is None:
                handler_subdir = tab_name

            if handler_subdir is not None:
                pegasus_root = os.path.join(os.path.dirname(__file__), '..', '..')
                handler_dir = os.path.join(pegasus_root, 'core', 'inst_handlers', handler_subdir)

                cpp_code = None
                for handler_file in os.listdir(handler_dir):
                    if handler_file.endswith('.cpp'):
                        with open(os.path.join(handler_dir, handler_file), 'r') as fin:
                            cpp_code = fin.read()
                            break
            else:
                with open(handler_file, 'r') as fin:
                    cpp_code = fin.read()

            cpp_scrolled_panel = wx.lib.scrolledpanel.ScrolledPanel(notebook)
            cpp_scrolled_panel.SetupScrolling()

            mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
            cpp_code_panel = wx.TextCtrl(cpp_scrolled_panel, style=wx.TE_MULTILINE|wx.TE_READONLY)
            cpp_code_panel.SetFont(mono10)
            cpp_code_panel.SetValue(cpp_code)

            search_bar = wx.SearchCtrl(cpp_scrolled_panel)
            search_bar.Bind(wx.EVT_SEARCH, self.__OnCppSearch)
            search_bar.Bind(wx.EVT_SEARCH_CANCEL, self.__OnCppSearchCancelled)

            cpp_scrolled_panel_sizer = wx.BoxSizer(wx.VERTICAL)
            cpp_scrolled_panel_sizer.Add(cpp_code_panel, 1, wx.EXPAND)
            cpp_scrolled_panel_sizer.Add(search_bar, 0, wx.EXPAND)
            cpp_scrolled_panel.SetSizer(cpp_scrolled_panel_sizer)

            notebook.AddPage(cpp_scrolled_panel, tab_name)

        AddTab(cpp_notebook, 'a')
        AddTab(cpp_notebook, 'd')
        AddTab(cpp_notebook, 'f')
        AddTab(cpp_notebook, 'i')
        AddTab(cpp_notebook, 'm')
        AddTab(cpp_notebook, 'zicsr')
        AddTab(cpp_notebook, 'zifencei')

        pegasus_root = os.path.join(os.path.dirname(__file__), '..', '..')
        execute_cpp_file = os.path.join(pegasus_root, 'core', 'Exception.cpp')
        AddTab(cpp_notebook, 'trap', handler_file=execute_cpp_file)

        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        hsizer.Add(self.regval_grids['x'])
        hsizer.Add(self.regval_grids['f'])
        hsizer.Add(self.regval_grids['csr'])
        hsizer.Add(cpp_notebook, 1, wx.LEFT|wx.EXPAND, 10)
        sizer.Add(hsizer, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

    def SetXLEN(self, xlen):
        for _, grid in self.regval_grids.items():
            grid.SetXLEN(xlen)

    def SetStateQuery(self, state_query):
        self.state_query = state_query
        self.LoadInitialState()

    def LoadInst(self, pc, inst):
        snapshot = self.state_query.GetSnapshot(pc)
        changes = snapshot.getChanges()

        self.pc_label.SetLabel('pc:'+FormatHex(pc))

        changes_list = []
        for reg_name, (prev_val, curr_val) in changes.items():
            prev_val = FormatHex(prev_val)
            curr_val = FormatHex(curr_val)
            changes_list.append('{}({})'.format(reg_name, '{}->{}'.format(prev_val, curr_val)))

        for mem_read in snapshot.getMemReads():
            addr = FormatHex(mem_read.addr)
            value = FormatHex(mem_read.value)
            change = 'memread.{}({})'.format(addr, value)
            changes_list.append(change)

        for mem_write in snapshot.getMemWrites():
            addr = FormatHex(mem_write.addr)
            value = FormatHex(mem_write.value)
            prior = FormatHex(mem_write.prior)
            change = 'memwrite.{}({}->{})'.format(addr, prior, value)
            changes_list.append(change)

        if not changes_list:
            changes_list.append('none')

        self.changes_label.SetLabel('changes: ' + ', '.join(changes_list))
        for grid in self.regval_grids.values():
            grid.SetRegValues(snapshot)

        self.Layout()

    def LoadInitialState(self):
        if not self.state_query:
            return

        snapshot = self.state_query.GetInitialState()
        pc = snapshot.getPC()
        priv = ['U', 'S', 'H', 'M'][int(snapshot.getRegValue('resv_priv'), 16)]
        self.pc_label.SetLabel('pc:{}'.format(FormatHex(pc)))
        self.priv_label.SetLabel('priv:'+priv)
        self.changes_label.SetLabel('changes: ----')

        for _, grid in self.regval_grids.items():
            grid.SetRegValues(snapshot)

        self.Layout()

    def __OnCppSearch(self, event):
        search_bar = event.GetEventObject()
        search_text = search_bar.GetValue()
        cpp_code_panel = search_bar.GetParent().GetChildren()[0]
        cpp_code = cpp_code_panel.GetValue()

        curr_sel = cpp_code_panel.GetSelection()
        if curr_sel[0] == curr_sel[1]:
            start = cpp_code.find(search_text)
        else:
            start = cpp_code.find(search_text, curr_sel[1])
            if start == -1:
                start = cpp_code.find(search_text)

        if start == -1:
            return

        end = start + len(search_text)
        cpp_code_panel.SetSelection(start, end)
        cpp_code_panel.ShowPosition(start)

    def __OnCppSearchCancelled(self, event):
        search_bar = event.GetEventObject()
        cpp_code_panel = search_bar.GetParent().GetChildren()[0]
        cpp_code_panel.SetSelection(0, 0)

    def __OnRightClick(self, event):
        text = self.pc_label.GetLabel()
        if not text:
            return
        
        menu = wx.Menu()
        menu.Append(1, 'Copy')
        self.Bind(wx.EVT_MENU, self.__OnCopy, id=1)
        self.PopupMenu(menu)
        menu.Destroy()

    def __OnCopy(self, event):
        text = self.pc_label.GetLabel()
        val = text.split(':')[-1]
        val = 'uint64_t({})'.format(text.split(':')[-1])
        wx.TheClipboard.Open()
        wx.TheClipboard.SetData(wx.TextDataObject(val))
        wx.TheClipboard.Close()

class RegisterGrid(wx.grid.Grid):
    def __init__(self, parent, reg_names, color_nonzero=False):
        wx.grid.Grid.__init__(self, parent)
        self.reg_names = reg_names
        self.color_nonzero = color_nonzero
        self.xlen = None
        self.expected_vals_by_row = {}
        self.GetGridWindow().Bind(wx.EVT_MOTION, self.__OnMouseMove)

        self.CreateGrid(0, 2)
        self.HideRowLabels()
        self.HideColLabels()
        self.EnableEditing(False)
        self.DisableDragGridSize()

        mono9 = wx.Font(9, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.SetDefaultCellFont(mono9)

        self.Bind(wx.grid.EVT_GRID_CELL_RIGHT_CLICK, self.__OnCellRightClick)

    def SetXLEN(self, xlen):
        self.xlen = xlen

    def SetRegValues(self, snapshot):
        self.ClearGrid()
        self.expected_vals_by_row = {}

        for row in range(self.GetNumberRows()):
            self.SetCellBackgroundColour(row, 0, 'white')
            self.SetCellBackgroundColour(row, 1, 'white')

            # Reset the font weight to normal
            cell_font = self.GetCellFont(row, 0)
            cell_font.SetWeight(wx.FONTWEIGHT_NORMAL)
            self.SetCellFont(row, 0, cell_font)
            self.SetCellFont(row, 1, cell_font)

        # Note that screen space becomes a problem when we try to display
        # every CSR value. We should only display the ones that are non-zero.
        # The regex below is to distinguish between integer/fp registers and CSRs.
        xpattern = r"^x([0-9]|[12][0-9]|3[01])$"
        fpattern = r"^f([0-9]|[12][0-9]|3[01])$"
        nvpairs = []

        for reg_name in self.reg_names:
            reg_val = snapshot.getRegValue(reg_name)
            take = False
            if reg_val not in (None, 'NULL', '0x0'):
                take = int(reg_val, 16) > 0

            take = take or re.match(xpattern, reg_name) or re.match(fpattern, reg_name)
            if take:
                nvpairs.append((reg_name, reg_val))

        num_rows_have = self.GetNumberRows()
        num_rows_need = len(nvpairs)
        if num_rows_need > num_rows_have:
            self.AppendRows(num_rows_need - num_rows_have)

        for row, (reg_name, reg_val) in enumerate(nvpairs):
            self.SetCellValue(row, 0, reg_name)
            self.SetCellValue(row, 1, reg_val)

            changes = snapshot.getChanges()
            if changes is not None and reg_name in snapshot.getChanges():
                # This background color is used merely for UX purposes to quickly see
                # which registers just changed at this PC.
                self.SetCellBackgroundColour(row, 0, (255, 255, 153))
                self.SetCellBackgroundColour(row, 1, (255, 255, 153))
            elif int(reg_val, 16) and self.color_nonzero:
                # This background color is used merely for UX purposes to quickly see
                # which registers are non-zero.
                self.SetCellBackgroundColour(row, 0, (255, 191, 128))
                self.SetCellBackgroundColour(row, 1, (255, 191, 128))

            # Add a tooltip showing the expected value (Spike) if different than the actual value (Pegasus).
            expected_val = snapshot.getExpectedValue(reg_name)
            if expected_val not in (None, 'NULL'):
                if FormatHex(reg_val) != FormatHex(expected_val):
                    self.SetCellBackgroundColour(row, 0, (255, 51, 0))
                    self.SetCellBackgroundColour(row, 1, (255, 51, 0))
                    cell_font = self.GetCellFont(row, 0)
                    cell_font.SetWeight(wx.FONTWEIGHT_BOLD)
                    self.SetCellFont(row, 0, cell_font)
                    self.SetCellFont(row, 1, cell_font)
                    self.expected_vals_by_row[row] = FormatHex(expected_val)

            self.ShowRow(row)

        for row in range(len(nvpairs), self.GetNumberRows()):
            self.HideRow(row)

        max_reg_val_len = max(len(reg_val) for _, reg_val in nvpairs) - 2
        for row in range(len(nvpairs)):
            reg_val_label = self.GetCellValue(row, 1).lstrip('0x')
            reg_val_label = reg_val_label.zfill(max_reg_val_len)
            reg_val_label = '0x' + reg_val_label
            self.SetCellValue(row, 1, reg_val_label)

        for row in self.expected_vals_by_row:
            expected_val = self.expected_vals_by_row[row].lstrip('0x')
            expected_val = expected_val.zfill(max_reg_val_len)
            expected_val = '0x' + expected_val
            self.expected_vals_by_row[row] = expected_val

        self.AutoSize()

    def __OnCellRightClick(self, event):
        row = event.GetRow()
        col = event.GetCol()
        if col == 1:
            is_csr = not all([name.startswith('x') for name in self.reg_names]) and \
                     not all([name.startswith('f') for name in self.reg_names])

            def copy_text(event):
                reg_name = self.GetCellValue(row, 0)
                reg_val = self.GetCellValue(row, col)
                reg_type = 'CSR' if is_csr else 'uint64_t'
                copy_text = '{} = {}("{}", {}, {})'.format(reg_name, reg_type, reg_name, reg_val, self.xlen)

                wx.TheClipboard.Open()
                wx.TheClipboard.SetData(wx.TextDataObject(copy_text))
                wx.TheClipboard.Close()

            menu = wx.Menu()
            menu.Append(1, 'Copy')
            self.Bind(wx.EVT_MENU, copy_text, id=1)
            self.PopupMenu(menu)

    def __OnMouseMove(self, event):
        if len(self.expected_vals_by_row) == 0:
            event.Skip()
            return

        _, y = event.GetPosition()
        row = self.YToRow(y)

        if row in self.expected_vals_by_row:
            tooltip = 'Expected: {}'.format(self.expected_vals_by_row[row])
            self.SetToolTip(wx.ToolTip(tooltip))
        else:
            self.UnsetToolTip()
