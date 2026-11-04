#include <stddef.h>
#include <tty.h>

size_t strlen(const char* str) {
	if (!str) {
		return 0;
	}

	size_t length = 0;

	while (str[length] != '\0') {
		length++;
	}

	return length;
}

void kprint(const char* str) {
	int str_length = strlen(str);
	for(int i = 0; i < str_length; i++) {
		if (str[i] == '\\' && str[i + 1] == 't') {
			tty_putc(9);
			continue;
			i++;
		}

		tty_putc(str[i]);
	}
}
