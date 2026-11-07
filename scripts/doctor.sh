#!/bin/sh
set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FAIL=0

check_cmd() {
	if command -v "$1" >/dev/null 2>&1; then
		printf 'ok: %s\n' "$1"
	else
		printf 'missing: %s\n' "$1"
		FAIL=1
	fi
}

warn() {
	printf 'warning: %s\n' "$1"
}

check_cmd make
check_cmd sha256sum
check_cmd tar
check_cmd zstd
check_cmd grub-mkrescue
check_cmd grub-mkimage
check_cmd xorriso
check_cmd mcopy
check_cmd mtools
check_cmd qemu-system-x86_64

if [ -x "$ROOT/kernel/netbsd/build.sh" ] && [ -d "$ROOT/kernel/netbsd/sys" ] && \
	[ -d "$ROOT/kernel/netbsd/common" ] && [ -d "$ROOT/kernel/netbsd/share/mk" ]; then
	printf 'ok: vendored NetBSD source\n'
else
	printf 'missing: vendored NetBSD source in %s\n' "$ROOT/kernel/netbsd"
	FAIL=1
fi

TOOLCHAIN_ARCHIVE="$ROOT/toolchain/netbsd-amd64-tools-14.4.0-36eddeb.tar.zst"
TOOLCHAIN_SUMS="$ROOT/toolchain/SHA256SUMS"

if [ -x "$ROOT/tools/bin/x86_64--netbsd-gcc" ]; then
	printf 'ok: extracted toolchain\n'
elif [ -f "$TOOLCHAIN_ARCHIVE" ]; then
	warn "toolchain is not extracted (run make bootstrap)"
else
	printf 'missing: toolchain archive and extracted toolchain\n'
	FAIL=1
fi

if [ -f "$TOOLCHAIN_ARCHIVE" ] && [ -f "$TOOLCHAIN_SUMS" ]; then
	if (cd "$ROOT/toolchain" && sha256sum -c "$(basename "$TOOLCHAIN_SUMS")" >/dev/null 2>&1); then
		printf 'ok: toolchain checksum\n'
	else
		printf 'invalid: toolchain checksum\n'
		FAIL=1
	fi
fi

if [ -f "$ROOT/kagami/branding/grub.cfg" ]; then
	printf 'ok: branding grub.cfg\n'
else
	printf 'missing: %s\n' "$ROOT/kagami/branding/grub.cfg"
	FAIL=1
fi

if ls /usr/lib/grub/*/command.lst >/dev/null 2>&1; then
	if grep -q 'knetbsd' /usr/lib/grub/*/command.lst; then
		printf 'ok: GRUB knetbsd\n'
	else
		printf 'missing: GRUB knetbsd module\n'
		FAIL=1
	fi
else
	warn 'GRUB module directory not found'
fi

AVAIL=$(df -Pk "$ROOT" | awk 'NR == 2 { print $4 }')
if [ -n "$AVAIL" ] && [ "$AVAIL" -lt 1048576 ]; then
	warn 'less than 1 GiB free'
else
	printf 'ok: free space\n'
fi

if [ "$FAIL" -ne 0 ]; then
	exit 1
fi

printf 'all required checks passed\n'
