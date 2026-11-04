#include <stdint.h>
#include <arch/cpu.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/panic.h>
#include <drivers/fb.h>
#include <lib/kstd.h>
#include <limine.h>
#include <tty.h>

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start[4] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[3] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end[2] = LIMINE_REQUESTS_END_MARKER;

void kmain(void) {
	if (!fb_init()) {
		panic("framebuffer unavailable");
	}

	tty_init(&fb_putc, &fb_clear);

	gdt_init();
	idt_init();

	kprint("KagamiBSD 0.1-INDEV\n");
	kprint("\tCopyright (c) 2026 Zennix\n");

	cpu_interrupts_enable();

	for (;;) {
		cpu_halt();
	}
}
