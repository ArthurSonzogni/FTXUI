#!/bin/bash

# This script generates a fingerprint of the FTXUI ABI.
# It hashes the exported symbols of the shared libraries and the public headers.
#
# Usage: ./tools/generate_abi_fingerprint.sh <build_dir>

set -e

# Disable debuginfod to avoid network delays
export DEBUGINFOD_URLS=""

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <build_dir>"
    exit 1
fi

BUILD_DIR=$1

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory $BUILD_DIR not found."
    exit 1
fi

LIBS=("screen" "dom" "component")
FINGERPRINT_FILE=$(mktemp)

# Hash exported symbols using normalized abidw output.
# This captures all public classes, structs, member variables, offsets, sizes,
# and function signatures, while remaining stable across environments.
for lib in "${LIBS[@]}"; do
    SO_FILE="$BUILD_DIR/libftxui-$lib.so"
    if [ -f "$SO_FILE" ]; then
        # abidw flags for stability:
        # --no-architecture: avoid amd64 vs aarch64 noise
        # --no-corpus-path: avoid absolute path noise
        # --no-show-locs: avoid header path noise
        # --no-comp-dir-path: avoid build dir noise
        abidw --no-architecture \
              --no-corpus-path \
              --no-show-locs \
              --no-comp-dir-path \
              --suppressions tools/abidiff_suppressions.ini \
              "$SO_FILE" | \
              ./tools/normalize_abidw.py >> "$FINGERPRINT_FILE"
    else
        echo "Warning: $SO_FILE not found. Skipping symbol hash for $lib."
    fi
done

# 3. Generate final hash
sha256sum "$FINGERPRINT_FILE" | cut -d' ' -f1
rm "$FINGERPRINT_FILE"
