#!/usr/bin/env python3
"""Helper script for generating instruction handler code.
"""

from enum import Enum
import json
import math
import sys
import os

from insts.RVI_INST import RV32I_INST
from insts.RVI_INST import RV64I_INST
from insts.RVM_INST import RV32M_INST
from insts.RVM_INST import RV64M_INST
from insts.RVA_INST import RV32A_INST
from insts.RVA_INST import RV64A_INST
from insts.RVF_INST import RV32F_INST
from insts.RVF_INST import RV64F_INST
from insts.RVD_INST import RV32D_INST
from insts.RVD_INST import RV64D_INST

from insts.RVZBA_INST import RV32ZBA_INST
from insts.RVZBA_INST import RV64ZBA_INST
from insts.RVZBB_INST import RV32ZBB_INST
from insts.RVZBB_INST import RV64ZBB_INST
from insts.RVZBC_INST import RV32ZBC_INST
from insts.RVZBC_INST import RV64ZBC_INST
from insts.RVZBS_INST import RV32ZBS_INST
from insts.RVZBS_INST import RV64ZBS_INST

from insts.RVZICSR_INST import RV32ZICSR_INST
from insts.RVZICSR_INST import RV64ZICSR_INST
from insts.RVZIFENCEI_INST import RV32ZIFENCEI_INST
from insts.RVZIFENCEI_INST import RV64ZIFENCEI_INST

class InstHandlerGenerator():
    """Helper script for adding a new extension to Pegasus.

    This script generates the extension class and instruction handler methods
    required to support a new extension in Pegasus. If a path to Spike is
    provided, the script will also insert Spike's instruction handler code as
    comments into the instruction handler methods for reference.

    Args:
        xlen (int): XLEN (32 or 64)
        ext_str (str): Extension string (e.g. "i", "m", "a", "f", "d")
        spike_path (str): Optional path to Spike directory
    """

    def __init__(self, xlen, ext_str, spike_path=None):
        self.xlen = xlen
        self.isa_string = "rv"+str(xlen)
        self.ext_str = ext_str

        self.insts = globals()[ "RV" + str(self.xlen) + self.ext_str.upper() + "_INST" ]
        for inst in self.insts:
            inst['xlen'] = self.xlen

        # Sort alphabetically
        self.insts = sorted(self.insts, key=lambda x: x['mnemonic'])

        self.spike_path = spike_path

    def write_ext_class(self, ext_str):
        """ """

        class_name = "Rv" + ext_str + "Insts"

        #######################################################################
        # HEADER
        header_string = f"""#pragma once

#include <map>
#include <string>

namespace pegasus
{{
    class PegasusState;
    class Action;
    class ActionGroup;

    class {class_name}
    {{
    public:
        using base_type = {class_name};

        static void getInstComputeAddressHandlers(std::map<std::string, Action>& inst_handlers);
        static void getInstHandlers(std::map<std::string, Action>& inst_handlers);

    private:
"""

        for inst in self.insts:
            handler_name = inst["mnemonic"].replace('.', '_') + "_" + str(self.xlen)
            if inst["memory"]:
                header_string += f"        ActionGroup * {handler_name}_compute_address_handler(pegasus::PegasusState* state);\n"
            header_string += f"        ActionGroup * {handler_name}_handler(pegasus::PegasusState* state);\n"

        header_string += """    };
}
"""

        # Write to header file
        header_fname = class_name + ".hpp"
        if not os.path.isfile(header_fname):
            with open(header_fname, "w") as fh:
                fh.write(header_string)

        #######################################################################
        # SOURCE
        source_string = f"""#include "core/inst_handlers/{self.isa_string}/{ext_str}/{class_name}.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace pegasus
{{"""

        if any(inst["memory"] for inst in self.insts):
            source_string += f"""    void {class_name}::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {{
"""

            for inst in self.insts:
                if inst["memory"] == False:
                    continue
                mnemonic = inst["mnemonic"]
                inst_name = mnemonic.replace('.', '_')
                handler_name = inst_name + "_" + str(self.xlen)
                source_string += f"""        inst_handlers.emplace(\"{mnemonic}\", pegasus::Action::createAction<&{class_name}::{handler_name}_compute_address_handler, {class_name}>(nullptr, \"{inst_name}\", ActionTags::COMPUTE_ADDR_TAG));
"""

            source_string += f"""    }}

"""

        source_string += f"""    void {class_name}::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {{
"""

        for inst in self.insts:
            mnemonic = inst["mnemonic"]
            inst_name = mnemonic.replace('.', '_')
            handler_name = inst_name + "_" + str(self.xlen)
            source_string += f"        inst_handlers.emplace(\"{mnemonic}\", pegasus::Action::createAction<&{class_name}::{handler_name}_handler, {class_name}>(nullptr, \"{inst_name}\", ActionTags::EXECUTE_TAG));\n"

        source_string += """    }
};
"""

        # Write to source file
        source_fname = class_name + ".cpp"
        with open(source_fname, "w") as fh:
            fh.write(source_string)


    def write_inst_handler(self, ext_str, inst):
        inst_name = inst["mnemonic"].replace('.', '_')
        handler_name = inst_name + "_" + str(self.xlen)
        class_name = "Rv" + ext_str + "Insts"

        inst_handler_string = f"""#include \"core/inst_handlers/{self.isa_string}/{ext_str}/{class_name}.hpp\"
//#include \"core/ActionGroup.hpp\"
//#include \"core/PegasusState.hpp\"
//#include \"core/PegasusInst.hpp\"

namespace pegasus
{{"""

        inst_handler_string += "\n    class PegasusState;\n\n"

        if inst["memory"]:
            inst_handler_string += f"""
    ActionGroup* {class_name}::{handler_name}_compute_address_handler(pegasus::PegasusState* state)
    {{
        (void)state;
        return nullptr;
    }}
"""

        inst_handler_string += f"""
    ActionGroup* {class_name}::{handler_name}_handler(pegasus::PegasusState* state)
    {{
        (void)state;
"""

        spike_fname = ""
        if(self.spike_path):
            spike_fname = self.spike_path + "/riscv/insns/" + inst_name + ".h"
            if not os.path.isfile(spike_fname):
                print("WARNING FILE DOES NOT EXISTS:", spike_fname)
                spike_fname = ""

        if spike_fname and os.path.isfile(spike_fname):
            inst_handler_string += f"""        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

"""
            with open(spike_fname, "r") as sfh:
                for line in sfh:
                    line = line.replace("  ", "    ")

                    if line.strip():
                        inst_handler_string += f"""        // {line}"""
                    else:
                        inst_handler_string += "\n"

            inst_handler_string += f"""
        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
"""

        inst_handler_string +="""        return nullptr;
    }
}"""

        pegasus_fname = inst_name + ".cpp"
        with open(pegasus_fname, "w") as fh:
            fh.write(inst_handler_string)


USAGE_STR = "Usage: GenInstructionHandlers.py [isa string] [spike path]"
def main():
    """{USAGE_STR}"""
    if(len(sys.argv) < 2):
        print(USAGE_STR)
        return

    isa_string = sys.argv[1]
    if(isa_string[0:4] != "rv32" and isa_string[0:4] != "rv64"):
        print("Invalid ISA string:", isa_string)
        return
    xlen = 32 if "32" in isa_string else 64
    ext = isa_string.removeprefix("rv32").removeprefix("rv64")

    spike_path = None
    if(len(sys.argv) == 3):
        spike_path = sys.argv[2]
        spike_path = os.path.abspath(spike_path)
        if(not os.path.isdir(spike_path)):
            print("Invalid path to Spike directory:", spike_path)
            return

    inst_handler_gen = InstHandlerGenerator(xlen, ext, spike_path)

    # File path is isa_string/ext/ e.g. rv64/i/
    dir_name = isa_string[0:4]
    if (not os.path.isdir(dir_name)):
        os.mkdir(dir_name)
    os.chdir(dir_name)

    # Create extension subdir
    if (os.path.isdir(ext)):
        print("ERROR: Directory \'"+ext+"\' already exists!")
        print("This script SHOULD NOT be used to regenerate existing instruction handlers.")
        return
    os.mkdir(ext)
    os.chdir(ext)

    # Write extension class header and source file
    inst_handler_gen.write_ext_class(ext)

    # Write instruction handler with Spike source code
    for inst in inst_handler_gen.insts:
        inst_handler_gen.write_inst_handler(ext, inst)


if __name__ == "__main__":
    main()
