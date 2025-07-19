import wx, multiprocessing, sys
from backend.sim_api import *
from backend.pegasus_dtypes import *
from backend.observers import *
from backend.state_db import StateDB
from backend.state_serializer import StateSerializer
from backend.state_query import StateQuery

class InstViewer(wx.Panel):
    def __init__(self, parent, frame, test_debugger):
        wx.Panel.__init__(self, parent)
        self.frame = frame
        self.test_debugger = test_debugger

        mono10 = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, 'Consolas')
        self.inst_list_ctrl = wx.ListCtrl(self, style=wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.inst_list_ctrl.SetFont(mono10)
        self.insts_by_pc = {}

        mono12_bold = wx.Font(12, wx.MODERN, wx.NORMAL, wx.BOLD, False, 'Consolas')
        self.loading_msg = wx.StaticText(self)
        self.loading_msg.SetFont(mono12_bold)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Layout()

        self.inst_list_ctrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__OnInstViewerItemSelected)

    def LoadTest(self, test):
        self.inst_list_ctrl.ClearAll()
        sizer = self.GetSizer()
        sizer.Clear()
        sizer.Add(self.loading_msg, 1, wx.EXPAND)
        self.loading_msg.Show()
        self.inst_list_ctrl.Hide()

        riscv_tests_dir = self.frame.riscv_tests_dir
        sim_exe_path = self.frame.sim_exe_path
        obs_sim = ObserverSim(riscv_tests_dir, sim_exe_path, test)
        if obs_sim.reg_info_query is None:
            msg = 'Test will load without "truth" values for comparison. '
            msg += 'Please follow the steps in <pegasus>/spike/README.md to build Spike to enable this feature.'
            wx.MessageBox(msg, 'Warning', wx.OK | wx.ICON_WARNING)

        state_db = StateDB()

        self.msg_queue = multiprocessing.Queue()
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.__UpdateLoadingMsg, self.timer)
        self.timer.Start(100)

        rv = test[:4]
        obs = StateSerializer(state_db, self.msg_queue, rv)

        # Get ready to call the transaction on a separate thread
        def RunObserver():
            state_db.CallInTransaction(lambda: obs_sim.RunObserver(obs, timeout=10))

        self.loading_msg.SetLabel('Loading test...')
        self.infinite_loop_pc = None
        self.process = multiprocessing.Process(target=RunObserver)
        self.process.start()

        while self.timer is not None:
            wx.Yield()

        self.process.join()
        self.process = None
        self.loading_msg.SetLabel('')

        self.inst_list_ctrl.InsertColumn(0, "PC")
        self.inst_list_ctrl.InsertColumn(1, "Disasm")
        self.inst_list_ctrl.InsertItem(0, "Initial State")

        sizer.Clear()
        sizer.Add(self.inst_list_ctrl, 1, wx.EXPAND)
        self.inst_list_ctrl.Show()
        self.loading_msg.Hide()

        state_query = StateQuery(state_db.db_file.name)

        for pc, _, dasm, inst_uid in state_query.GetInstructions():
            i = self.inst_list_ctrl.GetItemCount()
            self.inst_list_ctrl.InsertItem(i, pc)
            self.inst_list_ctrl.SetItem(i, 1, dasm)
            self.insts_by_pc[pc] = inst_uid

        if self.infinite_loop_pc:
            i = self.inst_list_ctrl.GetItemCount()
            self.inst_list_ctrl.InsertItem(i, self.infinite_loop_pc)
            self.inst_list_ctrl.SetItem(i, 1, 'Infinite loop detected')
            self.infinite_loop_pc = None

        self.inst_list_ctrl.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.inst_list_ctrl.SetColumnWidth(1, wx.LIST_AUTOSIZE)

        # Set our size to be the minimum size needed to show all items
        colwidth0 = self.inst_list_ctrl.GetColumnWidth(0)
        colwidth1 = self.inst_list_ctrl.GetColumnWidth(1)
        minwidth = colwidth0 + colwidth1 + 10
        self.inst_list_ctrl.SetSize((minwidth, wx.DisplaySize()[1]))
        self.inst_list_ctrl.Select(0)

        self.Layout()

        return state_query

    def __OnInstViewerItemSelected(self, event):
        selected_item = event.GetIndex()
        text = self.inst_list_ctrl.GetItemText(selected_item)
        if text == 'Initial State':
            self.test_debugger.LoadInitialState()
        else:
            pc = text
            inst = self.insts_by_pc[pc]
            self.test_debugger.LoadInst(pc, inst)

    def __UpdateLoadingMsg(self, event):
        message = ''
        while not self.msg_queue.empty():
            message = self.msg_queue.get()

        if message in ('SIM_FINISHED', 'SIM_DEAD'):
            if self.timer:
                self.timer.Stop()
                self.timer = None

            self.loading_msg.SetLabel(message)

        elif message.startswith('SIM_STUCK:'):
            _, pc = message.split(':')
            self.infinite_loop_pc = pc

            if self.timer:
                self.timer.Stop()
                self.timer = None

        elif message:
            self.loading_msg.SetLabel(message)

        wx.Yield()
