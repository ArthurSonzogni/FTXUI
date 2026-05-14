#!/bin/bash

# This script generates a fingerprint of the FTXUI ABI.
# It hashes the exported symbols of the shared libraries and the public headers.
#
# Usage: ./tools/generate_abi_fingerprint.sh <build_dir>

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <build_dir>"
    exit 1
fi

BUILD_DIR=$1
INCLUDE_DIR="include"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory $BUILD_DIR not found."
    exit 1
fi

LIBS=("screen" "dom" "component")
FINGERPRINT_FILE=$(mktemp)

# 1. Hash public headers
find "$INCLUDE_DIR" -type f -name "*.hpp" -print0 | sort -z | xargs -0 sha256sum >> "$FINGERPRINT_FILE"

# 2. Hash exported symbols
for lib in "${LIBS[@]}"; do
    SO_FILE="$BUILD_DIR/libftxui-$lib.so"
    if [ -f "$SO_FILE" ]; then
        # We use mangled symbols here for the hash as they are more precise
        nm -D --defined-only "$SO_FILE" | cut -c 20- | sort >> "$FINGERPRINT_FILE"
    else
        echo "Warning: $SO_FILE not found. Skipping symbol hash for $lib."
    fi
done

# 3. Generate final hash
sha256sum "$FINGERPRINT_FILE" | cut -d' ' -f1
rm "$FINGERPRINT_FILE"
