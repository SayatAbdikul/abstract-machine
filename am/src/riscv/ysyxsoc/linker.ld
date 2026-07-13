ENTRY(_start)

MEMORY {
  mrom (rx)  : ORIGIN = 0x20000000, LENGTH = 4K
  sram (rwx) : ORIGIN = 0x0f000000, LENGTH = 8K
}

SECTIONS {
  . = ORIGIN(mrom);

  .text : {
    KEEP(*(entry))
    *(.text*)
  } > mrom

  .rodata : {
    *(.rodata*)
    *(.srodata*)
  } > mrom

  .data : {
    _data_start = .;
    *(.data*)
    *(.sdata*)
    _data_end = .;
  } > sram AT> mrom

  _data_load = LOADADDR(.data);

  .bss (NOLOAD) : {
    _bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(COMMON)
    _bss_end = .;
  } > sram

  . = ALIGN(4);
  _heap_start = .;
  _heap_end = ORIGIN(sram) + LENGTH(sram) - 0x400;
  _stack_pointer = ORIGIN(sram) + LENGTH(sram);
}

ASSERT(_heap_start <= _heap_end, "SRAM overflow")
