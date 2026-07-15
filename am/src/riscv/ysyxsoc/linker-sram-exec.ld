ENTRY(_start)

MEMORY {
  sram (rwx) : ORIGIN = 0x0f000000, LENGTH = 8K
}

_sram_start = ORIGIN(sram);
_sram_end = ORIGIN(sram) + LENGTH(sram);
_stack_bottom = _sram_end - 0x400;
_stack_pointer = _sram_end;

SECTIONS {
  . = ORIGIN(sram);

  .text : {
    _text_start = .;
    KEEP(*(entry))
    *(.text*)
    _text_end = .;
  } > sram

  .rodata : {
    . = ALIGN(4);
    _rodata_start = .;
    *(.rodata*)
    *(.srodata*)
    *(.eh_frame*)
    *(.gcc_except_table*)
    _rodata_end = .;
  } > sram

  .copy_table (READONLY) : {
    . = ALIGN(4);
    _copy_table_start = .;
    LONG(LOADADDR(.data))
    LONG(ADDR(.data))
    LONG(SIZEOF(.data))
    _copy_table_end = .;
  } > sram

  .zero_table (READONLY) : {
    . = ALIGN(4);
    _zero_table_start = .;
    LONG(ADDR(.bss))
    LONG(SIZEOF(.bss))
    _zero_table_end = .;
  } > sram

  .data : {
    . = ALIGN(4);
    _data_start = .;
    *(.data*)
    *(.sdata*)
    *(.got*)
    *(.init_array*)
    *(.fini_array*)
    _data_end = .;
  } > sram

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(COMMON)
    _bss_end = .;
  } > sram

  . = ALIGN(16);
  _image_end = .;
  _heap_start = _image_end;
  _heap_end = _image_end;
}

ASSERT(_text_start == _sram_start, "SRAM payload must start at SRAM base")
ASSERT(_rodata_start >= _text_end, "text and rodata overlap")
ASSERT(_copy_table_start >= _rodata_end, "rodata and copy table overlap")
ASSERT(_zero_table_start >= _copy_table_end, "copy and zero tables overlap")
ASSERT(_data_start >= _zero_table_end, "startup tables and data overlap")
ASSERT(_data_end <= _bss_start, "data and bss overlap")
ASSERT(_bss_end <= _image_end, "bss extends beyond SRAM image")
ASSERT(_image_end <= _stack_bottom,
       "SRAM payload exceeds available SRAM below the 1 KiB stack")
ASSERT(_stack_pointer - _stack_bottom == 0x400, "stack must be 1 KiB")
ASSERT(_stack_pointer <= _sram_end, "stack ends above SRAM")
