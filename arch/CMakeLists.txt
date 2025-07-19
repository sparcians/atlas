set(AUTOGEN_RV32_REG_JSON_FILES
  ${SIM_BASE}/arch/rv32/reg_csr.json
  ${SIM_BASE}/arch/rv32/reg_fp.json
  ${SIM_BASE}/arch/rv32/reg_int.json
  ${SIM_BASE}/arch/rv32/reg_vec.json
)

set(AUTOGEN_RV64_REG_JSON_FILES
  ${SIM_BASE}/arch/rv64/reg_csr.json
  ${SIM_BASE}/arch/rv64/reg_fp.json
  ${SIM_BASE}/arch/rv64/reg_int.json
  ${SIM_BASE}/arch/rv64/reg_vec.json
)

set(AUTOGEN_CSR_HEADERS
  ${SIM_BASE}/include/CSRNums.hpp
  ${SIM_BASE}/include/CSRBitMasks32.hpp
  ${SIM_BASE}/include/CSRFieldIdxs32.hpp
  ${SIM_BASE}/include/CSRBitMasks64.hpp
  ${SIM_BASE}/include/CSRFieldIdxs64.hpp
)

set(AUTOGEN_REGISTER_FILES
  ${AUTOGEN_RV32_REG_JSON_FILES}
  ${AUTOGEN_RV64_REG_JSON_FILES}
  ${AUTOGEN_CSR_HEADERS}
)

set(DEPENDS_REGISTER_FILES
  ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  ${SIM_BASE}/scripts/RV32_CSR.py
  ${SIM_BASE}/scripts/RV64_CSR.py
  ${SIM_BASE}/scripts/REG_CONSTS.py
)

add_custom_command(
  OUTPUT ${AUTOGEN_REGISTER_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  DEPENDS ${DEPENDS_REGISTER_FILES}
)

set(AUTOGEN_RV32_INST_JSON_FILES
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32i.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32m.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32a.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32f.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32d.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32zicsr.json
  ${SIM_BASE}/arch/rv32/pegasus_uarch_rv32zifencei.json
)

set(AUTOGEN_RV64_INST_JSON_FILES
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64i.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64m.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64a.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64f.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64d.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64zicsr.json
  ${SIM_BASE}/arch/rv64/pegasus_uarch_rv64zifencei.json
)

set(AUTOGEN_INST_FILES
  ${AUTOGEN_RV32_INST_JSON_FILES}
  ${AUTOGEN_RV64_INST_JSON_FILES}
)

file(GLOB DEPENDS_INST_JSON_FILES ${SIM_BASE}/scripts/insts/RV*.py)

add_custom_command(
  OUTPUT ${AUTOGEN_RV32_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv32imafdbv_zicsr_zifencei_zicond
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

add_custom_command(
  OUTPUT ${AUTOGEN_RV64_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv64imafdbv_zicsr_zifencei_zicond
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

add_custom_target(AutogenArchFiles ALL DEPENDS ${AUTOGEN_REGISTER_FILES} ${AUTOGEN_INST_FILES})
