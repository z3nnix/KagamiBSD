ROOT := $(abspath $(CURDIR)/..)
NETBSD_SRC ?= $(CURDIR)/netbsd
OBJ ?= $(ROOT)/obj
TOOLS ?= $(ROOT)/tools
KCONF ?= KAGAMI
KERNOBJ := $(OBJ)/sys/arch/amd64/compile/$(notdir $(KCONF))
KERNEL_OUT ?= $(ROOT)/build/kernel/netbsd
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

BUILD_SH := $(NETBSD_SRC)/build.sh
NBMK := $(TOOLS)/bin/nbmake-amd64
BUILD_FLAGS := -U -u -c gcc -O $(OBJ) -T $(TOOLS) -m amd64 -j$(JOBS) -V MKCTF=no -V MKDTRACE=no -V MKDEBUGKERNEL=no -V MKDEBUG=no

.PHONY: all kernel fast check-tools check-vendor clean

all: kernel

kernel: check-vendor check-tools
	$(BUILD_SH) $(BUILD_FLAGS) kernel=$(KCONF)
	@mkdir -p $(dir $(KERNEL_OUT))
	@cmp -s $(KERNOBJ)/netbsd $(KERNEL_OUT) || cp $(KERNOBJ)/netbsd $(KERNEL_OUT)

fast: check-vendor check-tools
	@test -f $(KERNOBJ)/Makefile || { echo "run 'make kernel' first"; exit 1; }
	$(NBMK) -j$(JOBS) -C $(KERNOBJ) all
	@mkdir -p $(dir $(KERNEL_OUT))
	@cmp -s $(KERNOBJ)/netbsd $(KERNEL_OUT) || cp $(KERNOBJ)/netbsd $(KERNEL_OUT)

check-tools:
	@test -x $(TOOLS)/bin/x86_64--netbsd-gcc || { \
		echo "toolchain not found: $(TOOLS)"; \
		echo "run 'make bootstrap'"; \
		exit 1; \
	}

check-vendor:
	@test -x $(BUILD_SH) && test -d $(NETBSD_SRC)/sys && \
		test -d $(NETBSD_SRC)/common && test -d $(NETBSD_SRC)/share/mk || { \
		echo "incomplete vendored source: $(NETBSD_SRC)"; \
		echo "expected sys/, common/, share/mk/ and build.sh"; \
		exit 1; \
	}

clean:
	rm -rf $(KERNOBJ) $(ROOT)/build/kernel
