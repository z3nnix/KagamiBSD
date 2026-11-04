.PHONY: all kernel iso clean

ISO_DIR := build/iso

all: kernel

kernel:
	$(MAKE) -C kernel

iso: kernel
	rm -rf $(ISO_DIR)
	mkdir -p dist
	mkdir -p $(ISO_DIR)/limine
	cp build/kernel.elf $(ISO_DIR)/kernel.bin
	cp limine/limine-bios-cd.bin limine/limine-bios.sys $(ISO_DIR)/limine/
	cp limine/limine.conf $(ISO_DIR)/limine/limine.conf
	cp limine/limine/wallpaper.jpg $(ISO_DIR)/limine/wallpaper.jpg
	xorriso -as mkisofs -b limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table $(ISO_DIR) -o dist/kagamibsd.iso

clean:
	$(MAKE) -C kernel clean
	rm -rf build dist

run:
	qemu-system-x86_64 -m 2G --cdrom dist/kagamibsd.iso