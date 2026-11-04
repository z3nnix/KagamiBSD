CC := gcc
LD := ld

CFLAGS := -std=gnu11 -Wall -Wextra -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -mno-red-zone -mcmodel=kernel -mno-sse -mno-sse2 -mno-mmx -mno-80387 -mgeneral-regs-only -I../include
LDFLAGS := -T linker.ld -nostdlib -static -z max-page-size=0x1000

OBJ_DIR := ../build/obj
TARGET := ../build/kernel.elf

SOURCES := $(patsubst ./%,%,$(shell find . -name '*.c'))
OBJECTS := $(addprefix $(OBJ_DIR)/,$(SOURCES:.c=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) linker.ld
	@mkdir -p $(dir $@)
	@printf 'LD %s\n' $(abspath $@)
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf 'CC %s\n' $(abspath $<)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
