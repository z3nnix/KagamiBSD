.DEFAULT_GOAL := all
SHELL := /bin/sh

ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

KERNEL_IMAGE ?= $(ROOT)/build/kernel/netbsd
DEFAULT_KERNEL := $(ROOT)/build/kernel/netbsd
BRANDING_DIR ?= $(ROOT)/kagami/branding
ISO_OVERLAY ?= $(ROOT)/kagami/iso-overlay
ISO ?= $(ROOT)/dist/kagamibsd.iso
ISO_ROOT := $(ROOT)/build/iso
ISO_TMP := $(ROOT)/build/kagamibsd.iso

QEMU ?= qemu-system-x86_64
QEMU_OPTS ?= -m 2G -smp 2 -cdrom $(ISO) -boot d
KVM := $(shell test -w /dev/kvm && echo -enable-kvm)

GRUB_MKRESCUE_OPTS ?=

.PHONY: all kernel kernel-fast tools bootstrap iso run run-only doctor fetch-source pin-source help clean kernel-clean tools-clean distclean check-kernel check-branding

all:
	+$(MAKE) kernel
	+$(MAKE) iso

kernel:
	+$(MAKE) -C $(ROOT)/kernel

kernel-fast:
	+$(MAKE) -C $(ROOT)/kernel fast

tools bootstrap:
	+$(MAKE) -C $(ROOT)/kernel tools

iso: check-kernel check-branding
	rm -rf $(ISO_ROOT)
	mkdir -p $(ISO_ROOT)/boot/grub $(dir $(ISO)) $(dir $(ISO_TMP))
	cp $(KERNEL_IMAGE) $(ISO_ROOT)/boot/netbsd
	cp -a $(BRANDING_DIR)/. $(ISO_ROOT)/boot/grub/
	@if [ -d "$(ISO_OVERLAY)" ]; then cp -a $(ISO_OVERLAY)/. $(ISO_ROOT)/; fi
	grub-mkrescue $(GRUB_MKRESCUE_OPTS) -o $(ISO_TMP) $(ISO_ROOT)
	mv $(ISO_TMP) $(ISO)

check-kernel:
	@if [ ! -f "$(KERNEL_IMAGE)" ]; then \
		if [ "$(KERNEL_IMAGE)" = "$(DEFAULT_KERNEL)" ]; then \
			$(MAKE) -C $(ROOT)/kernel; \
		else \
			echo "kernel not found: $(KERNEL_IMAGE)"; \
			exit 1; \
		fi; \
	fi

check-branding:
	@test -f "$(BRANDING_DIR)/grub.cfg" || { \
		echo "branding config not found: $(BRANDING_DIR)/grub.cfg"; \
		exit 1; \
	}

run: iso
	$(QEMU) $(KVM) $(QEMU_OPTS)

run-only:
	@test -f "$(ISO)" || { echo "iso not found: $(ISO)"; exit 1; }
	$(QEMU) $(KVM) $(QEMU_OPTS)

doctor:
	@$(ROOT)/scripts/doctor.sh

fetch-source:
	@$(ROOT)/scripts/fetch-netbsd.sh

pin-source:
	@git -C $(ROOT)/kernel/src rev-parse HEAD > $(ROOT)/kagami/netbsd.rev
	@printf 'pinned: %s\n' "$$(cat $(ROOT)/kagami/netbsd.rev)"

help:
	@printf '%s\n' \
		'make              build kernel and ISO' \
		'make kernel       build or update the NetBSD kernel' \
		'make kernel-fast  incremental kernel rebuild via nbmake' \
		'make bootstrap    build the NetBSD gcc cross toolchain' \
		'make iso          build ISO from the current kernel artifact' \
		'make run          build ISO if needed and run QEMU' \
		'make run-only     run the existing ISO without rebuilding' \
		'make doctor       check host tools, source, branding and GRUB' \
		'make fetch-source checkout the pinned NetBSD source' \
		'make pin-source   pin the current NetBSD source commit' \
		'make clean        remove build/ and dist/' \
		'make kernel-clean remove kernel objects and artifact' \
		'make tools-clean  remove the cross toolchain' \
		'make distclean    remove all generated files except kernel/src'

clean:
	rm -rf $(ROOT)/build $(ROOT)/dist

kernel-clean:
	+$(MAKE) -C $(ROOT)/kernel clean
	rm -rf $(ROOT)/build/kernel

tools-clean:
	rm -rf $(ROOT)/tools $(ROOT)/obj/tools

distclean: clean kernel-clean tools-clean
	rm -rf $(ROOT)/obj
