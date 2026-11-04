#ifndef ARCH_PANIC_H
#define ARCH_PANIC_H

#include <stdbool.h>
#include <stdint.h>
#include <arch/idt.h>

void panic(const char* msg);
void panic_exception(const char* name, interrupt_frame_t* frame, uint64_t error_code, bool has_error);

#endif
