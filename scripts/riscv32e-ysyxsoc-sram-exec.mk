include $(AM_HOME)/scripts/isa/riscv.mk

YSYXSOC_LINKER_SCRIPT := linker-sram-exec.ld
include $(AM_HOME)/scripts/platform/ysyxsoc.mk

COMMON_CFLAGS += -march=rv32e_zicsr -mabi=ilp32e \
                 -DYSYXSOC_REPORT_MEMORY_LAYOUT=1 \
                 -DMEMTEST_SRAM_BASIC_ONLY=1
CFLAGS        += -Os
LDFLAGS       += -melf32lriscv

AM_SRCS += riscv/npc/libgcc/div.S \
           riscv/npc/libgcc/muldi3.S \
           riscv/npc/libgcc/multi3.c \
           riscv/npc/libgcc/ashldi3.c \
           riscv/npc/libgcc/unused.c
