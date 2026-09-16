#!/bin/bash
set -eu
cd "$(dirname "$0")"
ulimit -n 4096
mkdir -p build
export ZIG_GLOBAL_CACHE_DIR="$PWD/build/zig-cache"
exec ninja "$@"
