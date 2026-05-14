#!/bin/bash

# This script checks for ABI changes between two versions of the FTXUI library.
# It uses 'nm' to compare exported symbols.
#
# Usage: ./tools/check_abi_change.sh <commit1> <commit2>
# Example: ./tools/check_abi_change.sh v3.0.0 HEAD

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check dependencies
for cmd in cmake git nm; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: '$cmd' not found.${NC}"
        exit 1
    fi
done

# Optional deep analysis tool
HAS_ABIDIFF=0
if command -v abidiff &> /dev/null; then
    HAS_ABIDIFF=1
fi

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <commit1> <commit2>"
    exit 1
fi

COMMIT1=$1
COMMIT2=$2

# Create a temporary directory for the worktrees
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR; git worktree prune &> /dev/null" EXIT

echo -e "${YELLOW}Checking ABI changes between $COMMIT1 and $COMMIT2...${NC}"

# Optimization: Check fingerprints if available
# We assume COMMIT2 is usually the current state (HEAD)
get_fingerprint() {
    local commit=$1
    # Try to read the fingerprint file from the commit
    git show "$commit:tools/abi_fingerprint.txt" 2>/dev/null || echo ""
}

FP1=$(get_fingerprint "$COMMIT1")
FP2=$(get_fingerprint "$COMMIT2")

# If we are comparing against HEAD and it's not committed yet, we'll need to generate it
if [ "$COMMIT2" == "HEAD" ] || [ "$COMMIT2" == "." ]; then
    if [ -d "build" ] && [ -f "build/libftxui-screen.so" ]; then
        FP2=$(./tools/generate_abi_fingerprint.sh build)
    fi
fi

if [ -n "$FP1" ] && [ -n "$FP2" ] && [ "$FP1" == "$FP2" ]; then
    echo -e "${GREEN}ABI fingerprints match ($FP1). No changes detected.${NC}"
    exit 0
fi

if [ -n "$FP1" ] && [ -n "$FP2" ]; then
    echo -e "${YELLOW}ABI fingerprints differ ($FP1 -> $FP2). Performing deep analysis...${NC}"
fi

build_commit() {
    local commit=$1
    local path=$2
    
    echo -e "${YELLOW}Building $commit...${NC}"
    # Create worktree
    git worktree add "$path" "$commit" --detach > /dev/null 2>&1
    
    pushd "$path" > /dev/null
    # Configure and build
    cmake -B build \
          -DBUILD_SHARED_LIBS=ON \
          -DCMAKE_BUILD_TYPE=Debug \
          -DFTXUI_BUILD_EXAMPLES=OFF \
          -DFTXUI_BUILD_TESTS=OFF \
          -DFTXUI_BUILD_MODULES=OFF \
          -DFTXUI_BUILD_DOCS=OFF \
          -Wno-dev \
          > /dev/null
    cmake --build build --parallel > /dev/null
    popd > /dev/null
}

PATH1="$TEMP_DIR/v1"
PATH2="$TEMP_DIR/v2"

build_commit "$COMMIT1" "$PATH1"
build_commit "$COMMIT2" "$PATH2"

LIBS=("screen" "dom" "component")
HAS_CHANGES=0

count_lines() {
    wc -l | xargs
}

# Compare the symbol lists
for lib in "${LIBS[@]}"; do
    SO_FILE="libftxui-$lib.so"
    FILE1="$PATH1/build/$SO_FILE"
    FILE2="$PATH2/build/$SO_FILE"
    
    echo "--------------------------------------------------------------------------------"
    echo -e "${YELLOW}Comparing $SO_FILE...${NC}"
    
    if [ ! -f "$FILE1" ]; then
        echo -e "${RED}Warning: $FILE1 not found in $COMMIT1. Skipping.${NC}"
        continue
    fi
    if [ ! -f "$FILE2" ]; then
        echo -e "${RED}Warning: $FILE2 not found in $COMMIT2. Skipping.${NC}"
        continue
    fi

    # Use abidiff for deep analysis if available
    if [ $HAS_ABIDIFF -eq 1 ]; then
        if abidiff "$FILE1" "$FILE2" > "$TEMP_DIR/diff_$lib"; then
            echo -e "${GREEN}No ABI changes detected for $SO_FILE (via abidiff).${NC}"
            continue
        fi
    else
        # Fallback to nm for symbol comparison
        nm -C -D --defined-only "$FILE1" | cut -c 20- | sort > "$TEMP_DIR/symbols_$lib.1"
        nm -C -D --defined-only "$FILE2" | cut -c 20- | sort > "$TEMP_DIR/symbols_$lib.2"
        if diff -u "$TEMP_DIR/symbols_$lib.1" "$TEMP_DIR/symbols_$lib.2" > "$TEMP_DIR/diff_$lib"; then
             echo -e "${GREEN}No exported symbol changes detected for $SO_FILE.${NC}"
             continue
        fi
    fi

    echo -e "${RED}ABI changes detected for $SO_FILE!${NC}"
    
    # Analyze the diff for compatibility
    REMOVALS=$(grep "^-" "$TEMP_DIR/diff_$lib" | grep -v "^---" | count_lines || true)
    ADDITIONS=$(grep "^+" "$TEMP_DIR/diff_$lib" | grep -v "^+++" | count_lines || true)

    if [ "$REMOVALS" -gt 0 ]; then
        echo -e "${RED}  - BACKWARD INCOMPATIBLE: symbols removed or changed.${NC}"
        echo -e "    (Binaries compiled against $COMMIT1 will NOT work with $COMMIT2)"
    fi
    if [ "$ADDITIONS" -gt 0 ]; then
        echo -e "${YELLOW}  - FORWARD INCOMPATIBLE: symbols added.${NC}"
        echo -e "    (Binaries compiled against $COMMIT2 will NOT work with $COMMIT1)"
    fi
    
    echo -e "\nReport:"
    cat "$TEMP_DIR/diff_$lib"
    HAS_CHANGES=1
done

echo "--------------------------------------------------------------------------------"
if [ $HAS_CHANGES -eq 1 ]; then
    echo -e "${RED}ABI changes (exported symbols) were detected between $COMMIT1 and $COMMIT2.${NC}"
    exit 1
else
    echo -e "${GREEN}No changes in exported symbols detected.${NC}"
    exit 0
fi
