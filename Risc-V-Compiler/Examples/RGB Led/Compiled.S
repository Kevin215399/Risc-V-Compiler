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
.equ PADS_BANK0_BASE, 0x40038000    # User Bank Pad Control register base
.equ SIO_BASE,        0xD0000000    # SIO (Single-Cycle I/O) Base Address

.equ GPIO_DIR,        SIO_BASE + 0x30   # GPIO direction register

.equ GPIO_OUT,        SIO_BASE + 0x10
.equ GPIO_OUT_XOR,    SIO_BASE + 0x28   # GPIO output XOR register (atomic)
.equ GPIO_SET,        SIO_BASE + 0x14   # GPIO direction register
.equ GPIO_CLEAR,        SIO_BASE + 0x18   # GPIO direction register

.equ GPIO_IN,        SIO_BASE + 0x4   # GPIO input

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

li s0,4
mv a2,s0
jal s2,push
la a1,L0
jal a7,callFunction
j EXIT

L0:
li t1,16
sw t1,0(sp)
addi sp,sp,4
lw t1,36(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,17
sw t1,0(sp)
addi sp,sp,4
lw t1,44(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,18
sw t1,0(sp)
addi sp,sp,4
lw t1,52(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,16
sw t1,0(sp)
addi sp,sp,4
li t1,3
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,60(t0)
jal s2,push
lw s0,64(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,17
sw t1,0(sp)
addi sp,sp,4
li t1,3
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,68(t0)
jal s2,push
lw s0,72(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,18
sw t1,0(sp)
addi sp,sp,4
li t1,3
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,76(t0)
jal s2,push
lw s0,80(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,2
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,84(t0)
jal s2,push
lw s0,88(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,3
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,92(t0)
jal s2,push
lw s0,96(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,4
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,100(t0)
jal s2,push
lw s0,104(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,5
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,108(t0)
jal s2,push
lw s0,112(t0)
jal s2,push
la a1,pinMode
jal a7,callFunction
li t1,0
sw t1,0(sp)
addi sp,sp,4
lw t1,116(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,0
sw t1,0(sp)
addi sp,sp,4
lw t1,124(t0)
sw t1,0(sp)
addi sp,sp,4
L1:
addi t1,x0,1
sw t1,0(sp)
addi sp,sp,4
lw t1,132(t0)
li t6,4
sub sp,sp,t6
beq t1,x0,L2
li s0,8
mv a2,s0
jal s2,push
li s0,2
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,132(t0)
beq t1,x0,L3
addi t1,x0,1
L3:
xori t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,136(t0)
beq t1,x0,L4
lw t1,128(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,140(t0)
beq t1,x0,L5
lw t1,40(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,144(t0)
jal s2,push
lw s0,148(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L5:
lw t1,128(t0)
li t2,1
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,120(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,144(t0)
lw t2,148(t0)
sltu t1,x0,t1
sltu t2,x0,t2
and t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,152(t0)
beq t1,x0,L6
lw t1,40(t0)
sw t1,0(sp)
addi sp,sp,4
li s0,8
mv a2,s0
jal s2,push
lw s0,156(t0)
jal s2,push
la a1,pinToggle
jal a7,callFunction
li t6,4
sub sp,sp,t6
L6:
li t6,16
sub sp,sp,t6
L4:
li s0,8
mv a2,s0
jal s2,push
li s0,2
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,140(t0)
beq t1,x0,L7
lw t1,40(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,0
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,144(t0)
jal s2,push
lw s0,148(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L7:
li s0,8
mv a2,s0
jal s2,push
li s0,3
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,144(t0)
beq t1,x0,L8
addi t1,x0,1
L8:
xori t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,148(t0)
beq t1,x0,L9
lw t1,128(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,152(t0)
beq t1,x0,L10
lw t1,48(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,156(t0)
jal s2,push
lw s0,160(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L10:
lw t1,128(t0)
li t2,1
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,120(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,156(t0)
lw t2,160(t0)
sltu t1,x0,t1
sltu t2,x0,t2
and t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,164(t0)
beq t1,x0,L11
lw t1,48(t0)
sw t1,0(sp)
addi sp,sp,4
li s0,8
mv a2,s0
jal s2,push
lw s0,168(t0)
jal s2,push
la a1,pinToggle
jal a7,callFunction
li t6,4
sub sp,sp,t6
L11:
li t6,16
sub sp,sp,t6
L9:
li s0,8
mv a2,s0
jal s2,push
li s0,3
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,152(t0)
beq t1,x0,L12
lw t1,48(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,0
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,156(t0)
jal s2,push
lw s0,160(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L12:
li s0,8
mv a2,s0
jal s2,push
li s0,4
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,156(t0)
beq t1,x0,L13
addi t1,x0,1
L13:
xori t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,160(t0)
beq t1,x0,L14
lw t1,128(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,164(t0)
beq t1,x0,L15
lw t1,56(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,1
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,168(t0)
jal s2,push
lw s0,172(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L15:
lw t1,128(t0)
li t2,1
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,120(t0)
li t2,0
xor t1,t1,t2
sltiu t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,168(t0)
lw t2,172(t0)
sltu t1,x0,t1
sltu t2,x0,t2
and t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,176(t0)
beq t1,x0,L16
lw t1,56(t0)
sw t1,0(sp)
addi sp,sp,4
li s0,8
mv a2,s0
jal s2,push
lw s0,180(t0)
jal s2,push
la a1,pinToggle
jal a7,callFunction
li t6,4
sub sp,sp,t6
L16:
li t6,16
sub sp,sp,t6
L14:
li s0,8
mv a2,s0
jal s2,push
li s0,4
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,164(t0)
beq t1,x0,L17
lw t1,56(t0)
sw t1,0(sp)
addi sp,sp,4
li t1,0
sw t1,0(sp)
addi sp,sp,4
li s0,12
mv a2,s0
jal s2,push
lw s0,168(t0)
jal s2,push
lw s0,172(t0)
jal s2,push
la a1,pinWrite
jal a7,callFunction
li t6,8
sub sp,sp,t6
L17:
li s0,8
mv a2,s0
jal s2,push
li s0,5
jal s2,push
la a1,pinRead
jal a7,callFunction
sw a0,0(sp)
addi sp,sp,4
lw t1,168(t0)
beq t1,x0,L18
addi t1,x0,1
L18:
xori t1,t1,1
sw t1,0(sp)
addi sp,sp,4
lw t1,172(t0)
beq t1,x0,L19
li t1,1
lw t2,128(t0)
xor t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,176(t0)
#StackReplace
sw t1,128(t0)
li t1,0
sw t1,0(sp)
addi sp,sp,4
lw t1,180(t0)
sw t1,0(sp)
addi sp,sp,4
L20:
lw t1,184(t0)
li t2,1000000
slt t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,188(t0)
li t6,4
sub sp,sp,t6
beq t1,x0,L21
li t1,1
lw t2,184(t0)
add t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,188(t0)
#StackReplace
sw t1,184(t0)
li t6,4
sub sp,sp,t6
j L20
L21:
li t6,12
sub sp,sp,t6
L19:
li t1,1
lw t2,120(t0)
add t1,t1,t2
sw t1,0(sp)
addi sp,sp,4
lw t1,176(t0)
#StackReplace
sw t1,120(t0)
lw t1,120(t0)
li t2,10000
slt t3,t1,t2
xor t1,t1,t2
sltu t1,x0,t1
xor t1,t1,t3
sw t1,0(sp)
addi sp,sp,4
lw t1,180(t0)
beq t1,x0,L22
li t1,0
sw t1,0(sp)
addi sp,sp,4
lw t1,184(t0)
#StackReplace
sw t1,120(t0)
li t6,4
sub sp,sp,t6
L22:
li t6,52
sub sp,sp,t6
j L1
L2:
j functionReturn

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
# a2 = parameter offset
# a7 = following line
callFunction:
  
  #store t0, because fp must be changed
  mv s0, t0
  mv t0, sp

  sub t0, t0, a2

  # push all temp registers

  #t0 stored to s0 earlier
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

  # push return point
  mv s0, a7
  jal s2, push

  jr a1

#a0 = return value
#t0 = frame pointer
functionReturn:
  
  lw s0, 0(t0)
  add t0, t0, s0


  #a7 is return line
  lw a7, 28(t0)

  lw t1, 4(t0)
  lw t2, 8(t0)
  lw t3, 12(t0)
  lw t4, 16(t0)
  lw t5, 20(t0)
  lw t6, 24(t0)
  mv sp, t0
  sub sp, sp, s0
  lw t0, 0(t0)
  
  jr a7

malloc:
  lw s0, 4(t0)
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

#p1 - pin
#p2 - mode (3=output, 2=input pd, 1= input pu, 0= input)
pinMode:
  lw a2, 4(t0)
  lw a3, 8(t0)
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

  li t5, 3
  blt a3, t5, pinIn

  or t2, t2, t3       # Set the bit for used GPIO (output mode)
  j notInput

  pinIn:
  not t3, t3
  and t2, t2, t3       # Clear the bit for used GPIO (input mode)

  notInput:

  sw t2, 0(t1)        # Store the updated value back to the GPIO dir. reg.

  ### Setup: Clear Pad Isolation for GPIO
  li t1, PADS_BANK0_BASE      # Load the address of the pad register
  add t1, t1, t6
  lw t2, 0(t1)        # Load the current value of the pad register
  andi t2, t2, ~0x100 # Clear isolation bit (bit nr 8)
  ori t2, t2, 0x30    # Drive strength 12mA

  li t5, 3
  beq a3, t5, notInputEnable
  ori t2, t2, 1<<6    # Input enabled
  ori t2, t2, 1<<7    # output disabled
  j checkPulls
  notInputEnable:
  andi t2, t2, ~(1<<6)    # Input disabled
  andi t2, t2, ~(1<<7)    # output enabled
  checkPulls:

  li t5, 2
  bne a3, t5, notPullDown
  ori t2, t2, 4    # Pull Down
  andi t2, t2, ~8    # Pull up off
  notPullDown:

  li t5, 1
  bne a3, t5, notPullUp
  ori t2, t2, 8    # Pull up
  andi t2, t2, ~4    # Pull Down off
  notPullUp:

  sw t2, 0(t1)        # Store the updated value back to the pad register

  j functionReturn

#a0 - pin
pinToggle:
  lw a2, 4(t0)
  addi t4, x0, 1
  sll t4, t4, a2


 li t1, GPIO_OUT_XOR # Load the address of the GPIO_OUT_XOR
  mv t2, t4    # Load the bit for the output GPIO to be toggled

  sw t2, 0(t1)        # Set the bit (atomic bit setting-writing)
  
  j functionReturn

# p1 - pin
# p2 - value
pinWrite:
  lw a2, 4(t0)
  lw a3, 8(t0)
  
  addi t4, x0, 1
  sll t4, t4, a2

  beqz a3, setLow

  #set the pin HIGH
  li t1, GPIO_OUT
  lw t2, 0(t1)
  or t2, t2, t4
  sw t2, 0(t1)

  j functionReturn
  setLow:
  
  #set the pin LOW
  li t1, GPIO_OUT
  lw t2, 0(t1)
  not t4,t4
  and t2, t2, t4
  sw t2, 0(t1)

  j functionReturn

# p1- pin
pinRead:
  lw a2, 4(t0)

  addi t4, x0, 1
  sll t4, t4, a2
  
  li t1, GPIO_IN
  lw t2, 0(t1)

  and t2, t2, t4
  sltu a0, x0, t2
  j functionReturn

EXIT:
  wfi