
#define HEAD_DATA_ADDR 0
#define HEAD_DATA_END 1116
#define HEAP_ADDR 1116
#define HEAP_END 10044
#define STACK_ADDR 10044
#define STACK_END 15900
#define MATH_ADDR 15900
#define MATH_END 16000

const char picoAsmHeader[] = {
    R"(

### RP2350 Image Definition Block
# This block is required for the RP2350 to recognize the binary as a valid
# RISC-V executable. It must appear within the first 4 KB of the image.
# RP2350 data sheet: 5.9.1. Blocks and block loops
# 5.9.3. Image definition items, 5.9.3.4. ENTRY_POINT item
.p2align 8       # Align to 8-byte boundary (required for RP2350 image header)
.word 0xffffded3 # Header magic number (identifies the image)
.word 0x11010142 # Item 0 flags: 0b0001 0001 0000 0001 0000 0100 0010
          # IMAGE_TYPE_EXE, EXE_SECURITY_UNSPECIFIED,
          # EXE_CPU_RISC,  EXE_CHIP_RP2350
.word 0x00000344 # Item 1: Block size = 3,
          # Block type = PICOBIN_BLOCK_ITEM_1BS_ENTRY_POINT
.word __StackTop # Item 3: Initial SP address (aka stack pointer)
.word 0x000004ff # Item 4: Optional SP limit address (aka stack limit)
.word 0x00000000 # Link: Single block loop has 0 items
.word 0xab123579 # Footer magic number

.equ IO_BANK0_BASE,   0x40028000    # IO Bank0 Base Address
.equ SIO_BASE,        0xD0000000    # SIO (Single-Cycle I/O) Base Address
.equ GPIO_OUT_XOR,    SIO_BASE + 0x28   # GPIO output XOR register (atomic)
.equ GPIO_DIR,        SIO_BASE + 0x30   # GPIO direction register
.equ PADS_BANK0_BASE, 0x40038000    # User Bank Pad Control register base

.section .text          # Code section
.global main            # Make 'main' visible to the linker

main:
li sp, 0x20000000

mv gp,sp
li t0,10044 #stack start addr
add sp,sp,t0
mv t0,sp

# needed to zero out memory, comment out if memory is zeroed
mv t1, gp
li t2, 1116 #heap end addr
add t2, t1, t2
clearHeap:
  sw x0, 0(t1)
  addi t1, t1, 4
  bltu t1, t2, clearHeap
#init done
)"};

const char stdFunctions[] = {
    R"(
# s0 = push value
# s2 = jump register
push:
  sw s0, 0(sp)
  addi sp, sp, 4
  jr s2

# s0 = result
# s2 = jump register
pop:
  addi sp, sp, -4
  lw s0, 0(sp)
  jr s2

# s0 = result
# s2 = jump register
peek:
  addi sp, sp, -4
  lw s0, 0(sp)
  addi sp, sp, 4
  jr s2
# s0 = result
# s1 = index back
# s2 = jump register
peekNum:
  slli s1,s1, 2
  sub sp, sp, s1
  lw s0, 0(sp)
  add sp, sp, s1
  jr s2

# a0 = result
# a1 = jump to
# a2-6 = parameters
# a7 = following line
callFunction:
  #store t0, because fp must be changed
  mv s0, t0
  mv t0, sp

  # push all temp registers
  #t0 stored earlier
  jal s2, push
  mv s0, t1
  jal s2, push
  mv s0, t2
  jal s2, push
  mv s0, t3
  jal s2, push
  mv s0, t4
  jal s2, push
  mv s0, t5
  jal s2, push
  mv s0, t6
  jal s2, push

  # push all parameters
  mv s0, a2
  jal s2, push
  mv s0, a3
  jal s2, push
  mv s0, a4
  jal s2, push
  mv s0, a5
  jal s2, push
  mv s0, a6
  jal s2, push

  # push return point
  mv s0, a7
  jal s2, push

  jr a1

#a0 = return value
#t0 = frame pointer
functionReturn:
  #a7 is return line
  lw a7, 48(t0)

  lw t1, 4(t0)
  lw t2, 8(t0)
  lw t3, 12(t0)
  lw t4, 16(t0)
  lw t5, 20(t0)
  lw t6, 24(t0)
  mv sp, t0
  lw t0, 0(t0)
  
  jr a7

malloc:
  addi s1, x0, 6
  jal s2, peekNum
  #memory index
  mv t2, gp
  mallocResetCount:
  #memory amount
  addi t3, x0, 0
  mallocLoop:
    # t4 has the byte address
    srli t4, t2, 3
    # t4 has the byte data
    lb t4, 0(t4)
    #t5 has the bit address
    andi t5, t2, 7
    addi t6, x0, 128
    srl t5, t6, t5
    #t5 has mask
    and t4, t4, t5
    
    addi t2, t2,1
    blt x0, t4, mallocResetCount
    #empty spot, increment counter
    addi t3, t3, 1
    blt t3, s0, mallocLoop
  
  mallocBlockLoop:
    addi t3, t3, -1
    addi t2, t2, -1
    srli t4, t2, 3
    # t4 has the byte data
    lb t4, 0(t4)
    #t5 has the bit address
    andi t5, t2, 7
    addi t6, x0, 128
    srl t5, t6, t5
    #t5 has mask
    srli t6, t2, 3
    or t4, t4, t5
    sb t4, 0(t6)

    blt x0, t3, mallocBlockLoop
  mv a0, t2

  j functionReturn

#a0 - pin
pinOut:
  addi t4, x0, 1
  sll t4, t4, a2

  li t5, 8
  mul t5, t5, a2
  addi t5,t5,4

  li t6, 4
  mul t6, t6, a2
  addi t6, t6, 4

  # SETUP: Tell the multiplexer to set our Pin to GPIO
  li t1, IO_BANK0_BASE
  add t1, t1, t5
  lw t2, 0(t1)        # Load the current value of the GPIO_CTRL register to t2
  andi t2, t2, ~0x1F  # Clear the lower 5 bits (function select)
  ori t2, t2, 5       # Set the function select to 5 (GPIO mode)
  sw t2, 0(t1)        # Store the updated value back to the GPIO_CTRL reg.

  ### SETUP: Set GPIO15 as an output
  li t1, GPIO_DIR     # Load the address of the GPIO direction register
  lw t2, 0(t1)        # Load the current value of the direction reg. to t2
  mv t3, t4    # Load the bitmask for the used GPIO
  or t2, t2, t3       # Set the bit for used GPIO (output mode)
  sw t2, 0(t1)        # Store the updated value back to the GPIO dir. reg.

  ### Setup: Clear Pad Isolation for GPIO
  li t1, PADS_BANK0_BASE      # Load the address of the pad register
  add t1, t1, t6
  lw t2, 0(t1)        # Load the current value of the pad register
  andi t2, t2, ~0x100 # Clear isolation bit (bit nr 8)
  ori t2, t2, 0x30    # Drive strength 12mA
  sw t2, 0(t1)        # Store the updated value back to the pad register

  j functionReturn

#a0 - pin
pinToggle:
  addi t4, x0, 1
  sll t4, t4, a2


 li t1, GPIO_OUT_XOR # Load the address of the GPIO_OUT_XOR
  mv t2, t4    # Load the bit for the output GPIO to be toggled

  sw t2, 0(t1)        # Set the bit (atomic bit setting-writing)
  
  j functionReturn

EXIT:
  wfi
)"};
