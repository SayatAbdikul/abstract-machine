.section entry, "ax"
.globl _start
.type _start, @function

_start:
  mv s0, zero
  la sp, _stack_pointer

  # Copy every initialized region described by the linker. Each entry is
  # {load address, runtime address, byte count}.
  la t0, _copy_table_start
  la t1, _copy_table_end

1:
  beq t0, t1, 4f
  lw a0, 0(t0)
  lw a1, 4(t0)
  lw a2, 8(t0)
  addi t0, t0, 12

2:
  beqz a2, 1b
  lbu a3, 0(a0)
  sb a3, 0(a1)
  addi a0, a0, 1
  addi a1, a1, 1
  addi a2, a2, -1
  j 2b

  # Clear every zero-initialized region. Each entry is
  # {runtime address, byte count}.
4:
  la t0, _zero_table_start
  la t1, _zero_table_end

5:
  beq t0, t1, 8f
  lw a0, 0(t0)
  lw a1, 4(t0)
  addi t0, t0, 8

6:
  beqz a1, 5b
  sb zero, 0(a0)
  addi a0, a0, 1
  addi a1, a1, -1
  j 6b

8:
  call _trm_init

9:
  j 9b

.size _start, . - _start
