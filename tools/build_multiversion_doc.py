#!/usr/bin/env python3

import os
import subprocess
import shutil
import tempfile
import json
from pathlib import Path
from typing import List, Dict

class VersionInfo:
    """A structure to hold all information about a single documentation version."""
    def __init__(self, name: str, is_main: bool, output_root: Path):
        self.name = name
        self.is_main = is_main
        # Destination directory for the built docs, relative to the output root.
        self.dest_dir = output_root if is_main else output_root / "en" / name
        # The path to this version's index.html, relative to the output root.
        self.index_path_from_root = self.dest_dir / "index.html"

    def __repr__(self) -> str:
        return f"VersionInfo(name='{self.name}', dest_dir='{self.dest_dir}')"

def run_command(command: List[str], check: bool = True, cwd: Path = None):
    """
    Runs a command, prints its output, and handles errors.
    """
    command_str = ' '.join(command)
    print(f"Executing: {command_str} in {cwd or Path.cwd()}")
    try:
        # Using capture_output=True to get stdout/stderr
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=check,
            cwd=cwd
        )
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return result
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Command failed with exit code {e.returncode}")
        print(f"Command: {command_str}")
        if e.stdout:
            print("--- STDOUT ---")
            print(e.stdout)
        if e.stderr:
            print("--- STDERR ---")
            print(e.stderr)
        raise  # Re-raise the exception to halt the script

def get_version_switcher_js(
    current_version: VersionInfo,
    all_versions: List[VersionInfo],
    current_html_file: Path
) -> str:
    """
    Generates the JavaScript for the version switcher dropdown.

    This version pre-calculates the relative path from the current HTML file
    to the index.html of every other version, simplifying the JS logic.
    """
    version_names = [v.name for v in all_versions]

    # Create a dictionary mapping version names to their relative URLs.
    relative_paths: Dict[str, str] = {}
    for version in all_versions:
        # Calculate the relative path from the *parent directory* of the current HTML file
        # to the target version's index.html.
        path = os.path.relpath(version.index_path_from_root, current_html_file.parent)
        relative_paths[version.name] = path

    # Use json.dumps for safe serialization of data into JavaScript.
    versions_json = json.dumps(version_names)
    paths_json = json.dumps(relative_paths)
    current_version_json = json.dumps(current_version.name)

    return f"""
document.addEventListener('DOMContentLoaded', function() {{
  const projectNumber = document.getElementById('projectnumber');
  if (!projectNumber) {{
    console.warn('Doxygen element with ID "projectnumber" not found. Cannot add version switcher.');
    return;
  }}

  const versions = {versions_json};
  const version_paths = {paths_json};
  const currentVersion = {current_version_json};

  // Sort versions: 'main' first, then others numerically descending.
  versions.sort((a, b) => {{
    if (a === 'main') return -1;
    if (b === 'main') return 1;
    return b.localeCompare(a, undefined, {{ numeric: true, sensitivity: 'base' }});
  }});

  const select = document.createElement('select');
  select.onchange = function() {{
    const selectedVersion = this.value;
    // Navigate directly to the pre-calculated relative path.
    if (selectedVersion !== currentVersion) {{
      window.location.href = version_paths[selectedVersion];
    }}
  }};

  versions.forEach(v => {{
    const option = document.createElement('option');
    option.value = v;
    option.textContent = v;
    if (v === currentVersion) {{
      option.selected = true;
    }}
    select.appendChild(option);
  }});

  // Replace the Doxygen project number element with our dropdown.
  projectNumber.replaceWith(select);

  // Apply some styling to make it look good.
  Object.assign(select.style, {{
    backgroundColor: 'rgba(0, 0, 0, 0.8)',
    color: 'white',
    border: '1px solid rgba(255, 255, 255, 0.2)',
    padding: '5px',
    borderRadius: '5px',
    fontSize: '14px',
    fontFamily: 'inherit',
    marginLeft: '10px',
    cursor: 'pointer'
  }});
}});
"""

def main():
    """Main function to build multi-version documentation."""
    root_dir = Path.cwd()
    output_dir = root_dir / "multiversion_docs"

    print("--- 1. Cleaning up old documentation ---")
    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    print("--- 2. Getting versions from git ---")
    git_tags_result = run_command(["git", "tag", "--list", "v*"])
    # Create a list of version names, starting with 'main'.
    version_names = ["main"] + sorted(
        git_tags_result.stdout.splitlines(),
        reverse=True
    )
    # For demonstration, limit the number of versions. Remove this in production.
    version_names = version_names[:4]
    print(f"Versions to build: {', '.join(version_names)}")

    # Pre-compute all version information and paths.
    versions = [
        VersionInfo(name, name == "main", output_dir)
        for name in version_names
    ]

    with tempfile.TemporaryDirectory() as build_dir_str:
        build_dir = Path(build_dir_str)
        # --- 3. Build documentation for each version ---
        for version in versions:
            print(f"\n--- Building docs for version: {version.name} ---")

            # Create a temporary directory for this version's source code.
            version_src_dir = build_dir / f"src_{version.name}"
            version_src_dir.mkdir()

            # Check out the version's source code from git.
            archive_path = version_src_dir / "source.tar"
            run_command([
                "git", "archive", version.name,
                "--format=tar", f"--output={archive_path}"
            ])
            run_command(["tar", "-xf", str(archive_path)], cwd=version_src_dir)
            archive_path.unlink()

            # Configure and build the docs using CMake.
            version_build_dir = build_dir / f"build_{version.name}"
            version_build_dir.mkdir()
            run_command([
                "cmake", str(version_src_dir), "-DFTXUI_BUILD_DOCS=ON"
            ], cwd=version_build_dir)
            run_command(["make", "doc"], cwd=version_build_dir)

            # Copy the generated HTML files to the final destination.
            doxygen_html_dir = version_build_dir / "doc" / "doxygen" / "html"
            if not doxygen_html_dir.is_dir():
                print(f"FATAL: Doxygen HTML output not found for version {version.name}")
                exit(1)

            print(f"Copying files to: {version.dest_dir}")
            shutil.copytree(doxygen_html_dir, version.dest_dir, dirs_exist_ok=True)

    # --- 4. Inject version switcher into all HTML files ---
    print("\n--- Injecting version switcher JavaScript ---")
    for version in versions:
        if not version.dest_dir.exists():
            print(f"Warning: Destination directory for {version.name} does not exist. Skipping JS injection.")
            continue
        
        print(f"Processing HTML files in: {version.dest_dir}")
        for html_file in version.dest_dir.rglob("*.html"):
            js_script = get_version_switcher_js(version, versions, html_file)
            script_tag = f'<script>{js_script}</script>'

            content = html_file.read_text(encoding='utf-8')
            # Inject the script right before the closing body tag.
            new_content = content.replace("</body>", f"{script_tag}\n</body>")
            html_file.write_text(new_content, encoding='utf-8')

    print("\n--- 5. Finalizing ---")
    print("Multi-version documentation generated successfully!")
    print(f"Output located in: {output_dir.resolve()}")

if __name__ == "__main__":
    main()
