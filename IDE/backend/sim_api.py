import json
import backend.c_dtypes as c_dtypes

def FormatHex(val):
    if val is None:
        return 'NULL'

    if isinstance(val, int):
        return FormatHex(hex(val))

    assert isinstance(val, str)

    if val.startswith('-0x'):
        val = val.lstrip('-0x')
        val = val.lstrip('0')
        if val == '':
            return '-0x0'
        else:
            return '-0x' + val

    if val.startswith('0x'):
        val = val.lstrip('0x')
        val = val.lstrip('0')
        if val == '':
            return '0x0'
        else:
            return '0x' + val

    if val == 'NULL':
        return 'NULL'

    return hex(int(val))

class SimResponse:
    def __init__(self, response=None):
        self.response = response

class AckResponse(SimResponse):
    def __bool__(self):
        return True

class ErrorResponse(SimResponse):
    def __bool__(self):
        return False

class WarningResponse(SimResponse):
    def __bool__(self):
        return False

class BrokenPipeResponse(SimResponse):
    def __bool__(self):
        return False

# Equiv C++:  PegasusState::getXlen()
def pegasus_xlen(endpoint):
    return endpoint.request('state.xlen')

# Equiv C++:  PegasusState::getPc()
def pegasus_pc(endpoint):
    pc = endpoint.request('state.pc')
    if type(pc) in (int, str):
        pc = FormatHex(pc)

    return pc

# Equiv C++:  PegasusState::getPrevPc()
def pegasus_prev_pc(endpoint):
    pc = endpoint.request('state.prev_pc')
    if type(pc) in (int, str):
        pc = FormatHex(pc)

    return pc

# Equiv C++:  PegasusState::getSimState()->current_uid
def pegasus_current_uid(endpoint):
    return endpoint.request('state.current_uid')

# Equiv C++:  PegasusState::getSimState()->workload_exit_code
def pegasus_exit_code(endpoint):
    return endpoint.request('state.exit_code')

# Equiv C++:  PegasusState::getSimState()->test_passed
def pegasus_test_passed(endpoint):
    return endpoint.request('state.test_passed')

# Equiv C++:  PegasusState::getSimState()->sim_stopped
def pegasus_sim_stopped(endpoint):
    return endpoint.request('state.sim_stopped')

# Equiv C++:  PegasusState::getSimState()->inst_count
def pegasus_inst_count(endpoint):
    return endpoint.request('state.inst_count')

# Equiv C++:  PegasusState::getCurrentInst()->getUid()
def pegasus_inst_uid(endpoint):
    return endpoint.request('inst.uid')

# Equiv C++:  PegasusState::getCurrentInst()->getMnemonic()
def pegasus_inst_mnemonic(endpoint):
    return endpoint.request('inst.mnemonic')

# Equiv C++:  PegasusState::getCurrentInst()->dasmString()
def pegasus_inst_dasm_string(endpoint):
    return endpoint.request('inst.dasm_string')

# Equiv C++:  PegasusState::getCurrentInst()->getOpcode()
def pegasus_inst_opcode(endpoint):
    opcode = endpoint.request('inst.opcode')
    if type(opcode) in (int, str):
        opcode = FormatHex(opcode)

    return opcode

# Equiv C++:  PegasusState::getCurrentInst()->getPrivMode()
def pegasus_inst_priv(endpoint):
    return endpoint.request('inst.priv')

# Equiv C++:  PegasusState::getCurrentInst()->hasImmediate()
def pegasus_inst_has_immediate(endpoint):
    return endpoint.request('inst.has_immediate')

# Equiv C++:  PegasusState::getCurrentInst()->getImmediate()
def pegasus_inst_immediate(endpoint):
    imm = endpoint.request('inst.immediate')
    if type(imm) in (int, str):
        imm = FormatHex(imm)

    return imm

# Equiv C++:  PegasusState::getCurrentInst()->getRs1()->getName()
def pegasus_inst_rs1_name(endpoint):
    rs1_name = endpoint.request('inst.rs1.name')
    if isinstance(rs1_name, str) and rs1_name != '':
        return rs1_name

    return None

# Equiv C++:  PegasusState::getCurrentInst()->getRs2()->getName()
def pegasus_inst_rs2_name(endpoint):
    rs2_name = endpoint.request('inst.rs2.name')
    if isinstance(rs2_name, str) and rs2_name != '':
        return rs2_name

    return None

# Equiv C++:  PegasusState::getCurrentInst()->getRd()->getName()
def pegasus_inst_rd_name(endpoint):
    rd_name = endpoint.request('inst.rd.name')
    if isinstance(rd_name, str) and rd_name != '':
        return rd_name

    return None

# Equiv C++:  PegasusState::getCurrentInst()->getExceptionUnit()->getUnhandledException()
def pegasus_inst_active_exception(endpoint):
    return endpoint.request('inst.active_exception')

# Equiv C++:  PegasusState::get[Int|Fp|Vec|Csr]RegisterSet()->getNumRegisters()
def pegasus_num_regs_in_group(endpoint, group_num):
    return endpoint.request('state.num_regs_in_group %d' % group_num)

# Equiv C++:  PegasusState::getCsrRegisterSet()->getRegister(csr_num)->getName()
def pegasus_csr_name(endpoint, csr_num):
    return endpoint.request('csr.name %d' % csr_num)

# Equiv C++:  PegasusState::findRegister(reg_name)->getGroupNum()
def pegasus_reg_group_num(endpoint, reg_name):
    return endpoint.request('reg.group_num %s' % reg_name)

# Equiv C++:  PegasusState::findRegister(reg_name)->getID()
def pegasus_reg_id(endpoint, reg_name):
    return endpoint.request('reg.reg_id %s' % reg_name)

# Equiv C++:  PegasusState::findRegister(reg_name)->read()
def pegasus_reg_value(endpoint, reg_name):
    reg_value = endpoint.request('reg.value %s' % reg_name)
    if type(reg_value) in (int, str):
        reg_value = FormatHex(reg_value)

    return reg_value

# Equiv C++:  PegasusState::findRegister(reg_name)->write(value)
def pegasus_reg_write(endpoint, reg_name, value):
    value = c_dtypes.convert_to_hex(value)
    return endpoint.request('reg.write %s %s' % (reg_name, value))

# Equiv C++:  PegasusState::findRegister(reg_name)->dmiWrite<uint64_t>(value)
def pegasus_reg_dmiwrite(endpoint, reg_name, value):
    value = c_dtypes.convert_to_hex(value)
    return endpoint.request('reg.dmiwrite %s %s' % (reg_name, value))

# Equiv C++:  This goes through the PegasusState's SimController observer,
#             whose Observer base class holds a vector of these structs:
#
#                 struct MemRead
#                 {
#                     Addr addr;
#                     size_t size;
#                     uint64_t value;
#                 };
#
# This is intended to be called during postExecute() where the memory reads/writes
# have occurred during the inst handler method.
#
# Returns a list of c_dtypes.MemRead objects.
#
def pegasus_mem_reads(endpoint):
    reads = []
    ans = endpoint.request('mem.reads')
    if not isinstance(ans, str):
        return reads

    for pair in ans.split(','):
        if not pair:
            continue

        addr, value = pair.split()
        addr = FormatHex(addr.strip())
        value = FormatHex(value.strip())
        reads.append(c_dtypes.MemRead(addr, value))

    return reads

# Equiv C++:  This goes through the PegasusState's SimController observer,
#             whose Observer base class holds a vector of these structs:
#
#                 struct MemWrite
#                 {
#                     Addr addr;
#                     size_t size;
#                     uint64_t value;
#                     uint64_t prior_value;
#                 };
#
# This is intended to be called during postExecute() where the memory reads/writes
# have occurred during the inst handler method.
#
# Returns a list of c_dtypes.MemWrite objects.
#
def pegasus_mem_writes(endpoint):
    writes = []
    ans = endpoint.request('mem.writes')
    if not isinstance(ans, str):
        return writes

    for trip in ans.split(','):
        if not trip:
            continue

        addr, prior, new = trip.split()
        addr = FormatHex(addr.strip())
        prior = FormatHex(prior.strip())
        new = FormatHex(new.strip())
        writes.append(c_dtypes.MemWrite(addr, prior, new))

    return writes

# Equiv C++:  PegasusState->getCurrentInst()
#                       ->getMavisOpcodeInfo()
#                       ->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
def pegasus_inst_csr(endpoint):
    return endpoint.request('inst.csr.name')

# Equiv C++:  PegasusState->getCurrentInst()
#                       ->getMavisOpcodeInfo()
#                       ->getInstType();
def pegasus_inst_type(endpoint):
    return endpoint.request('inst.type')


# Stop Pegasus from executing C++ instruction handler code and jump right
# to the finish ActionGroup (increment PC, post_execute). This is useful
# for when Python code is to be injected into the simulation in place of
# the C++ code.
#
# For example, you could write an observer to tryout a new instruction
# implementation or to debug your simulator in Python for rapid prototyping:
#
#     class PythonInstRewriter(Observer):
#         def OnPreExecute(self, endpoint):
#             insn = pegasus_current_inst(endpoint)
#             insn.getRd().write(insn.getRs1().read() * insn.getRs2().read())
#
#             # Tell Pegasus to skip the C++ inst implementation.
#             pegasus_finish_execute(endpoint)
#
def pegasus_finish_execute(endpoint):
    return endpoint.request('sim.finish_execute', 'sim_dead')

# Set a breakpoint at the given execute phase.
# Must be one of pre_execute, pre_exception, or post_execute.
def pegasus_break_action(endpoint, action):
    return endpoint.request('sim.break %s' % action)

# Continue the running simulation until the next breakpoint is hit
# or the simulation finishes.
#
# Returns the simulation/execution phase that the simulator is in
# which is one of pre_execute, pre_exception, post_execute, or
# sim_finished.
def pegasus_continue(endpoint):
    return endpoint.request('sim.continue', 'sim_dead')

# Finish the simulation now. The simulation will complete and the
# simulator will exit. This invalidates further use of the endpoint.
def pegasus_finish_sim(endpoint):
    return endpoint.request('sim.finish', 'sim_dead')

# Terminate a simulation early for any reason with the given exit code.
# This invalidates further use of the endpoint. The simulator's SimState
# will reflect the following:
#
#   sim_state.workload_exit_code = exit_code
#   sim_state.test_passed = false
#   sim_state.sim_stopped = true
def pegasus_kill_sim(endpoint, exit_code):
    return endpoint.request('sim.kill %d' % exit_code)

### ====================================================================
### JSON conversion (IPC using JSON messages)
class JsonConverter:
    @classmethod
    def ConvertResponse(cls, response):
        obj = json.loads(response.replace('\t', '  '))
        response_code = obj['response_code']

        if response_code == 'err':
            return cls._ConvertError(obj)
        elif response_code == 'warn':
            return cls._ConvertWarning(obj)
        elif response_code == 'ok':
            return cls._ConvertResponse(obj)

        return None

    @classmethod
    def _ConvertError(cls, obj):
        assert 'response_type' not in obj or obj['response_type'] == 'str'
        errmsg = obj['response_payload']
        return ErrorResponse(errmsg)

    @classmethod
    def _ConvertWarning(cls, obj):
        assert 'response_type' not in obj or obj['response_type'] == 'str'
        errmsg = obj['response_payload']
        return WarningResponse(errmsg)

    @classmethod
    def _ConvertResponse(cls, obj):
        if 'response_payload' not in obj:
            return AckResponse(None)

        response_payload = obj['response_payload']
        if 'response_type' not in obj:
            return AckResponse(response_payload)

        response_type = obj['response_type']

        if response_type == 'int':
            return int(response_payload)

        if response_type == 'float':
            return float(response_payload)

        if response_type == 'str':
            return response_payload

        if response_type == 'bool':
            return response_payload
