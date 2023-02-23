#include <stdio.h>
#include "arm32.h"

struct arm_regs_t {
    uint32_t r[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t cpsr;
};

static void show_regs(struct arm_regs_t* regs) {
    int i;

    printf("pc : [<%08lx>] lr : [<%08lx>] cpsr: %08lx\r\n", regs->pc, regs->lr, regs->cpsr);
    printf("sp : %08lx\r\n", regs->sp);
    for(i = 12; i >= 0; i--) {
        printf("r%-2d: %08lx ", i, regs->r[i]);
        if(i % 2 == 0) printf("\r\n");
    }
    printf("\r\n");
}

void _undefined_instruction_(struct arm_regs_t* regs) {
    printf("\r\n!! PREFETCH_ABORT !!\r\n");
    show_regs(regs);
    while(1)
        ;
}

void _software_interrupt_(struct arm_regs_t* regs) {
    printf("\r\n!! SOFT_INTERRUPT !!\r\n");
    //show_regs(regs);
    //while(1);
}

void _prefetch_abort_(struct arm_regs_t* regs) {
    printf("\r\n!! PREFETCH_ABORT !!\r\n");
    show_regs(regs);
    while(1)
        ;
}

void _data_abort_(struct arm_regs_t* regs) {
    printf("\r\n!! DATA_ABORT !!\r\n");
    show_regs(regs);
    while(1)
        ;
}
