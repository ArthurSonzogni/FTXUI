#!/bin/bash

# This script lints the GitHub Actions workflows with actionlint.
#
# actionlint additionally runs shellcheck over every "run:" block when that
# tool is available, so both are set up here.
#
# Tools already on the PATH are used as is. Otherwise a pinned release is
# downloaded into the user cache directory, so nothing is installed system
# wide and the download only happens once.
#
# Usage: ./tools/check_workflows.sh [extra actionlint arguments...]

set -euo pipefail

cd "$(dirname "$0")"
cd ..

# Pinned so that a new release cannot break CI unexpectedly.
# Override with: ACTIONLINT_VERSION=1.2.3 ./tools/check_workflows.sh
ACTIONLINT_VERSION="${ACTIONLINT_VERSION:-1.7.12}"
SHELLCHECK_VERSION="${SHELLCHECK_VERSION:-0.11.0}"
CACHE_DIR="${XDG_CACHE_HOME:-$HOME/.cache}/ftxui"

# The upstream project publishes no checksum file, so the expected hashes are
# pinned here. Refresh them when bumping SHELLCHECK_VERSION, with:
#   sha256sum shellcheck-v<version>.<platform>.tar.gz
shellcheck_sha256() {
    case "$1" in
        linux.x86_64)    echo "b7af85e41cc99489dcc21d66c6d5f3685138f06d34651e6d34b42ec6d54fe6f6" ;;
        linux.aarch64)   echo "68a8133197a50beb8803f8d42f9908d1af1c5540d4bb05fdfca8c1fa47decefc" ;;
        darwin.x86_64)   echo "c2c15e08df0e8fbc374c335b230a7ee958c313fa5714817a59aa59f1aa594f51" ;;
        darwin.aarch64)  echo "339b930feb1ea764467013cc1f72d09cd6b869ebf1013296ba9055ab2ffbd26f" ;;
    esac
}

sha256() {
    if command -v sha256sum > /dev/null 2>&1; then
        sha256sum "$1" | cut -d' ' -f1
    else
        shasum -a 256 "$1" | cut -d' ' -f1
    fi
}

unsupported() {
    echo "Error: unsupported platform $(uname -s) $(uname -m)." >&2
    echo "Install $1 manually: $2" >&2
    exit 1
}

# Sets the OS and ARCH globals, using the naming of each project.
detect_platform() {
    case "$(uname -s)" in
        Linux)  OS="linux"  ;;
        Darwin) OS="darwin" ;;
        *)      return 1    ;;
    esac

    case "$(uname -m)" in
        x86_64|amd64)  ARCH="amd64" ;;
        arm64|aarch64) ARCH="arm64" ;;
        *)             return 1     ;;
    esac
}

verify_and_install() {
    local archive="$1" expected="$2" destination="$3"
    local actual
    actual=$(sha256 "$archive")
    if [ "$expected" != "$actual" ]; then
        echo "Error: checksum mismatch for $(basename "$archive")." >&2
        echo "  expected: $expected" >&2
        echo "  actual:   $actual" >&2
        exit 1
    fi
    mkdir -p "$CACHE_DIR"
    mv "$destination" "$CACHE_DIR/"
    chmod +x "$CACHE_DIR/$(basename "$destination")"
}

download_actionlint() {
    local url asset dir expected
    url="https://github.com/rhysd/actionlint/releases/download/v${ACTIONLINT_VERSION}"
    asset="actionlint_${ACTIONLINT_VERSION}_${OS}_${ARCH}.tar.gz"
    dir=$(mktemp -d)
    trap 'rm -rf "$dir"' RETURN

    echo "Downloading actionlint ${ACTIONLINT_VERSION} (${OS}/${ARCH})..." >&2
    curl -sSfL "${url}/${asset}" -o "$dir/$asset"
    curl -sSfL "${url}/actionlint_${ACTIONLINT_VERSION}_checksums.txt" \
        -o "$dir/checksums.txt"

    # Note: the checksum comes from the same release, so this catches a
    # truncated or corrupted download, not a compromised upstream release.
    expected=$(grep " ${asset}\$" "$dir/checksums.txt" | cut -d' ' -f1)
    tar -xzf "$dir/$asset" -C "$dir" actionlint
    verify_and_install "$dir/$asset" "$expected" "$dir/actionlint"
}

download_shellcheck() {
    local platform asset dir expected
    # The asset names use uname style rather than Go style.
    case "${OS}/${ARCH}" in
        linux/amd64)  platform="linux.x86_64"   ;;
        linux/arm64)  platform="linux.aarch64"  ;;
        darwin/amd64) platform="darwin.x86_64"  ;;
        darwin/arm64) platform="darwin.aarch64" ;;
    esac

    expected=$(shellcheck_sha256 "$platform")
    if [ -z "$expected" ]; then
        echo "Warning: no pinned checksum for shellcheck on $platform." >&2
        echo "Skipping shellcheck: run: blocks will not be linted." >&2
        return 1
    fi

    asset="shellcheck-v${SHELLCHECK_VERSION}.${platform}.tar.gz"
    dir=$(mktemp -d)
    trap 'rm -rf "$dir"' RETURN

    echo "Downloading shellcheck ${SHELLCHECK_VERSION} (${platform})..." >&2
    curl -sSfL \
        "https://github.com/koalaman/shellcheck/releases/download/v${SHELLCHECK_VERSION}/${asset}" \
        -o "$dir/$asset"

    tar -xzf "$dir/$asset" -C "$dir"
    verify_and_install "$dir/$asset" "$expected" \
        "$dir/shellcheck-v${SHELLCHECK_VERSION}/shellcheck"
}

detect_platform || unsupported "the tools" "https://github.com/rhysd/actionlint"

# Resolve actionlint, downloading it if needed.
if command -v actionlint > /dev/null 2>&1; then
    ACTIONLINT=$(command -v actionlint)
else
    if [ ! -x "$CACHE_DIR/actionlint" ]; then
        download_actionlint
    fi
    ACTIONLINT="$CACHE_DIR/actionlint"
fi

# Resolve shellcheck, so that actionlint lints the run: blocks too. It is a
# nice to have: when it cannot be installed the workflow checks still run.
if ! command -v shellcheck > /dev/null 2>&1; then
    if [ ! -x "$CACHE_DIR/shellcheck" ]; then
        download_shellcheck || true
    fi
    if [ -x "$CACHE_DIR/shellcheck" ]; then
        PATH="$CACHE_DIR:$PATH"
        export PATH
    fi
fi

if ! command -v shellcheck > /dev/null 2>&1; then
    echo "Warning: shellcheck not available, run: blocks are not linted." >&2
fi

# Collect the workflows. The glob is not expanded when nothing matches, so the
# files are gathered explicitly.
mapfile -t WORKFLOWS < <(find .github/workflows -maxdepth 1 \
    \( -name '*.yaml' -o -name '*.yml' \) | sort)

if [ ${#WORKFLOWS[@]} -eq 0 ]; then
    echo "Error: no workflow found in .github/workflows." >&2
    exit 1
fi

echo "Checking ${#WORKFLOWS[@]} workflows with actionlint $("$ACTIONLINT" --version | head -n1)..."
"$ACTIONLINT" "$@" "${WORKFLOWS[@]}"
echo "All workflows are valid."
