#!/bin/bash

# This script checks for ABI changes between two versions of the FTXUI library.
# It uses 'abidiff' from the 'abigail-tools' package.
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
for cmd in cmake git abidiff; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: '$cmd' not found.${NC}"
        [ "$cmd" == "abidiff" ] && echo "Please install 'abigail-tools'."
        exit 1
    fi
done

if [ "$1" == "--accept" ]; then
    if [ ! -d "build" ]; then
        echo -e "${RED}Error: Build directory not found. Please build the project first.${NC}"
        exit 1
    fi
    NEW_FP=$(./tools/generate_abi_fingerprint.sh build)
    echo "$NEW_FP" > tools/abi_fingerprint.txt
    echo -e "${GREEN}ABI fingerprint updated to $NEW_FP.${NC}"
    exit 0
fi

if [ "$#" -ne 2 ]; then
    echo "Usage:"
    echo "  $0 <commit1> <commit2>    # Compare two versions"
    echo "  $0 --accept               # Update the golden fingerprint from current build"
    exit 1
fi

COMMIT1=$1
COMMIT2=$2

get_version() {
    local commit=$1
    git show "$commit:CMakeLists.txt" | grep "VERSION" | grep -v "REQUIRED" | head -n 1 | awk '{print $2}' | tr -d ')'
}

V1=$(get_version "$COMMIT1")
V2=$(get_version "$COMMIT2")
V1_MAJOR=$(echo "$V1" | cut -d. -f1)
V1_MINOR=$(echo "$V1" | cut -d. -f2)
V2_MAJOR=$(echo "$V2" | cut -d. -f1)
V2_MINOR=$(echo "$V2" | cut -d. -f2)

# Create a temporary directory for the worktrees
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR; git worktree prune &> /dev/null" EXIT

echo -e "${YELLOW}Checking ABI changes between $COMMIT1 and $COMMIT2...${NC}"

# Optimization: Check fingerprints if available
get_fingerprint() {
    local commit=$1
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

build_commit() {
    local commit=$1
    local path=$2
    
    echo -e "${YELLOW}Building $commit...${NC}"
    git worktree add "$path" "$commit" --detach > /dev/null 2>&1
    
    pushd "$path" > /dev/null
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

# Compare using abidiff
for lib in "${LIBS[@]}"; do
    SO_FILE="libftxui-$lib.so"
    FILE1="$PATH1/build/$SO_FILE"
    FILE2="$PATH2/build/$SO_FILE"
    
    echo "--------------------------------------------------------------------------------"
    echo -e "${YELLOW}Comparing $SO_FILE...${NC}"
    
    if [ ! -f "$FILE1" ] || [ ! -f "$FILE2" ]; then
        echo -e "${RED}Warning: $SO_FILE missing in one of the versions. Skipping.${NC}"
        continue
    fi

    # abidiff returns 0 if no changes
    # Use suppressions to ignore STL noise and keep it focused on FTXUI
    if abidiff --suppressions tools/abidiff_suppressions.ini "$FILE1" "$FILE2" > "$TEMP_DIR/diff_$lib" 2>/dev/null; then
        echo -e "${GREEN}No ABI changes detected for $SO_FILE.${NC}"
    else
        echo -e "${RED}ABI changes detected for $SO_FILE!${NC}"
        
        IS_BACKWARD_INCOMPATIBLE=0
        IS_FORWARD_INCOMPATIBLE=0

        # Check for removals (Backward Incompatible)
        if grep -q "removed" "$TEMP_DIR/diff_$lib" || grep -q "changed" "$TEMP_DIR/diff_$lib"; then
            echo -e "${RED}  - BACKWARD INCOMPATIBLE CHANGES DETECTED.${NC}"
            IS_BACKWARD_INCOMPATIBLE=1
        fi
        
        # Check for additions (Forward Incompatible)
        if grep -q "added" "$TEMP_DIR/diff_$lib"; then
            echo -e "${YELLOW}  - FORWARD INCOMPATIBLE CHANGES DETECTED.${NC}"
            IS_FORWARD_INCOMPATIBLE=1
        fi

        # Version enforcement
        if [ $IS_BACKWARD_INCOMPATIBLE -eq 1 ]; then
            if [ "$V2_MAJOR" -le "$V1_MAJOR" ]; then
                echo -e "${RED}  - ERROR: Major version bump required ($V1 -> $((V1_MAJOR + 1)).0.0).${NC}"
                echo -e "${RED}    Backward incompatible changes detected but VERSION_MAJOR was not incremented.${NC}"
                HAS_CHANGES=1
            fi
        elif [ $IS_FORWARD_INCOMPATIBLE -eq 1 ]; then
            if [ "$V2_MAJOR" -eq "$V1_MAJOR" ] && [ "$V2_MINOR" -le "$V1_MINOR" ]; then
                echo -e "${RED}  - ERROR: Minor version bump required ($V1 -> $V1_MAJOR.$((V1_MINOR + 1)).0).${NC}"
                echo -e "${RED}    New features detected but VERSION_MINOR was not incremented.${NC}"
                HAS_CHANGES=1
            fi
        fi

        echo -e "\nSummary of changes:"
        grep -E "summary:|functions|variables|symbols" "$TEMP_DIR/diff_$lib" | head -n 5
        
        echo -e "\nDetailed Report (filtered for ftxui::):"
        # Only show lines containing ftxui or the start of sections, and limit output
        grep -E "ftxui::|Removed functions:|Changed functions:|Added functions:" "$TEMP_DIR/diff_$lib" | head -n 50
        [ $(wc -l < "$TEMP_DIR/diff_$lib") -gt 50 ] && echo "... (see full log for details)"
        
        HAS_CHANGES=1
    fi
done

echo "--------------------------------------------------------------------------------"
if [ $HAS_CHANGES -eq 1 ]; then
    echo -e "${RED}ABI changes detected between $COMMIT1 and $COMMIT2.${NC}"
    exit 1
else
    echo -e "${GREEN}No ABI changes detected.${NC}"
    exit 0
fi
