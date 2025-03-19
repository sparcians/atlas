from backend.sim_api import *
from backend.c_dtypes import uint64_t
import os, json

# Create a wrapper around a running AtlasState C++ object.
def atlas_state(endpoint):
    return AtlasState(endpoint)

# Create a wrapper around a running AtlasInst C++ object.
# C++: "AtlasInstPtr AtlasState::getCurrentInst()"
def atlas_current_inst(endpoint):
    inst = AtlasInst(endpoint)
    return inst if isinstance(inst.getUid(), int) else None

# Wrap an AtlasState C++ object.
class AtlasState:
    def __init__(self, endpoint):
        self.endpoint = endpoint

    def getXlen(self):
        return atlas_xlen(self.endpoint)

    def getPc(self):
        return atlas_pc(self.endpoint)

    def getPrevPc(self):
        return atlas_prev_pc(self.endpoint)

    def getPrivMode(self):
        return atlas_inst_priv(self.endpoint)

    def getTrapCause(self):
        return atlas_inst_active_exception(self.endpoint)

    def getSimState(self):
        return AtlasState.SimState(self.endpoint)

    def getCurrentInst(self):
        return AtlasState.AtlasInst(self.endpoint)

    def getIntRegisterSet(self):
        return AtlasIntRegisterSet(self.endpoint)

    def getFpRegisterSet(self):
        return AtlasFpRegisterSet(self.endpoint)
    
    def getVectorRegisterSet(self):
        return AtlasVecRegisterSet(self.endpoint)
    
    def getCsrRegisterSet(self):
        return AtlasCsrRegisterSet(self.endpoint)

    # Wrap an AtlasState::SimState C++ object.
    class SimState:
        def __init__(self, endpoint):
            self.endpoint = endpoint

        @property
        def current_uid(self):
            return atlas_inst_uid(self.endpoint)

        @property
        def inst_count(self):
            return atlas_inst_count(self.endpoint)

        @property
        def sim_stopped(self):
            return atlas_sim_stopped(self.endpoint)

        @property
        def test_passed(self):
            return atlas_test_passed(self.endpoint)

        @property
        def workload_exit_code(self):
            return atlas_exit_code(self.endpoint)

# Wrap an AtlasInst C++ object.
class AtlasInst:
    def __init__(self, endpoint):
        self.endpoint = endpoint

    def getAtlasState(self):
        return AtlasState(self.endpoint)

    def getUid(self):
        return atlas_inst_uid(self.endpoint)

    def getMnemonic(self):
        return atlas_inst_mnemonic(self.endpoint)

    def dasmString(self):
        return atlas_inst_dasm_string(self.endpoint)

    def getOpcode(self):
        return atlas_inst_opcode(self.endpoint)

    def getPrivMode(self):
        return atlas_inst_priv(self.endpoint)

    def getRs1(self):
        rs1_name = atlas_inst_rs1_name(self.endpoint)
        return AtlasRS1(self.endpoint, rs1_name) if rs1_name else None

    def getRs2(self):
        rs2_name = atlas_inst_rs2_name(self.endpoint)
        return AtlasRS2(self.endpoint, rs2_name) if rs2_name else None

    def getRd(self):
        rd_name = atlas_inst_rd_name(self.endpoint)
        return AtlasRD(self.endpoint, rd_name) if rd_name else None

    def getImmediate(self):
        has_imm = atlas_inst_has_immediate(self.endpoint)
        return atlas_inst_immediate(self.endpoint) if has_imm else None

    def deepCopy(self):
        return AtlasInstDeepCopy(self)

class AtlasInstDeepCopy:
    def __init__(self, orig_inst):
        self.uid = orig_inst.getUid()
        self.mnemonic = orig_inst.getMnemonic()
        self.dasm = orig_inst.dasmString()
        self.opcode = orig_inst.getOpcode()
        self.priv = orig_inst.getPrivMode()

        rs1 = orig_inst.getRs1()
        rs2 = orig_inst.getRs2()
        rd = orig_inst.getRd()
        imm = orig_inst.getImmediate()

        self.rs1 = rs1.deepCopy() if rs1 else None
        self.rs2 = rs2.deepCopy() if rs2 else None
        self.rd = rd.deepCopy() if rd else None
        self.imm = imm

    def getAtlasState(self):
        return None

    def getUid(self):
        return self.uid

    def getMnemonic(self):
        return self.mnemonic
    
    def dasmString(self):
        return self.dasm
    
    def getOpcode(self):
        return self.opcode
    
    def getPrivMode(self):
        return self.priv
    
    def getRs1(self):
        return self.rs1
    
    def getRs2(self):
        return self.rs2
    
    def getRd(self):
        return self.rd
    
    def getImmediate(self):
        return self.imm

# Wrap a sparta::Register C++ object.
class SpartaRegister:
    def __init__(self, endpoint, reg_name):
        self.endpoint = endpoint
        self.reg_name = reg_name

    def getName(self):
        return self.reg_name

    def getGroupNum(self):
        return atlas_reg_group_num(self.endpoint, self.reg_name)

    def getID(self):
        return atlas_reg_id(self.endpoint, self.reg_name)

    def read(self):
        return atlas_reg_value(self.endpoint, self.reg_name)

    def write(self, value):
        atlas_reg_write(self.endpoint, self.reg_name, value)

    def dmiWrite(self, value):
        atlas_reg_dmiwrite(self.endpoint, self.reg_name, value)

    def deepCopy(self):
        return SpartaRegisterDeepCopy(self)

class AtlasRD(SpartaRegister):  pass
class AtlasRS1(SpartaRegister): pass
class AtlasRS2(SpartaRegister): pass

# Provide read-only register access after the simulation is
# over. This may be useful to some python observers for post-
# processing, and the simulation would be stopped at this point.
#
# Create from reg.deepCopy()
class SpartaRegisterDeepCopy:
    def __init__(self, orig_reg):
        self.reg_name = orig_reg.getName()
        self.group_num = orig_reg.getGroupNum()
        self.reg_id = orig_reg.getID()
        self.value = orig_reg.read()

    def getName(self):
        return self.reg_name

    def getGroupNum(self):
        return self.group_num

    def getID(self):
        return self.reg_id

    def read(self):
        return '0x{:08x}'.format(self.value)

    def deepCopy(self):
        return SpartaRegisterDeepCopy(self)

# Wrap an atlas::RegisterSet C++ object.
class AtlasRegisterSet:
    def __init__(self, endpoint, group_num):
        self.endpoint = endpoint
        self.group_num = group_num

    def getRegister(self, reg_id):
        if reg_id >= self.getNumRegisters():
            return None

        prefix = ['x', 'f', 'v'][self.group_num]
        reg_name = prefix + str(reg_id)
        return SpartaRegister(self.endpoint, reg_name)

    def getNumRegisters(self):
        return atlas_num_regs_in_group(self.endpoint, self.group_num)

class AtlasIntRegisterSet(AtlasRegisterSet): pass
class AtlasFpRegisterSet(AtlasRegisterSet):  pass
class AtlasVecRegisterSet(AtlasRegisterSet): pass

class AtlasCsrRegisterSet(AtlasRegisterSet):
    def __init__(self, endpoint):
        AtlasRegisterSet.__init__(self, endpoint, 3)

    def getRegister(self, reg_id):
        csr_name = atlas_csr_name(self.endpoint, reg_id)
        if isinstance(csr_name, str) and csr_name != '':
            return SpartaRegister(self.endpoint, csr_name)

        return None

class CSR:
    def __init__(self, csr_name, csr_val, xlen):
        if isinstance(csr_val, int):
            self.csr_value = uint64_t(csr_val)

        assert isinstance(self.csr_value, uint64_t), f"Invalid type for CSR value: {type(csr_val)}"

        atlas_root = os.path.join(os.path.dirname(__file__), '..', '..')
        csr_json = os.path.join(atlas_root, 'arch', 'rv'+str(xlen), 'reg_csr.json')

        csr_defn = None
        with open(csr_json, 'r') as fin:
            csr_json = json.load(fin)
            for defn in csr_json:
                if defn['name'] == csr_name:
                    csr_defn = defn
                    break

        assert csr_defn is not None
        if 'fields' not in csr_defn:
            return

        self.fields = Fields(self.csr_value, **csr_defn['fields'])

class Fields:
    def __init__(self, csr_value, **fields):
        for field_name, field_defn in fields.items():
            high_bit = field_defn['high_bit']
            low_bit = field_defn['low_bit']
            readonly = field_defn['readonly']
            desc = field_defn['desc']

            field_val = ExtractFieldValue(csr_value, low_bit, high_bit)
            setattr(self, field_name, Field(field_name, field_val, low_bit, high_bit, readonly, desc))

class Field:
    def __init__(self, field_name, field_val, low_bit, high_bit, readonly, desc):
        self.name = field_name
        self.value = field_val
        self.bits = (low_bit, high_bit)
        self.readonly = readonly
        self.desc = desc

    def __repr__(self):
        repr  = self.name + '\n'
        repr += '   value:    ' + str(self.value.as_int()) + '\n'
        repr += '   bits:     ' + '{}-{}'.format(*self.bits) + '\n'
        repr += '   readonly: ' + ('True' if self.readonly else 'False') + '\n'
        repr += '   desc:     ' + self.desc
        return repr

def ExtractFieldValue(csr_value, low_bit, high_bit):
    # Create a mask to isolate the field's bits
    mask = (1 << (high_bit - low_bit + 1)) - 1
    
    # Shift the value to the right to align the field at the least significant bits
    field_value = (csr_value >> low_bit) & mask
    
    return field_value
