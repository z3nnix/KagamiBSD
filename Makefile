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

TOOLS ?= $(ROOT)/tools
TOOLCHAIN_ARCHIVE ?= $(ROOT)/toolchain/netbsd-amd64-tools-14.4.0-36eddeb.tar.zst
TOOLCHAIN_SUMS ?= $(ROOT)/toolchain/SHA256SUMS

QEMU ?= qemu-system-x86_64
QEMU_OPTS ?= -m 2G -smp 2 -cdrom $(ISO) -boot d
KVM := $(shell test -w /dev/kvm && echo -enable-kvm)

GRUB_MKRESCUE_OPTS ?=

.PHONY: all kernel kernel-fast bootstrap tools iso run run-only doctor help clean kernel-clean tools-clean distclean check-kernel check-branding

all:
	+$(MAKE) kernel
	+$(MAKE) iso

kernel: bootstrap
	+$(MAKE) -C $(ROOT)/kernel

kernel-fast: bootstrap
	+$(MAKE) -C $(ROOT)/kernel fast

bootstrap tools:
	@if [ ! -x "$(TOOLS)/bin/x86_64--netbsd-gcc" ]; then \
		test -f "$(TOOLCHAIN_ARCHIVE)" || { \
			echo "toolchain archive not found: $(TOOLCHAIN_ARCHIVE)"; \
			exit 1; \
		}; \
		test -f "$(TOOLCHAIN_SUMS)" || { \
			echo "toolchain checksums not found: $(TOOLCHAIN_SUMS)"; \
			exit 1; \
		}; \
		cd "$(ROOT)/toolchain" && sha256sum -c "$(TOOLCHAIN_SUMS)"; \
		mkdir -p "$(TOOLS)"; \
		tar --zstd -xf "$(TOOLCHAIN_ARCHIVE)" -C "$(TOOLS)"; \
	fi
	@$(ROOT)/scripts/relocate-toolchain.sh
	@test -x "$(TOOLS)/bin/x86_64--netbsd-gcc" || { \
		echo "toolchain bootstrap failed"; \
		exit 1; \
	}

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
			$(MAKE) kernel; \
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

help:
	@printf '%s\n' \
		'make              build kernel and ISO' \
		'make kernel       build or update the NetBSD kernel' \
		'make kernel-fast  incremental kernel rebuild via nbmake' \
		'make bootstrap    extract and relocate the prebuilt toolchain' \
		'make iso          build ISO from the current kernel artifact' \
		'make run          build ISO if needed and run QEMU' \
		'make run-only     run the existing ISO without rebuilding' \
		'make doctor       check host tools, source, toolchain and GRUB' \
		'make clean        remove build/ and dist/' \
		'make kernel-clean remove kernel objects and artifact' \
		'make tools-clean  remove the extracted toolchain' \
		'make distclean    remove all generated files except vendored source'

clean:
	rm -rf $(ROOT)/build $(ROOT)/dist

kernel-clean:
	+$(MAKE) -C $(ROOT)/kernel clean
	rm -rf $(ROOT)/build/kernel

tools-clean:
	rm -rf $(TOOLS)

distclean: clean kernel-clean tools-clean
	rm -rf $(ROOT)/obj
