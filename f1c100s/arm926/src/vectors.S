#include "linkage.h"

.section ".vectors", "ax"

.weak _image_start
_image_start:
    
.globl _start
_start:

_vectors:
    b reset
    ldr pc, _undefined_instruction
    ldr pc, _software_interrupt
    ldr pc, _prefetch_abort
    ldr pc, _data_abort
    ldr pc, _not_used
    ldr pc, _irq
    ldr pc, _fiq

_undefined_instruction:
    .word undefined_instruction
_software_interrupt:
    .word software_interrupt
_prefetch_abort:
    .word prefetch_abort
_data_abort:
    .word data_abort
_not_used:
    .word not_used
_irq:
    .word irq
_fiq:
    .word fiq

reset:
    /* Enter svc mode and mask interrupts */
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r0, r0, #0xd3
    msr cpsr, r0

    /* Set vector to the low address */
    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1<<13)
    mcr p15, 0, r0, c1, c0, 0

    /* Copy vectors to the correct address */
    adr r0, _vectors
    mrc p15, 0, r2, c1, c0, 0
    ands r2, r2, #(1 << 13)
    ldreq r1, =0x00000000
    ldrne r1, =0xffff0000
    ldmia r0!, {r2-r8, r10}
    stmia r1!, {r2-r8, r10}
    ldmia r0!, {r2-r8, r10}
    stmia r1!, {r2-r8, r10}

    /* Initialize stacks */
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r1, r0, #0x1b
    msr cpsr_cxsf, r1
    ldr sp, _stack_und_end

    bic r0, r0, #0x1f
    orr r1, r0, #0x17
    msr cpsr_cxsf, r1
    ldr sp, _stack_abt_end

    bic r0, r0, #0x1f
    orr r1, r0, #0x12
    msr cpsr_cxsf, r1
    ldr sp, _stack_irq_end

    bic r0, r0, #0x1f
    orr r1, r0, #0x11
    msr cpsr_cxsf, r1
    ldr sp, _stack_fiq_end

    bic r0, r0, #0x1f
    orr r1, r0, #0x13
    msr cpsr_cxsf, r1
    ldr sp, _stack_svc_end

    /* Clear bss section */
    ldr r0, _bss_start
    ldr r2, _bss_end
    sub r2, r2, r0
    mov r1, #0
    bl memset

    /* Call _main */
    ldr r1, =_main
    mov pc, r1
_main:
    mov r0, #0;
    mov r1, #0;
    bl main
    b _main

/* Exception handlers */
    .align 5
undefined_instruction:
    ldr sp, _stack_und_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl undefined_instruction_handler

    .align 5
software_interrupt:
    ldr sp, _stack_svc_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl software_interrupt_handler
    ldmia sp, {r0 - lr}^
    mov r0, r0
    ldr lr, [sp, #60]
    add sp, sp, #72
    movs pc, lr

    .align 5
prefetch_abort:
    ldr sp, _stack_abt_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl prefetch_abort_handler

    .align 5
data_abort:
    ldr sp, _stack_abt_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl data_abort_handler

    .align 5
not_used:
    b .

    .align 5
irq:
    ldr sp, _stack_irq_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl irq_handler
    ldmia sp, {r0 - lr}^
    mov r0, r0
    ldr lr, [sp, #60]
    add sp, sp, #72
    subs pc, lr, #4

    .align 5
fiq:
    ldr sp, _stack_fiq_end
    sub sp, sp, #72
    stmia sp, {r0 - r12}
    add r8, sp, #60
    stmdb r8, {sp, lr}^
    str lr, [r8, #0]
    mrs r6, spsr
    str r6, [r8, #4]
    str r0, [r8, #8]
    mov r0, sp
    bl irq_handler
    ldmia sp, {r0 - lr}^
    mov r0, r0
    ldr lr, [sp, #60]
    add sp, sp, #72
    subs pc, lr, #4

/* The location of sections */
     .align 4
_data_start:
    .long __data_start
_data_end:
    .long __data_end
_bss_start:
    .long __bss_start
_bss_end:
    .long __bss_end
_stack_und_end:
    .long __stack_und_end
_stack_abt_end:
    .long __stack_abt_end
_stack_irq_end:
    .long __stack_irq_end
_stack_fiq_end:
    .long __stack_fiq_end
_stack_svc_end:
    .long __stack_svc_end

/* Default exception handlers */
ENTRY(undefined_instruction_handler)
    b .
ENDPROC(undefined_instruction_handler)

ENTRY(software_interrupt_handler)
    b .
ENDPROC(software_interrupt_handler)

ENTRY(prefetch_abort_handler)
    b .
ENDPROC(prefetch_abort_handler)

ENTRY(data_abort_handler)
    b .
ENDPROC(data_abort_handler)

ENTRY(irq_handler)
    b .
ENDPROC(irq_handler)

    .weak   undefined_instruction_handler
    .weak   software_interrupt_handler
    .weak   prefetch_abort_handler
    .weak   data_abort_handler
    .weak   irq_handler
