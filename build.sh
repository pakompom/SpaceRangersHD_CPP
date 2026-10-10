#!/bin/bash
set -eu
cd "$(dirname "$0")"
case "$OSTYPE" in
  msys*|cygwin*|win32*) ;;
  *) ulimit -n 4096 ;;
esac
mkdir -p build
export ZIG_GLOBAL_CACHE_DIR="$PWD/build/zig-cache"
exec ninja "$@"
python3 tools/large_address_aware.py build/Rangers.exe