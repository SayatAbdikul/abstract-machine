# Use the installed bare-metal toolchain.  Unlike Ubuntu 20.04's GCC 9
# linux-gnu cross compiler, this toolchain understands the Zicsr extension
# name used by the rv32 AbstractMachine targets.
CROSS_COMPILE := riscv64-unknown-elf-
COMMON_CFLAGS := -fno-pic -march=rv64g -mcmodel=medany -mstrict-align
CFLAGS        += $(COMMON_CFLAGS) -static
ASFLAGS       += $(COMMON_CFLAGS) -O0
LDFLAGS       += -melf64lriscv

# overwrite ARCH_H defined in $(AM_HOME)/Makefile
ARCH_H := arch/riscv.h
