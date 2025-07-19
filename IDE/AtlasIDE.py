import os, wx, argparse
from editor.test_tree import TestTreeCtrl
from backend.workspace import Workspace

class PegasusIDE(wx.Frame):
    def __init__(self, riscv_tests_dir, sim_exe_path):
        wx.Frame.__init__(self, None, -1, "Pegasus IDE")
        self.riscv_tests_dir = riscv_tests_dir
        self.sim_exe_path = sim_exe_path

        self.vsplitter = wx.SplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.test_tree = TestTreeCtrl(self.vsplitter, self)
        self.workspace = Workspace(self.vsplitter, self)

        self.vsplitter.SplitVertically(self.test_tree, self.workspace)
        self.vsplitter.SetMinimumPaneSize(50)
        self.vsplitter.SetSashPosition(250)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.vsplitter, 1, wx.EXPAND)
        self.SetSizer(sizer)

        if wx.DisplaySize()[0] > 1920:
            self.SetSize((1920, 1080))
        else:
            self.Maximize()

        self.Layout()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Pegasus IDE')
    parser.add_argument('--riscv-tests-dir', required=True, help='Path to the RISC-V tests directory')
    parser.add_argument('--sim-exe-path', required=True, help='Path to the RISC-V simulator executable')
    parser.add_argument('--inspect-frame', action='store_true', help='Enable frame inspection (developer use only)')
    args = parser.parse_args()

    riscv_tests_dir = args.riscv_tests_dir
    assert os.path.isdir(riscv_tests_dir), 'Invalid RISC-V tests directory: ' + riscv_tests_dir
    riscv_tests_dir = os.path.abspath(riscv_tests_dir)

    sim_exe_path = args.sim_exe_path
    assert os.path.isfile(sim_exe_path), 'Invalid simulator executable: ' + sim_exe_path
    sim_exe_path = os.path.abspath(sim_exe_path)

    app = wx.App()

    if args.inspect_frame:
        import wx.lib.inspection
        wx.lib.inspection.InspectionTool().Show()

    frame = PegasusIDE(riscv_tests_dir, sim_exe_path)
    frame.Show()
    app.MainLoop()
