ENTRY(_start)

MEMORY {
  flash (rx)  : ORIGIN = 0x30000000, LENGTH = 16M
  sram (rwx)  : ORIGIN = 0x0f000000, LENGTH = 8K
  psram (rwx) : ORIGIN = 0x80000000, LENGTH = 4M
}

_flash_start = ORIGIN(flash);
_flash_end = ORIGIN(flash) + LENGTH(flash);
_sram_start = ORIGIN(sram);
_sram_end = ORIGIN(sram) + LENGTH(sram);
_psram_start = ORIGIN(psram);
_psram_end = ORIGIN(psram) + LENGTH(psram);

SECTIONS {
  . = ORIGIN(flash);

  .text : {
    _text_start = .;
    KEEP(*(entry))
    *(.text*)
    _text_end = .;
  } > flash

  .rodata : {
    _rodata_start = .;
    *(.rodata*)
    *(.srodata*)
    *(.eh_frame*)
    *(.gcc_except_table*)
    _rodata_end = .;
  } > flash

  .copy_table (READONLY) : {
    . = ALIGN(4);
    _copy_table_start = .;
    LONG(LOADADDR(.data))
    LONG(ADDR(.data))
    LONG(SIZEOF(.data))
    _copy_table_end = .;
  } > flash

  .zero_table (READONLY) : {
    . = ALIGN(4);
    _zero_table_start = .;
    LONG(ADDR(.bss))
    LONG(SIZEOF(.bss))
    _zero_table_end = .;
  } > flash

  .data : {
    . = ALIGN(4);
    _data_start = .;
    *(.data*)
    *(.sdata*)
    *(.got*)
    *(.init_array*)
    *(.fini_array*)
    _data_end = .;
  } > psram AT> flash

  _data_load = LOADADDR(.data);
  _flash_load_end = _data_load + SIZEOF(.data);

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(COMMON)
    _bss_end = .;
  } > psram

  . = ALIGN(16);
  _heap_start = ALIGN(16);
  _heap_end = _psram_end;

  _stack_bottom = _sram_end - 0x400;
  _stack_pointer = _sram_end;
}

ASSERT(_text_start >= _flash_start, "text begins below flash")
ASSERT(_rodata_start >= _text_end, "text and rodata overlap")
ASSERT(_copy_table_start >= _rodata_end, "rodata and copy table overlap")
ASSERT(_zero_table_start >= _copy_table_end, "copy and zero tables overlap")
ASSERT(_data_load >= _zero_table_end, "startup tables and data load image overlap")
ASSERT(_flash_load_end <= _flash_end, "flash load image overflow")

ASSERT(_data_start >= _psram_start, "data begins below PSRAM")
ASSERT(_data_end <= _bss_start, "data and bss overlap")
ASSERT(_bss_end <= _heap_start, "bss and heap overlap")
ASSERT(_heap_start <= _heap_end, "PSRAM heap overflow")
ASSERT(_heap_end <= _psram_end, "PSRAM capacity overflow")

ASSERT(_stack_bottom >= _sram_start, "stack begins below SRAM")
ASSERT(_stack_pointer <= _sram_end, "stack ends above SRAM")
ASSERT(_stack_bottom < _stack_pointer, "empty SRAM stack")
ASSERT(_stack_pointer - _stack_bottom == 0x400, "stack must be 1 KiB")
