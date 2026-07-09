AM_SRCS := riscv/ysyxsoc/start.S \
           riscv/ysyxsoc/trm.c \
           platform/dummy/ioe.c \
           platform/dummy/cte.c \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDSCRIPTS += $(AM_HOME)/am/src/riscv/ysyxsoc/linker.ld
LDFLAGS   += --gc-sections -e _start

NPC_HOME ?= $(AM_HOME)/../npc
YSYXSOC_RUN_ARGS ?= --cycles=20000 --reset-cycles=20

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S -O binary $(IMAGE).elf $(IMAGE).bin

run: image
	$(MAKE) -C $(NPC_HOME) ysyxsoc-build
	$(NPC_HOME)/build/ysyxsoc/VysyxSoCFull \
		--mrom=$(IMAGE).bin \
		$(YSYXSOC_RUN_ARGS)
