#!/usr/bin/env python3
"""Helper script for regenerating register definition JSON files.
"""

from enum import Enum
import os, shutil

from GenRegisterJSON import GenRegisterJSON
from GenRegisterJSON import RegisterGroup

from GenCSRHeaders import gen_csr_num_header
from GenCSRHeaders import gen_csr_helpers_header
from GenCSRHeaders import gen_csr_bitmask_header
from GenCSRHeaders import gen_csr_field_idxs_header

def main():
    pegasus_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    arch_root = os.path.join(pegasus_root, 'arch')
    rv32_root = os.path.join(arch_root, 'rv32')
    inc_root = os.path.join(pegasus_root, 'include')

    os.chdir(arch_root)

    # Make rv64 directory if it doesn't exist
    if not os.path.exists("rv64"):
        os.makedirs("rv64")
    os.chdir("rv64")

    # Generate rv64g int, fp and CSR registers
    registers = {}
    RV64_XLEN = 8
    SUPPORTED_VLENS = [128, 256, 512, 1024, 2048]
    registers["int"] = GenRegisterJSON(RegisterGroup.INT, 32, RV64_XLEN)
    registers["fp"]  = GenRegisterJSON(RegisterGroup.FP,  32, RV64_XLEN)
    for vlen in SUPPORTED_VLENS:
        name = "vec" + str(vlen)
        registers[name] = GenRegisterJSON(RegisterGroup.VEC, 32, int(vlen/8))
    registers["csr"] = GenRegisterJSON(RegisterGroup.CSR, 0,  RV64_XLEN)

    # Add register for the PC
    num = 32
    registers["int"].add_custom_register("pc", num, "Program counter", 8, [], {}, True)

    # Add registers for atomic load-reservation and store-conditional instructions
    num += 1
    registers["int"].add_custom_register("resv_addr", num, "Load reservation address", 8, [], {}, True)
    num += 1
    registers["int"].add_custom_register("resv_valid", num, "Load reservation valid", 8, [], {}, True)

    for name, regs in registers.items():
        filename = "reg_" + name + ".json"
        regs.write_json(filename)

    # Make rv32 directory if it doesn't exist
    os.chdir("..")
    if not os.path.exists("rv32"):
        os.makedirs("rv32")
    os.chdir("rv32")

    # Generate rv32g int, fp and CSR registers
    RV32_XLEN = 4
    registers["int"] = GenRegisterJSON(RegisterGroup.INT, 32, RV32_XLEN)
    registers["fp"] = GenRegisterJSON(RegisterGroup.FP,  32, RV32_XLEN)
    # Vector registers are the same for RV32
    registers["csr"] = GenRegisterJSON(RegisterGroup.CSR, 0,  RV32_XLEN)

    # Add register for the PC
    num = 32
    registers["int"].add_custom_register("pc", num, "Program counter", 4, [], {}, True)

    # Add registers for atomic load-reservation and store-conditional instructions
    num += 1
    registers["int"].add_custom_register("resv_addr", num, "Load reservation address", 4, [], {}, True)
    num += 1
    registers["int"].add_custom_register("resv_valid", num, "Load reservation valid", 4, [], {}, True)

    for name, regs in registers.items():
        filename = "reg_" + name + ".json"
        regs.write_json(filename)

    # Generate Pegasus header files
    os.chdir("..")
    csr_num_hpp = gen_csr_num_header()
    csr_helpers_hpp = gen_csr_helpers_header()
    csr_field_idxs32_hpp = gen_csr_field_idxs_header(4)
    csr_field_idxs64_hpp = gen_csr_field_idxs_header(8)
    csr_bitmasks32_hpp = gen_csr_bitmask_header(4)
    csr_bitmasks64_hpp = gen_csr_bitmask_header(8)

    shutil.copyfile(csr_num_hpp, os.path.join(inc_root, csr_num_hpp))
    os.remove(csr_num_hpp)

    shutil.copyfile(csr_helpers_hpp, os.path.join(inc_root, csr_helpers_hpp))
    os.remove(csr_helpers_hpp)

    os.rename(csr_field_idxs32_hpp, os.path.join(inc_root, csr_field_idxs32_hpp))
    os.rename(csr_field_idxs64_hpp, os.path.join(inc_root, csr_field_idxs64_hpp))

    os.rename(csr_bitmasks32_hpp, os.path.join(inc_root, csr_bitmasks32_hpp))
    os.rename(csr_bitmasks64_hpp, os.path.join(inc_root, csr_bitmasks64_hpp))

if __name__ == "__main__":
    main()
