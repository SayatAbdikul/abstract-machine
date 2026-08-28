include $(AM_HOME)/scripts/isa/riscv.mk

YSYXSOC_LINKER_SCRIPT := linker-sdram-exec.ld
include $(AM_HOME)/scripts/platform/ysyxsoc.mk

COMMON_CFLAGS += -march=rv32i_zicsr_zifencei -mabi=ilp32 \
                 -DYSYXSOC_REPORT_MEMORY_LAYOUT=1
LDFLAGS       += -melf32lriscv
YSYXSOC_TEXT_PAD ?= 0
LDFLAGS       := --defsym=PHASE11_TEXT_PAD=$(YSYXSOC_TEXT_PAD) $(LDFLAGS)

AM_SRCS += riscv/npc/libgcc/div.S \
           riscv/npc/libgcc/muldi3.S \
           riscv/npc/libgcc/multi3.c \
           riscv/npc/libgcc/ashldi3.c \
           riscv/npc/libgcc/unused.c
