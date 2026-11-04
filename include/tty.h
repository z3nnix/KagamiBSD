#ifndef TTY_H
#define TTY_H

typedef void (*tty_putc_fn_t)(char c);
typedef void (*tty_clear_fn_t)(void);

void tty_init(tty_putc_fn_t putc, tty_clear_fn_t clear);
void tty_putc(char c);
void tty_puts(const char* str);
void tty_clear(void);
void tty_newline(void); 

#endif
