#include <stdint.h>
#include <arch/gdt.h>

typedef struct __attribute__((packed)) {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} gdt_pointer_t;

static gdt_entry_t gdt[3];
static gdt_pointer_t gdt_pointer;

static void gdt_set_entry(uint64_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
	gdt[index].base_low = (uint16_t)(base & 0xFFFF);
	gdt[index].base_middle = (uint8_t)((base >> 16) & 0xFF);
	gdt[index].base_high = (uint8_t)((base >> 24) & 0xFF);
	gdt[index].limit_low = (uint16_t)(limit & 0xFFFF);
	gdt[index].granularity = (uint8_t)((limit >> 16) & 0x0F);
	gdt[index].granularity |= granularity & 0xF0;
	gdt[index].access = access;
}

void gdt_init(void) {
	gdt_set_entry(0, 0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xAF);
	gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);

	gdt_pointer.limit = (uint16_t)(sizeof(gdt) - 1);
	gdt_pointer.base = (uint64_t)&gdt;

	__asm__ volatile(
		"lgdt %0\n\t"
		"pushq $0x08\n\t"
		"leaq 1f(%%rip), %%rax\n\t"
		"pushq %%rax\n\t"
		"lretq\n"
		"1:\n\t"
		"movw $0x10, %%ax\n\t"
		"movw %%ax, %%ds\n\t"
		"movw %%ax, %%es\n\t"
		"movw %%ax, %%fs\n\t"
		"movw %%ax, %%gs\n\t"
		"movw %%ax, %%ss\n\t"
		:
		: "m"(gdt_pointer)
		: "rax", "memory"
	);
}
