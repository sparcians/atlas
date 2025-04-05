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

add_custom_command(
  OUTPUT ${AUTOGEN_REGISTER_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  DEPENDS ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
)

set(AUTOGEN_RV32_INST_JSON_FILES
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32i.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32m.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32a.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32f.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32d.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32Zicsr.json
  ${SIM_BASE}/arch/rv32/atlas_uarch_rv32Zifencei.json
)

set(AUTOGEN_RV64_INST_JSON_FILES
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64i.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64m.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64a.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64f.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64d.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64Zicsr.json
  ${SIM_BASE}/arch/rv64/atlas_uarch_rv64Zifencei.json
)

set(AUTOGEN_INST_FILES
  ${AUTOGEN_RV32_INST_JSON_FILES}
  ${AUTOGEN_RV64_INST_JSON_FILES}
)

set(DEPENDS_INST_JSON_FILES
    ${SIM_BASE}/scripts/GenInstructionJSON.py
    ${SIM_BASE}/scripts/insts/RVI_INST.py
    ${SIM_BASE}/scripts/insts/RVM_INST.py
    ${SIM_BASE}/scripts/insts/RVA_INST.py
    ${SIM_BASE}/scripts/insts/RVF_INST.py
    ${SIM_BASE}/scripts/insts/RVD_INST.py
    ${SIM_BASE}/scripts/insts/RVZICSR_INST.py
    ${SIM_BASE}/scripts/insts/RVZIFENCEI_INST.py
)

add_custom_command(
  OUTPUT ${AUTOGEN_RV32_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv32imafd_Zicsr_Zifencei
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

add_custom_command(
  OUTPUT ${AUTOGEN_RV64_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv64imafd_Zicsr_Zifencei
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

add_custom_target(AutogenArchFiles ALL DEPENDS ${AUTOGEN_CSR_HEADERS} ${AUTOGEN_REGISTER_FILES} ${AUTOGEN_INST_FILES})
