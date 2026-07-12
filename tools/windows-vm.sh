#!/usr/bin/env bash
# Create and drive a Windows 11 VM (QEMU/KVM via quickemu) to build and run
# the FTXUI tests on Windows, from a Linux host.
#
# Usage:
#   ./tools/windows-vm.sh setup      # install quickemu + download Windows 11
#   ./tools/windows-vm.sh start      # boot the VM (first boot auto-installs)
#   ./tools/windows-vm.sh provision  # set up ssh + build tools in the guest
#   ./tools/windows-vm.sh test       # upload the working tree, build, run tests
#   ./tools/windows-vm.sh ssh [cmd]  # interactive shell / run a command
#   ./tools/windows-vm.sh status     # check whether the guest is reachable
#
# The only manual step is during `provision`: Windows provides no channel for
# automation until ssh is installed, so you must type one bootstrap command in
# a terminal inside the VM. Everything before and after is automated.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VM_DIR="$ROOT/tools/windows-vm/data"
PROVISION_PS1="$ROOT/tools/windows-vm/provision.ps1"
KEY="$VM_DIR/id_ed25519"

# quickemu forwards the guest ssh port to 22220 on the host by default.
SSH_PORT=22220
SSH_USER=Quickemu  # user created by quickget's unattended Windows install.
# The guest reaches the host at 10.0.2.2 (QEMU user networking). Must match
# the URL hardcoded in provision.ps1.
HTTP_PORT=8123

SSH_OPTS=(
  -i "$KEY"
  -p "$SSH_PORT"
  -o StrictHostKeyChecking=no
  -o UserKnownHostsFile="$VM_DIR/known_hosts"
  -o ConnectTimeout=5
  -o LogLevel=ERROR
)

run_ssh() {
  ssh "${SSH_OPTS[@]}" "$SSH_USER@localhost" "$@"
}

guest_reachable() {
  run_ssh "exit" 2>/dev/null
}

guest_provisioned() {
  run_ssh "cmd /c if exist provision-done.txt echo DONE" 2>/dev/null | grep -q "DONE"
}

# quickget downloads with a browser-like user agent, which the anti-bot proxy
# in front of fedorapeople.org rejects: it saves an HTML challenge page as
# virtio-win.iso. curl's default user agent passes through, so fetch the ISO
# ourselves when quickget's copy is missing or bogus (a real one is >500MB).
fetch_virtio() {
  local iso="$VM_DIR/windows-11/virtio-win.iso"
  if [ -f "$iso" ] && [ "$(stat -c %s "$iso")" -gt 100000000 ]; then
    return
  fi
  echo "Fetching virtio-win.iso (quickget's copy is missing or not an ISO)..."
  rm -f "$iso"
  curl -L --fail --progress-bar -o "$iso" \
    https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/stable-virtio/virtio-win.iso
}

# quickget's autounattend.xml has two driver paths that break Windows 11
# 24H2+ installs (quickemu-project/quickemu#1475 and #1620): E:\amd64\w10
# loads vioscsi/viostor a second time, failing setup with 0x80070103, and
# viogpudo in the windowsPE pass causes a black screen. Drop both, and
# reinstall viogpudo at first logon instead.
patch_unattended() {
  local xml="$VM_DIR/windows-11/unattended/autounattend.xml"
  [ -f "$xml" ] || return 0
  if ! grep -q "quickemu#1620" "$xml"; then
    echo "Patching autounattend.xml (quickemu#1475, quickemu#1620)..."
    python3 - "$xml" <<'PYEOF'
import re
import sys

path = sys.argv[1]
s = open(path).read()

for driver in (r'E:\amd64\w10', r'E:\viogpudo\w10\amd64'):
    pattern = re.compile(
        r'\s*<PathAndCredentials wcm:action="add" wcm:keyValue="\d+">\s*'
        r'<Path>' + re.escape(driver) + r'</Path>\s*'
        r'</PathAndCredentials>')
    s = pattern.sub('', s)

addition = '''        <SynchronousCommand wcm:action="add">
          <CommandLine>pnputil /add-driver E:\\viogpudo\\w10\\amd64\\viogpudo.inf /install</CommandLine>
          <Description>Install virtio GPU driver (skipped during windowsPE, quickemu#1620)</Description>
          <Order>6</Order>
        </SynchronousCommand>
      </FirstLogonCommands>'''
s = s.replace('      </FirstLogonCommands>', addition)

open(path, 'w').write(s)
PYEOF
  fi
  genisoimage -quiet -o "$VM_DIR/windows-11/unattended.iso" -V CDROM -J -R \
    "$VM_DIR/windows-11/unattended/"
}

cmd_setup() {
  if ! command -v quickemu >/dev/null; then
    echo "Installing quickemu..."
    sudo apt-get install -y quickemu || {
      echo "quickemu not in the archive; using the maintainer's PPA."
      sudo add-apt-repository -y ppa:flexiondotorg/quickemu
      sudo apt-get update
      sudo apt-get install -y quickemu
    }
  fi

  local free_gb
  free_gb=$(df --output=avail -BG "$ROOT" | tail -1 | tr -dc '0-9')
  if [ "$free_gb" -lt 50 ]; then
    echo "WARNING: only ${free_gb}G free. The VM can grow to ~40G." >&2
  fi

  mkdir -p "$VM_DIR"
  if [ ! -e "$VM_DIR/windows-11/windows-11.iso" ]; then
    echo "Downloading the Windows 11 ISO (several GB, this takes a while)..."
    (cd "$VM_DIR" && quickget windows 11) || true
  fi
  fetch_virtio
  patch_unattended

  # quickget can fail silently (e.g. blocked downloads); verify the result.
  local f
  for f in windows-11.conf windows-11/windows-11.iso windows-11/virtio-win.iso; do
    [ -s "$VM_DIR/$f" ] || {
      echo "ERROR: $f is missing; the download failed. See output above." >&2
      exit 1
    }
  done
  echo
  echo "Done. Next: ./tools/windows-vm.sh start"
  echo "The first boot runs the unattended Windows install (15-30 min)."
}

cmd_start() {
  [ -e "$VM_DIR/windows-11.conf" ] || {
    echo "No VM found. Run: ./tools/windows-vm.sh setup" >&2
    exit 1
  }
  (cd "$VM_DIR" && quickemu --vm windows-11.conf --display spice)
}

cmd_provision() {
  if guest_reachable && guest_provisioned; then
    echo "Guest is already provisioned."
    return
  fi

  [ -f "$KEY" ] || ssh-keygen -t ed25519 -N '' -f "$KEY" -C ftxui-windows-vm

  # Serve the provisioning script and the public key to the guest over HTTP.
  local serve_dir
  serve_dir=$(mktemp -d)
  cp "$PROVISION_PS1" "$serve_dir/provision.ps1"
  cp "$KEY.pub" "$serve_dir/id_ed25519.pub"
  python3 -m http.server "$HTTP_PORT" --directory "$serve_dir" --bind 0.0.0.0 \
    >/dev/null 2>&1 &
  local server_pid=$!
  # shellcheck disable=SC2064
  trap "kill $server_pid 2>/dev/null; rm -rf '$serve_dir'" EXIT

  cat <<EOF

In the VM, open «Terminal (Admin)» (right click on the start menu), and run:

    irm http://10.0.2.2:$HTTP_PORT/provision.ps1 | iex

This installs the ssh server, then Git, CMake and the MSVC Build Tools
(~3GB download, expect 15-45 min). Waiting for it to complete...

EOF

  local start elapsed
  start=$(date +%s)
  until guest_reachable && guest_provisioned; do
    sleep 15
    elapsed=$(( ($(date +%s) - start) / 60 ))
    echo "  ... still waiting (${elapsed} min). VS Build Tools install is slow."
    if [ "$elapsed" -ge 90 ]; then
      echo "Timed out after 90 min. Check the VM window for errors." >&2
      exit 1
    fi
  done
  echo "Guest provisioned and reachable over ssh."
  echo "Next: ./tools/windows-vm.sh test"
}

cmd_test() {
  guest_reachable || {
    echo "Guest not reachable. Is the VM started and provisioned?" >&2
    exit 1
  }

  local cmake='"C:\Program Files\CMake\bin\cmake.exe"'
  local ctest='"C:\Program Files\CMake\bin\ctest.exe"'

  echo "Uploading the working tree (tracked files, including local changes)..."
  git -C "$ROOT" ls-files -z |
    tar -C "$ROOT" --null -T - -czf - |
    run_ssh "cmd /c (if exist ftxui-src rmdir /s /q ftxui-src) & mkdir ftxui-src & tar xzf - -C ftxui-src"

  echo "Configuring (MSVC is auto-detected by CMake)..."
  run_ssh "$cmake -S ftxui-src -B ftxui-build -DFTXUI_BUILD_TESTS=ON -DFTXUI_BUILD_EXAMPLES=ON"

  echo "Building..."
  run_ssh "$cmake --build ftxui-build --config Release --parallel"

  echo "Running the tests..."
  run_ssh "$ctest --test-dir ftxui-build -C Release --output-on-failure"

  cat <<'EOF'

Automated tests passed. Manual visual checks, inside the VM:

  Windows Terminal (TrueColor gradients, issue #1305) - run in PowerShell:
    C:\Users\Quickemu\ftxui-build\examples\component\Release\ftxui_example_button_style.exe

  Classic conhost (no WT_SESSION, must still be TrueColor) - run in PowerShell:
    conhost.exe C:\Users\Quickemu\ftxui-build\examples\component\Release\ftxui_example_button_style.exe

  Redirected output (GetConsoleMode failure path) - run in cmd.exe (Command Prompt):
    C:\Users\Quickemu\ftxui-build\examples\dom\Release\ftxui_example_color_gallery.exe > out.txt & type out.txt
EOF
}

cmd_ssh() {
  if [ $# -eq 0 ]; then
    ssh "${SSH_OPTS[@]}" "$SSH_USER@localhost"
  else
    run_ssh "$@"
  fi
}

cmd_status() {
  if guest_reachable; then
    if guest_provisioned; then
      echo "Guest is reachable and provisioned."
    else
      echo "Guest is reachable but not provisioned yet."
    fi
  else
    echo "Guest is not reachable on port $SSH_PORT."
  fi
}

case "${1:-help}" in
  setup) cmd_setup ;;
  start) cmd_start ;;
  provision) cmd_provision ;;
  test) shift; cmd_test "$@" ;;
  ssh) shift; cmd_ssh "$@" ;;
  status) cmd_status ;;
  *) grep '^#' "$0" | head -17 | sed 's/^# \?//' ;;
esac
