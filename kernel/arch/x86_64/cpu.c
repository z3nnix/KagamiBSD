#include <arch/cpu.h>

void cpu_interrupts_disable(void) {
	__asm__ volatile("cli");
}

void cpu_interrupts_enable(void) {
	__asm__ volatile("sti");
}

void cpu_halt(void) {
	__asm__ volatile("hlt");
}

void cpu_halt_forever(void) {
	for (;;) {
		cpu_interrupts_disable();
		cpu_halt();
	}
}

uint64_t cpu_read_cr2(void) {
	uint64_t value;

	__asm__ volatile("mov %%cr2, %0" : "=r"(value));

	return value;
}

uint64_t cpu_read_cr3(void) {
	uint64_t value;

	__asm__ volatile("mov %%cr3, %0" : "=r"(value));

	return value;
}
