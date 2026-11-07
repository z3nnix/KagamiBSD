#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
SRC="$ROOT/kernel/src"
REV_FILE="$ROOT/kagami/netbsd.rev"

if [ ! -f "$REV_FILE" ]; then
	printf 'missing revision file: %s\n' "$REV_FILE" >&2
	exit 1
fi

REV=$(tr -d '[:space:]' < "$REV_FILE")

if [ ! -d "$SRC/.git" ]; then
	mkdir -p "$ROOT/kernel"
	git clone --filter=blob:none https://github.com/NetBSD/src.git "$SRC"
fi

if [ -n "$(git -C "$SRC" status --porcelain)" ]; then
	printf 'source tree has local changes: %s\n' "$SRC" >&2
	exit 1
fi

if ! git -C "$SRC" cat-file -e "$REV^{commit}" 2>/dev/null; then
	git -C "$SRC" fetch --depth=1 origin "$REV" || git -C "$SRC" fetch origin
fi

git -C "$SRC" checkout --detach "$REV"
printf 'NetBSD source: %s\n' "$REV"
