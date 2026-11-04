#include <tty.h>

static tty_putc_fn_t tty_putc_fn;
static tty_clear_fn_t tty_clear_fn;

void tty_init(tty_putc_fn_t putc, tty_clear_fn_t clear) {
	tty_putc_fn = putc;
	tty_clear_fn = clear;

	if (tty_clear_fn) {
		tty_clear_fn();
	}
}

void tty_putc(char c) {
	if (!tty_putc_fn) {
		return;
	}

	tty_putc_fn(c);
}

void tty_puts(const char* str) {
	if (!str) {
		return;
	}

	for (int i = 0; str[i] != '\0'; i++) {
		tty_putc(str[i]);
	}
}

void tty_clear(void) {
	if (!tty_clear_fn) {
		return;
	}

	tty_clear_fn();
}

void tty_newline(void) {
	if (!tty_putc_fn) {
		return;
	}

	tty_putc_fn('\n');
}