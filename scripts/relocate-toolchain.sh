#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
TOOLS="$ROOT/tools"
PREFIX_FILE="$ROOT/toolchain/BUILD_PREFIX"

if [ ! -d "$TOOLS" ]; then
	printf 'toolchain directory not found: %s\n' "$TOOLS" >&2
	exit 1
fi

if [ ! -f "$PREFIX_FILE" ]; then
	printf 'build prefix not found: %s\n' "$PREFIX_FILE" >&2
	exit 1
fi

OLD=$(tr -d '[:space:]' < "$PREFIX_FILE")
NEW="$ROOT"

if [ -z "$OLD" ]; then
	printf 'empty build prefix: %s\n' "$PREFIX_FILE" >&2
	exit 1
fi

if [ "$OLD" != "$NEW" ]; then
	find "$TOOLS" -type f -exec grep -IlF "$OLD" {} + | while IFS= read -r file; do
		sed -i "s|$OLD|$NEW|g" "$file"
	done

	if find "$TOOLS" -type f -exec grep -IlF "$OLD" {} + | grep -q .; then
		printf 'warning: stale build prefix remains in %s\n' "$TOOLS" >&2
	fi
fi

find "$TOOLS" -type f -exec grep -IlF "$NEW/kernel/src" {} + | while IFS= read -r file; do
	sed -i "s|$NEW/kernel/src|$NEW/kernel/netbsd|g" "$file"
done

printf 'toolchain relocated to: %s\n' "$NEW"
