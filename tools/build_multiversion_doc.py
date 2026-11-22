#!/usr/bin/env python3
"""Build multi-version, multi-language VitePress documentation for FTXUI."""

import argparse
import json
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path
from typing import Dict, List, Optional, Tuple


SITE_PREFIX = "/FTXUI"
SITE_ORIGIN = "https://arthursonzogni.github.io"


def version_sort_key(tag: str) -> Tuple[int, ...]:
    """Parse a tag like 'v7.0.3' or 'v0.11' into a numeric tuple for sorting."""
    nums = [int(x) for x in re.findall(r"\d+", tag)]
    return tuple(nums) if nums else (0,)


def run_command(
    command: List[str],
    check: bool = True,
    cwd: Optional[Path] = None,
    env: Optional[Dict[str, str]] = None,
) -> subprocess.CompletedProcess:
    command_str = " ".join(command)
    print(f"Executing: {command_str} in {cwd or Path.cwd()}")
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=check,
            cwd=cwd,
            env=env,
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
        raise


def generate_doxygen_xml(src_dir: Path, xml_out_dir: Path) -> Path:
    """Run Doxygen directly to emit XML for include/, src/, and examples/."""
    xml_out_dir.mkdir(parents=True, exist_ok=True)
    # Provide @defgroup and @example definitions so all historical versions
    # (v0.9..main) emit group__screen, group__dom, group__component, and
    # all example compounds consistently without running CMake.
    groups_lines = [
        "/// @defgroup screen Screen",
        "/// @defgroup dom DOM",
        "/// @defgroup component Component",
    ]
    example_dir = src_dir / "examples"
    if example_dir.is_dir():
        for cat in ("dom", "component"):
            cat_dir = example_dir / cat
            if cat_dir.is_dir():
                for cpp in sorted(cat_dir.glob("*.cpp")):
                    groups_lines.append(f"/// @example examples/{cat}/{cpp.name}")
    groups_dox = xml_out_dir / "ftxui_groups.dox"
    groups_dox.write_text("\n".join(groups_lines) + "\n", encoding="utf-8")
    input_dirs = [str(groups_dox)] + [
        str(src_dir / d) for d in ("include", "src") if (src_dir / d).is_dir()
    ]

    doxyfile = xml_out_dir / "Doxyfile"
    doxyfile.write_text(
        "\n".join(
            [
                "PROJECT_NAME           = FTXUI",
                f"OUTPUT_DIRECTORY       = {xml_out_dir}",
                "FULL_PATH_NAMES        = YES",
                f"STRIP_FROM_PATH        = {src_dir}",
                "GENERATE_HTML          = NO",
                "GENERATE_LATEX         = NO",
                "GENERATE_XML           = YES",
                "XML_OUTPUT             = xml",
                "RECURSIVE              = YES",
                "EXTRACT_ALL            = YES",
                "QUIET                  = YES",
                "WARNINGS               = NO",
                "WARN_IF_UNDOCUMENTED   = NO",
                "ENABLE_PREPROCESSING   = YES",
                "MACRO_EXPANSION        = YES",
                "EXPAND_ONLY_PREDEF     = YES",
                'PREDEFINED             = "FTXUI_EXPORT(x)="',
                f"INPUT                  = {' '.join(input_dirs)}",
                f"EXAMPLE_PATH           = {src_dir}" if example_dir.is_dir() else "",
                "EXAMPLE_RECURSIVE      = YES",
                "EXCLUDE_PATTERNS       = *_test.cpp *_fuzzer.cpp",
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    run_command(["doxygen", str(doxyfile)], cwd=src_dir)
    return xml_out_dir / "xml"


def build_version(
    version: str,
    root_dir: Path,
    work_dir: Path,
    shared_vp_dir: Path,
    converter_script: Path,
    output_dir: Path,
    version_items: List[Dict[str, str]],
) -> None:
    is_main = version == "main"
    print(f"\n=== Building VitePress docs for {version} ===")

    # 1. Extract version source from git (or use working tree for main if requested)
    src_dir = work_dir / f"src_{version}"
    src_dir.mkdir(parents=True, exist_ok=True)
    archive_path = work_dir / f"{version}.tar"
    git_ref = "HEAD" if is_main else version
    run_command(
        ["git", "archive", git_ref, "--format=tar", f"--output={archive_path}"],
        cwd=root_dir,
    )
    run_command(["tar", "-xf", str(archive_path)], cwd=src_dir)
    archive_path.unlink()

    # 2. Generate Doxygen XML for this version's headers/sources/examples
    xml_work = work_dir / f"doxygen_{version}"
    xml_dir = generate_doxygen_xml(src_dir, xml_work)

    # 3. Clean generated VitePress artifacts from any previous version in shared_vp_dir
    for sub in ("api", "examples", ".vitepress/dist", ".vitepress/cache"):
        p = shared_vp_dir / sub
        if p.exists():
            shutil.rmtree(p)

    # 4. Run Doxygen XML -> VitePress converter using this version's source tree as cwd
    cmd = [
        "python3",
        str(converter_script),
        str(xml_dir),
        str(shared_vp_dir),
        "--no-guide-autolink",
    ]
    run_command(cmd, cwd=src_dir)

    # 5. Build VitePress site
    vp_base = f"{SITE_PREFIX}/" if is_main else f"{SITE_PREFIX}/en/{version}/"
    vp_env = os.environ.copy()
    vp_env["VITEPRESS_BASE"] = vp_base
    vp_env["VITEPRESS_VERSION"] = version
    vp_env["VITEPRESS_VERSIONS"] = json.dumps(version_items)
    if not is_main:
        vp_env["VITEPRESS_SINGLE_LOCALE"] = "1"
        vp_env["VITEPRESS_IGNORE_DEAD_LINKS"] = "1"
    else:
        vp_env.pop("VITEPRESS_SINGLE_LOCALE", None)
        vp_env.pop("VITEPRESS_IGNORE_DEAD_LINKS", None)

    run_command(["npm", "run", "docs:build"], cwd=shared_vp_dir, env=vp_env)

    # 6. Copy built dist to destination
    dist_dir = shared_vp_dir / ".vitepress" / "dist"
    dest_dir = output_dir if is_main else (output_dir / "en" / version)
    dest_dir.mkdir(parents=True, exist_ok=True)
    shutil.copytree(dist_dir, dest_dir, dirs_exist_ok=True)


def write_legacy_redirects(output_dir: Path) -> None:
    """Keep legacy /zh-CH/ URLs working by redirecting to /zh-CN/."""
    zh_cn = output_dir / "zh-CN"
    if not zh_cn.is_dir():
        return
    zh_ch = output_dir / "zh-CH"
    zh_ch.mkdir(parents=True, exist_ok=True)
    (zh_ch / "index.html").write_text(
        '<!DOCTYPE html><html><head><meta charset="utf-8">'
        '<meta http-equiv="refresh" content="0; url=../zh-CN/">'
        '<link rel="canonical" href="../zh-CN/"></head>'
        '<body><a href="../zh-CN/">Redirecting to Simplified Chinese docs...</a></body></html>\n',
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser(description="Build multi-version FTXUI VitePress docs.")
    parser.add_argument(
        "--versions",
        help="Comma-separated list of versions to build (default: main + all v* tags).",
        default="",
    )
    parser.add_argument(
        "--output",
        help="Output directory (default: multiversion_docs).",
        default="multiversion_docs",
    )
    args = parser.parse_args()

    root_dir = Path.cwd()
    output_dir = root_dir / args.output
    converter_script = root_dir / "tools" / "doxygen_xml_to_vitepress.py"
    source_vp_dir = root_dir / "docs_vitepress"

    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Determine all versions (semantic version descending)
    tags_result = run_command(["git", "tag", "--list", "v*"], cwd=root_dir)
    all_tags = sorted(
        [t.strip() for t in tags_result.stdout.splitlines() if t.strip()],
        key=version_sort_key,
        reverse=True,
    )
    all_versions = ["main"] + all_tags

    if args.versions:
        requested = [v.strip() for v in args.versions.split(",") if v.strip()]
        versions_to_build = requested
    else:
        versions_to_build = all_versions

    print(f"Versions to build ({len(versions_to_build)}): {', '.join(versions_to_build)}")

    # Build version dropdown entries (always list all versions built)
    version_items: List[Dict[str, str]] = []
    for v in versions_to_build:
        if v == "main":
            version_items.append(
                {
                    "text": "main (latest)",
                    "link": f"{SITE_ORIGIN}{SITE_PREFIX}/",
                    "target": "_self",
                }
            )
        else:
            version_items.append(
                {
                    "text": v,
                    "link": f"{SITE_ORIGIN}{SITE_PREFIX}/en/{v}/",
                    "target": "_self",
                }
            )

    with tempfile.TemporaryDirectory() as work_str:
        work_dir = Path(work_str)
        shared_vp_dir = work_dir / "docs_vitepress"

        # Copy VitePress source (excluding generated artifacts & node_modules)
        shutil.copytree(
            source_vp_dir,
            shared_vp_dir,
            ignore=shutil.ignore_patterns("node_modules", "dist", "cache", "api", "examples"),
        )

        # Reuse local node_modules if present (fast local runs), else npm ci
        local_nm = source_vp_dir / "node_modules"
        if local_nm.is_dir():
            os.symlink(local_nm, shared_vp_dir / "node_modules")
        else:
            install_cmd = (
                ["npm", "ci"]
                if (shared_vp_dir / "package-lock.json").is_file()
                else ["npm", "install"]
            )
            run_command(install_cmd, cwd=shared_vp_dir)

        for version in versions_to_build:
            build_version(
                version=version,
                root_dir=root_dir,
                work_dir=work_dir,
                shared_vp_dir=shared_vp_dir,
                converter_script=converter_script,
                output_dir=output_dir,
                version_items=version_items,
            )

    write_legacy_redirects(output_dir)
    print(f"\nMulti-version VitePress docs built successfully in {output_dir.resolve()}")


if __name__ == "__main__":
    main()
