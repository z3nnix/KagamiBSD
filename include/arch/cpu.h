#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <stdint.h>

void cpu_interrupts_disable(void);
void cpu_interrupts_enable(void);
void cpu_halt(void);
void cpu_halt_forever(void);
uint64_t cpu_read_cr2(void);
uint64_t cpu_read_cr3(void);

#endif
