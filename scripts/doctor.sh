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
check_cmd gcc
check_cmd git
check_cmd grub-mkrescue
check_cmd grub-mkimage
check_cmd xorriso
check_cmd mcopy
check_cmd mtools
check_cmd qemu-system-x86_64

if [ -x "$ROOT/kernel/src/build.sh" ]; then
	printf 'ok: NetBSD source\n'
else
	warn "NetBSD source not found: $ROOT/kernel/src (run make fetch-source)"
fi

if [ -x "$ROOT/tools/bin/x86_64--netbsd-gcc" ]; then
	printf 'ok: NetBSD gcc toolchain\n'
else
	warn "NetBSD gcc toolchain not found (run make bootstrap)"
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

if [ -d "$ROOT/kernel/src/.git" ] && [ -f "$ROOT/kagami/netbsd.rev" ]; then
	WANT=$(tr -d '[:space:]' < "$ROOT/kagami/netbsd.rev")
	GOT=$(git -C "$ROOT/kernel/src" rev-parse HEAD 2>/dev/null || true)
	if [ -n "$GOT" ] && [ "$WANT" != "$GOT" ]; then
		warn "NetBSD source is $GOT, pinned is $WANT"
	else
		printf 'ok: NetBSD source revision\n'
	fi
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
