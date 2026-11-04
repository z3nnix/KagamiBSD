#include <stdint.h>
#include <arch/cpu.h>
#include <arch/panic.h>
#include <tty.h>

static void panic_hex(uint64_t value) {
	char buffer[19];

	buffer[0] = '0';
	buffer[1] = 'x';

	for (int i = 15; i >= 0; i--) {
		uint8_t nibble = (uint8_t)((value >> (i * 4)) & 0xF);
		buffer[17 - i] = (char)(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
	}

	buffer[18] = '\0';
	tty_puts(buffer);
}

void panic(const char* msg) {
	cpu_interrupts_disable();

	tty_puts("\nKERNEL PANIC: ");
	tty_puts(msg ? msg : "unknown");
	tty_puts("\n");

	cpu_halt_forever();
}

void panic_exception(const char* name, interrupt_frame_t* frame, uint64_t error_code, bool has_error) {
	cpu_interrupts_disable();

	tty_puts("\nKERNEL PANIC: ");
	tty_puts(name ? name : "unknown exception");
	tty_puts("\nRIP: ");
	panic_hex(frame->rip);
	tty_puts("\nCS: ");
	panic_hex(frame->cs);
	tty_puts("\nRFLAGS: ");
	panic_hex(frame->rflags);
	tty_puts("\nRSP: ");
	panic_hex(frame->rsp);
	tty_puts("\nSS: ");
	panic_hex(frame->ss);
	tty_puts("\nCR2: ");
	panic_hex(cpu_read_cr2());
	tty_puts("\nCR3: ");
	panic_hex(cpu_read_cr3());

	if (has_error) {
		tty_puts("\nERR: ");
		panic_hex(error_code);
	}

	tty_puts("\n");

	cpu_halt_forever();
}
