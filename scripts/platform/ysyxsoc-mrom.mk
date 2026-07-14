AM_SRCS := riscv/ysyxsoc/start.S \
           riscv/ysyxsoc/trm.c \
           riscv/ysyxsoc/spi.c \
           riscv/ysyxsoc/ioe.c \
           riscv/ysyxsoc/timer.c \
           platform/dummy/cte.c \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDSCRIPTS += $(AM_HOME)/am/src/riscv/ysyxsoc/linker-mrom.ld
LDFLAGS   += --gc-sections -e _start -Map=$(IMAGE).map

MAINARGS_MAX_LEN = 256
MAINARGS_PLACEHOLDER = the_insert_arg_rule_in_ysyxsoc_mrom_mk_will_replace_this_text
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN)
CFLAGS += -DMAINARGS_PLACEHOLDER=$(MAINARGS_PLACEHOLDER)
PYTHON ?= python3

NPC_HOME ?= $(AM_HOME)/../npc
YSYXSOC_RUN_ARGS ?= --cycles=2000000 --reset-cycles=20

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S -O binary $(IMAGE).elf $(IMAGE).bin
	@$(PYTHON) $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin \
		$(MAINARGS_MAX_LEN) $(MAINARGS_PLACEHOLDER) "$(mainargs)"

run: image
	$(MAKE) -C $(NPC_HOME) ysyxsoc-mrom-build
	$(NPC_HOME)/build/ysyxsoc-mrom/VysyxSoCFull \
		--mrom=$(IMAGE).bin \
		$(YSYXSOC_RUN_ARGS)
