.section entry, "ax"
.globl _start
.type _start, @function

_start:
  mv s0, zero
  la sp, _stack_pointer

  # Copy initialized .data from MROM load address to SRAM runtime address.
  la a0, _data_load
  la a1, _data_start
  la a2, _data_end

1:
  beq a1, a2, 2f
  lbu a3, 0(a0)
  sb a3, 0(a1)
  addi a0, a0, 1
  addi a1, a1, 1
  j 1b

  # Clear .bss in SRAM.
2:
  la a0, _bss_start
  la a1, _bss_end

3:
  beq a0, a1, 4f
  sb zero, 0(a0)
  addi a0, a0, 1
  j 3b

4:
  call _trm_init

5:
  j 5b

.size _start, . - _start
