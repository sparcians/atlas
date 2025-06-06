RV32ZBC_INST = [
    {'mnemonic': 'clmul', 'handler': 'clmul', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulh', 'handler': 'clmulh', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulr', 'handler': 'clmulr', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBC_INST = [
    {'mnemonic': 'clmul', 'handler': 'clmul', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulh', 'handler': 'clmulh', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'clmulr', 'handler': 'clmulr', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]
