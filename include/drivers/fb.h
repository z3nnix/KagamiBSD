#ifndef DRIVERS_FB_H
#define DRIVERS_FB_H

#include <stdbool.h>

bool fb_init(void);
void fb_putc(char c);
void fb_clear(void);

#endif
