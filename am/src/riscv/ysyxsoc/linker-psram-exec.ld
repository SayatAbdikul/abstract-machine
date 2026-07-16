ENTRY(_start)

MEMORY {
  sram (rw)   : ORIGIN = 0x0f000000, LENGTH = 8K
  psram (rwx) : ORIGIN = 0x80000000, LENGTH = 4M
}

_sram_start = ORIGIN(sram);
_sram_end = ORIGIN(sram) + LENGTH(sram);
_psram_start = ORIGIN(psram);
_psram_end = ORIGIN(psram) + LENGTH(psram);

/* The staged bootloader and the application deliberately share this bounded
 * stack. The SSBL has stopped using it before the payload entry is called. */
_stack_bottom = _sram_end - 0x400;
_stack_pointer = _sram_end;

SECTIONS {
  . = ORIGIN(psram);
  _image_start = .;

  .text : {
    _text_start = .;
    KEEP(*(entry))
    *(.text*)
    _text_end = .;
  } > psram

  .rodata : {
    . = ALIGN(4);
    _rodata_start = .;
    *(.rodata*)
    *(.srodata*)
    *(.eh_frame*)
    *(.gcc_except_table*)
    _rodata_end = .;
  } > psram

  /* File-backed payloads are already copied directly to their VMAs by the
   * SSBL, so there is no data relocation entry here. */
  .copy_table (READONLY) : {
    . = ALIGN(4);
    _copy_table_start = .;
    _copy_table_end = .;
  } > psram

  .zero_table (READONLY) : {
    . = ALIGN(4);
    _zero_table_start = .;
    LONG(ADDR(.bss))
    LONG(SIZEOF(.bss))
    _zero_table_end = .;
  } > psram

  .data : {
    . = ALIGN(4);
    _data_start = .;
    *(.data*)
    *(.sdata*)
    *(.got*)
    *(.init_array*)
    *(.fini_array*)
    _data_end = .;
  } > psram

  . = ALIGN(4);
  _image_file_end = .;

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(COMMON)
    _bss_end = .;
  } > psram

  . = ALIGN(16);
  _image_end = .;
  _heap_start = _image_end;
  _heap_end = _psram_end;
  _entry = _start;
}

ASSERT(_image_start == _psram_start,
       "PSRAM executable image must start at the physical PSRAM base")
ASSERT(_text_start == _image_start, "entry text must start the PSRAM image")
ASSERT(_rodata_start >= _text_end, "text and rodata overlap")
ASSERT(_copy_table_start >= _rodata_end, "rodata and copy table overlap")
ASSERT(_zero_table_start >= _copy_table_end,
       "copy and zero tables overlap")
ASSERT(_data_start >= _zero_table_end, "startup tables and data overlap")
ASSERT(_data_end <= _image_file_end, "data extends beyond file image")
ASSERT(_image_file_end <= _bss_start, "file image and BSS overlap")
ASSERT(_bss_end <= _image_end, "BSS extends beyond memory image")
ASSERT(_image_end <= _heap_end, "PSRAM executable image exceeds capacity")
ASSERT(_heap_start <= _heap_end, "PSRAM heap overflow")
ASSERT(_entry >= _image_start && _entry < _image_file_end,
       "payload entry is outside the executable file image")

ASSERT(_stack_bottom >= _sram_start, "stack begins below SRAM")
ASSERT(_stack_pointer <= _sram_end, "stack ends above SRAM")
ASSERT(_stack_pointer - _stack_bottom == 0x400, "stack must be 1 KiB")
