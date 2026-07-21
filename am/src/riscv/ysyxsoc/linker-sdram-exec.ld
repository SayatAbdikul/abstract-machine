ENTRY(_start)

MEMORY {
  sram  (rw)  : ORIGIN = 0x0f000000, LENGTH = 8K
  sdram (rwx) : ORIGIN = 0xa0000000, LENGTH = 32M
}

_sram_start = ORIGIN(sram);
_sram_end = ORIGIN(sram) + LENGTH(sram);
_sdram_start = ORIGIN(sdram);
_sdram_end = ORIGIN(sdram) + LENGTH(sdram);
_stack_bottom = _sram_end - 0x400;
_stack_pointer = _sram_end;
PHASE11_TEXT_PAD = DEFINED(PHASE11_TEXT_PAD) ? PHASE11_TEXT_PAD : 0;

SECTIONS {
  . = ORIGIN(sdram);
  _image_start = .;
  .phase11_text_pad ORIGIN(sdram) : {
    /* Emit real file bytes so ld cannot collapse the requested alignment.
     * The fixed 16-byte prefix keeps pad=0 on a cache-block boundary. */
    BYTE(0)
  } > sdram
  .text ORIGIN(sdram) + 16 + PHASE11_TEXT_PAD : {
    _text_start = .;
    KEEP(*(entry))
    *(.text*)
    _text_end = .;
  } > sdram
  .rodata : {
    . = ALIGN(4); _rodata_start = .;
    *(.rodata*) *(.srodata*) *(.eh_frame*) *(.gcc_except_table*)
    _rodata_end = .;
  } > sdram
  .copy_table (READONLY) : {
    . = ALIGN(4); _copy_table_start = .; _copy_table_end = .;
  } > sdram
  .zero_table (READONLY) : {
    . = ALIGN(4); _zero_table_start = .;
    /* The SSBL already clears ELF memsz-filesz for staged payloads. */
    _zero_table_end = .;
  } > sdram
  .data : {
    . = ALIGN(4); _data_start = .;
    __fsymtab_start = .; KEEP(*(FSymTab)) __fsymtab_end = .;
    __vsymtab_start = .; KEEP(*(VSymTab)) __vsymtab_end = .;
    __rt_init_start = .; KEEP(*(SORT(.rti_fn*))) __rt_init_end = .;
    __rt_utest_tc_tab_start = .; KEEP(*(UtestTcTab))
    __rt_utest_tc_tab_end = .;
    __am_apps_data_start = .;
    *(__am_apps.data*) *(__am_apps.sdata*)
    __am_apps_data_end = .;
    *(.data*) *(.sdata*) *(.got*) *(.init_array*) *(.fini_array*)
    _data_end = .;
  } > sdram
  . = ALIGN(4); _image_file_end = .;
  .bss (NOLOAD) : {
    . = ALIGN(4); _bss_start = .;
    . = ALIGN(8);
    __am_apps_bss_start = .;
    *(__am_apps.bss*) *(__am_apps.sbss*) *(__am_apps.scommon*)
    __am_apps_bss_end = .;
    . = ALIGN(8);
    *(.bss*) *(.sbss*) *(COMMON) _bss_end = .;
  } > sdram
  . = ALIGN(16); _image_end = .;
  _heap_start = _image_end;
  _heap_end = _sdram_end;
  _entry = _start;
}

ASSERT(_image_start == _sdram_start, "SDRAM image must start at 0xa0000000")
ASSERT(PHASE11_TEXT_PAD >= 0 && PHASE11_TEXT_PAD < 16,
       "Phase 11 text padding must be in [0, 16)")
ASSERT((PHASE11_TEXT_PAD & 3) == 0,
       "Phase 11 text padding must preserve instruction alignment")
ASSERT(_text_start == _image_start + 16 + PHASE11_TEXT_PAD,
       "entry text padding differs from PHASE11_TEXT_PAD")
ASSERT(_image_file_end <= _bss_start, "file image and BSS overlap")
ASSERT(_bss_end <= _image_end, "BSS extends beyond memory image")
ASSERT(_image_end <= _heap_end, "SDRAM executable image exceeds capacity")
ASSERT(_entry >= _image_start && _entry < _image_file_end,
       "payload entry is outside the executable file image")
ASSERT(_stack_bottom >= _sram_start, "stack begins below SRAM")
ASSERT(_stack_pointer <= _sram_end, "stack ends above SRAM")
ASSERT(_stack_pointer - _stack_bottom == 0x400, "stack must be 1 KiB")
