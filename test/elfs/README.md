# Overview

In this directory are tests for Pegasus as well as a build system for
those tests.  The tests are divided into two parts/directories:

1. `asm`: Baremetal, pure .S files
1. `linux`: Statically compiled linux binaries that use system call emulation to run

# Building

Building these tests requires a cross compiler for RISC-V.  Some
systems have these cross compilers as part of their distribution
packages and can be installed from there.  Keep in mind that there are
_two_ types of compilers needed for these tests: `linux-gnu` and
`unknown-elf`.

`linux-gnu` is required to build those tests in `linux` directory
`unknown-elf` is required to build those tests in `asm` directory (aka bare-metal)

1. Ubuntu: `apt-get install gcc-riscv64-linux-gnu gcc-riscv64-unknown-elf`
1. MacOS:
   ```
   brew tap riscv-software-src/riscv
   brew install riscv-tools
   ```
There are also pre-built tool chains here: https://github.com/stnolting/riscv-gcc-prebuilt

After getting the cross compilers installed, building the binaries
here can be as simple as typing `make`.

However, if the tools are installed somewhere else or named
differently, these variables can be overridden:

```
RV_ASM
RV_CC
RV_LINKER
```

As an example:
```
make RV_ASM=/opt/riscv/bin/riscv64-unknown-elf-as RV_CC=/usr/bin/riscv64-linux-gnu-gcc-12
```

When the programs are built, install them for testing:

```
make install
```
This copies the ELF binaries to `../sim/workloads/` for the testers to
pick up.
