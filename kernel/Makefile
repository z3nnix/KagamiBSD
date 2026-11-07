ROOT := $(abspath $(CURDIR)/..)
NETBSD_SRC ?= $(CURDIR)/src
OBJ ?= $(ROOT)/obj
TOOLS ?= $(ROOT)/tools
KCONF ?= $(ROOT)/kagami/conf/KAGAMI
KERNOBJ := $(OBJ)/sys/arch/amd64/compile/$(notdir $(KCONF))
KERNEL_OUT ?= $(ROOT)/build/kernel/netbsd
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

BUILD_SH := $(NETBSD_SRC)/build.sh
NBMK := $(TOOLS)/bin/nbmake-amd64
BUILD_FLAGS := -U -u -c gcc -O $(OBJ) -T $(TOOLS) -m amd64 -j$(JOBS) -V MKCTF=no -V MKDTRACE=no -V MKDEBUGKERNEL=no -V MKDEBUG=no

.PHONY: all kernel fast tools check-src clean

all: kernel

kernel: check-src tools
	$(BUILD_SH) $(BUILD_FLAGS) kernel=$(KCONF)
	@mkdir -p $(dir $(KERNEL_OUT))
	@cmp -s $(KERNOBJ)/netbsd $(KERNEL_OUT) || cp $(KERNOBJ)/netbsd $(KERNEL_OUT)

fast: check-src tools
	@test -f $(KERNOBJ)/Makefile || { echo "run 'make kernel' first"; exit 1; }
	$(NBMK) -j$(JOBS) -C $(KERNOBJ) all
	@mkdir -p $(dir $(KERNEL_OUT))
	@cmp -s $(KERNOBJ)/netbsd $(KERNEL_OUT) || cp $(KERNOBJ)/netbsd $(KERNEL_OUT)

tools:
	@test -x $(TOOLS)/bin/x86_64--netbsd-gcc || \
		$(BUILD_SH) -U -u -c gcc -O $(OBJ) -T $(TOOLS) -m amd64 -j$(JOBS) tools

check-src:
	@test -d $(NETBSD_SRC)/sys || { \
		echo "NetBSD source not found: $(NETBSD_SRC)"; \
		echo "run 'make fetch-source'"; \
		exit 1; \
	}
	@want=$$(cat $(ROOT)/kagami/netbsd.rev 2>/dev/null); \
	got=$$(git -C $(NETBSD_SRC) rev-parse HEAD 2>/dev/null); \
	if [ -n "$$want" ] && [ -n "$$got" ] && [ "$$want" != "$$got" ]; then \
		echo "warning: NetBSD source is $$got, pinned is $$want"; \
	fi

clean:
	rm -rf $(KERNOBJ) $(ROOT)/build/kernel
