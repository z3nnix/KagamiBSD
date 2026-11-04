#ifndef ARCH_IDT_H
#define ARCH_IDT_H

#include <stdint.h>

#define IDT_SIZE 256
#define KERNEL_CS 0x08
#define INTERRUPT_GATE 0x8E

typedef struct __attribute__((packed)) {
	uint16_t offset_0_15;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t offset_16_31;
	uint32_t offset_32_63;
	uint32_t zero;
} idt_entry_t;

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} idtr_t;

typedef struct {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} interrupt_frame_t;

void idt_init(void);

#endif
