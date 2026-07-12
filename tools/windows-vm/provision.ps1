# Provision a Windows 11 guest for building and testing FTXUI.
# Run from an elevated terminal inside the VM, bootstrapped by
# ../windows-vm.sh provision:
#
#     irm http://10.0.2.2:8123/provision.ps1 | iex
#
# Installs the OpenSSH server (so the Linux host can drive the guest), then
# Git, CMake and the MSVC Build Tools. Writes %USERPROFILE%\provision-done.txt
# when finished; the host polls for that file.
$ErrorActionPreference = 'Stop'
$hostUrl = 'http://10.0.2.2:8123'  # Linux host, as seen from QEMU user networking.

$isAdmin = ([Security.Principal.WindowsPrincipal] `
  [Security.Principal.WindowsIdentity]::GetCurrent()
).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
  throw 'This script must run from an elevated (Admin) terminal.'
}

Write-Host '=== [1/3] OpenSSH server ==='
Add-WindowsCapability -Online -Name OpenSSH.Server~~~~0.0.1.0 | Out-Null
Set-Service -Name sshd -StartupType Automatic
Start-Service sshd
if (-not (Get-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -ErrorAction SilentlyContinue)) {
  New-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' `
    -DisplayName 'OpenSSH Server (sshd)' -Enabled True -Direction Inbound `
    -Protocol TCP -Action Allow -LocalPort 22 -Profile Any | Out-Null
} else {
  Set-NetFirewallRule -Name 'OpenSSH-Server-In-TCP' -Profile Any
}

# Ensure SPICE agent and WebDAV services are set to Automatic and running
Get-Service -Name spice-agent, spice-webdavd -ErrorAction SilentlyContinue | ForEach-Object {
  Set-Service -Name $_.Name -StartupType Automatic
  Start-Service $_.Name -ErrorAction SilentlyContinue
}


# Install the host's public key. Administrator accounts use a shared,
# strictly-ACLed authorized_keys file. Use SIDs (S-1-5-32-544 =
# Administrators, S-1-5-18 = SYSTEM) so this works on localized Windows.
$authKeys = 'C:\ProgramData\ssh\administrators_authorized_keys'
Invoke-RestMethod "$hostUrl/id_ed25519.pub" | Out-File -Encoding ascii $authKeys
icacls $authKeys /inheritance:r /grant '*S-1-5-32-544:F' /grant '*S-1-5-18:F' | Out-Null
Write-Host 'sshd is running; the host can now reach this guest.'

Write-Host '=== [2/3] Git and CMake ==='
winget install --id Git.Git -e --silent `
  --accept-package-agreements --accept-source-agreements
winget install --id Kitware.CMake -e --silent `
  --accept-package-agreements --accept-source-agreements

Write-Host '=== [3/3] MSVC Build Tools (~3GB, this is the slow part) ==='
winget install --id Microsoft.VisualStudio.2022.BuildTools -e --silent `
  --accept-package-agreements --accept-source-agreements `
  --override '--quiet --wait --norestart --nocache --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended'

New-Item -ItemType File -Force "$env:USERPROFILE\provision-done.txt" | Out-Null
Write-Host 'Provisioning done. You can go back to the Linux terminal.'
