RV32ZBA_INST = [
    {'mnemonic': 'sh1add', 'handler': 'sh1add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'sh2add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'sh3add', 'cost': 1, 'tags': 'B_EXT_32', 'memory': False, 'cof': False},
]

RV64ZBA_INST = [
    {'mnemonic': 'add.uw', 'handler': 'add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add', 'handler': 'sh1add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh1add.uw', 'handler': 'sh1add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add', 'handler': 'sh2add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh2add.uw', 'handler': 'sh2add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add', 'handler': 'sh3add', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'sh3add.uw', 'handler': 'sh3add.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
    {'mnemonic': 'slli.uw', 'handler': 'slli.uw', 'cost': 1, 'tags': 'B_EXT_64', 'memory': False, 'cof': False},
]
