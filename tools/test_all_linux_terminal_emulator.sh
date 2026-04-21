#!/bin/bash

# This script installs common Linux terminal emulators and runs the
# terminal_info example in each to verify FTXUI's terminal identification
# and capability detection.

set -e

# List of terminal emulators to test
# Format: "package_name:command_to_run:arguments"
TERMINALS=(
  "xterm:xterm:-e"
  "kitty:kitty:"
  "alacritty:alacritty:-e"
  "terminator:terminator:-e"
  "gnome-terminal:gnome-terminal:--"
  "konsole:konsole:-e"
  "xfce4-terminal:xfce4-terminal:-e"
  "tilix:tilix:-e"
  "rxvt-unicode:urxvt:-e"
  "foot:foot:"
)

# Use absolute path for the example
EXAMPLE="$(pwd)/build/examples/component/ftxui_example_terminal_info"

if [ ! -f "$EXAMPLE" ]; then
  echo "Error: Example $EXAMPLE not found. Please build the project first."
  exit 1
fi

echo "This script will attempt to install and run several terminal emulators."
echo "You will be asked to validate FTXUI's output in each one."
echo ""

# Attempt to install missing terminals
MISSING_PACKAGES=()
for t in "${TERMINALS[@]}"; do
  PKG=$(echo "$t" | cut -d: -f1)
  CMD=$(echo "$t" | cut -d: -f2)
  if ! command -v "$CMD" &> /dev/null; then
    MISSING_PACKAGES+=("$PKG")
  fi
done

if [ ${#MISSING_PACKAGES[@]} -gt 0 ]; then
  echo "The following packages are missing: ${MISSING_PACKAGES[*]}"
  if command -v apt-get &> /dev/null; then
    read -p "Do you want to install them using apt-get? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
      sudo apt-get update
      sudo apt-get install -y "${MISSING_PACKAGES[@]}"
    fi
  else
    echo "Please install them manually using your package manager."
  fi
fi

echo ""
echo "--- Starting Tests ---"

for t in "${TERMINALS[@]}"; do
  CMD=$(echo "$t" | cut -d: -f2)
  ARG=$(echo "$t" | cut -d: -f3)

  if command -v "$CMD" &> /dev/null; then
    echo "--------------------------------------------------"
    echo "Testing $CMD..."
    
    if [ -z "$ARG" ]; then
      $CMD "$EXAMPLE" &
    else
      $CMD "$ARG" "$EXAMPLE" &
    fi
    PID=$!
    
    # Wait for the terminal to potentially open and send focus events
    sleep 2
    
    # Clear the input buffer to discard focus sequences
    while read -r -t 0.1 -n 10000 discard; do :; done
    
    echo "Check the newly opened $CMD window."
    echo -n "Did it work correctly? [y]es / [n]o / [s]kip: "
    read -r REPLY
    
    # Try to kill the terminal if it's still running
    kill $PID 2>/dev/null || true
    
    case "$REPLY" in
      [Yy]*) echo ">>> $CMD passed." ;;
      [Nn]*) echo "!!! $CMD failed validation." ;;
      *)     echo "--- $CMD skipped." ;;
    esac
  else
    echo "Skipping $CMD (not installed)."
  fi
  echo ""
done

echo "Tests completed."
