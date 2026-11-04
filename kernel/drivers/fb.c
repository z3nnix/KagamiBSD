#define FONT8x16_IMPLEMENTATION
#include <font.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <drivers/fb.h>
#include <limine.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0,
	.response = NULL,
};

static struct {
	uint32_t* address;
	uint64_t width;
	uint64_t height;
	uint64_t pitch_pixels;
	uint32_t foreground;
	uint32_t background;
	uint64_t cursor_x;
	uint64_t cursor_y;
	bool ready;
} fb_state;

static void fb_put_pixel(uint64_t x, uint64_t y, uint32_t color) {
	if (!fb_state.ready) {
		return;
	}

	if (x >= fb_state.width || y >= fb_state.height) {
		return;
	}

	fb_state.address[y * fb_state.pitch_pixels + x] = color;
}

static void fb_draw_char(char c, uint64_t x, uint64_t y) {
	unsigned char ch = (unsigned char)c;

	if (ch >= 128) {
		ch = '?';
	}

	for (uint64_t row = 0; row < FONT_HEIGHT; row++) {
		unsigned char bits = font8x16[ch][row];

		for (uint64_t col = 0; col < FONT_WIDTH; col++) {
			uint32_t color = (bits & (0x80 >> col)) ? fb_state.foreground : fb_state.background;
			fb_put_pixel(x + col, y + row, color);
		}
	}
}

static void fb_scroll(void) {
	for (uint64_t y = FONT_HEIGHT; y < fb_state.height; y++) {
		uint32_t* source = fb_state.address + y * fb_state.pitch_pixels;
		uint32_t* destination = fb_state.address + (y - FONT_HEIGHT) * fb_state.pitch_pixels;

		for (uint64_t x = 0; x < fb_state.width; x++) {
			destination[x] = source[x];
		}
	}

	for (uint64_t y = fb_state.height - FONT_HEIGHT; y < fb_state.height; y++) {
		uint32_t* row = fb_state.address + y * fb_state.pitch_pixels;

		for (uint64_t x = 0; x < fb_state.width; x++) {
			row[x] = fb_state.background;
		}
	}
}

static void fb_newline(void) {
	fb_state.cursor_x = 0;

	if (fb_state.cursor_y + FONT_HEIGHT >= fb_state.height) {
		fb_scroll();
		fb_state.cursor_y = fb_state.height - FONT_HEIGHT;
		return;
	}

	fb_state.cursor_y += FONT_HEIGHT;
}

static void fb_backspace(void) {
	if (fb_state.cursor_x < FONT_WIDTH) {
		return;
	}

	fb_state.cursor_x -= FONT_WIDTH;
	fb_draw_char(' ', fb_state.cursor_x, fb_state.cursor_y);
}

bool fb_init(void) {
	if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count == 0) {
		return false;
	}

	struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

	if (!framebuffer || !framebuffer->address) {
		return false;
	}

	if (framebuffer->bpp != 32 || framebuffer->memory_model != LIMINE_FRAMEBUFFER_RGB) {
		return false;
	}

	if (framebuffer->width < FONT_WIDTH || framebuffer->height < FONT_HEIGHT) {
		return false;
	}

	fb_state.address = (uint32_t*)framebuffer->address;
	fb_state.width = framebuffer->width;
	fb_state.height = framebuffer->height;
	fb_state.pitch_pixels = framebuffer->pitch / sizeof(uint32_t);
	fb_state.foreground = 0x00FFFFFF;
	fb_state.background = 0x00000000;
	fb_state.cursor_x = 0;
	fb_state.cursor_y = 0;
	fb_state.ready = true;

	fb_clear();

	return true;
}

void fb_putc(char c) {
	if (!fb_state.ready) {
		return;
	}

	if (c == '\n') {
		fb_newline();
		return;
	}

	if (c == '\r') {
		fb_state.cursor_x = 0;
		return;
	}

	if (c == '\b') {
		fb_backspace();
		return;
	}

	if (c == '\t') {
		for (int i = 0; i < 4; i++) {
			fb_putc(' ');
		}

		return;
	}

	unsigned char ch = (unsigned char)c;

	if (ch < 0x20 || ch == 0x7F) {
		return;
	}

	if (fb_state.cursor_x + FONT_WIDTH > fb_state.width) {
		fb_newline();
	}

	fb_draw_char((char)ch, fb_state.cursor_x, fb_state.cursor_y);
	fb_state.cursor_x += FONT_WIDTH;
}

void fb_clear(void) {
	if (!fb_state.ready) {
		return;
	}

	for (uint64_t y = 0; y < fb_state.height; y++) {
		uint32_t* row = fb_state.address + y * fb_state.pitch_pixels;

		for (uint64_t x = 0; x < fb_state.width; x++) {
			row[x] = fb_state.background;
		}
	}

	fb_state.cursor_x = 0;
	fb_state.cursor_y = 0;
}
