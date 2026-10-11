"""
CMake, Meson and Bazel must build the same library sources.

Each of them lists the sources separately, so a new file is easily forgotten in
one of them, and a removed one easily left behind.
"""
import os
import re
from pathlib import Path

from checks._common import is_library_source, tracked_files


def cmake_sources(root):
    text = (root / "CMakeLists.txt").read_text()
    return {Path(p) for p in re.findall(r"src/ftxui/[\w/.-]+\.cpp", text)}


def bazel_sources(root):
    text = (root / "BUILD.bazel").read_text()
    return {Path(p) for p in re.findall(r'"(src/ftxui/[\w/.-]+\.cpp)"', text)}


def meson_sources(root):
    sources = set()
    for meson in tracked_files(root, "src/ftxui/*/meson.build"):
        for name in re.findall(r"'([\w/.-]+\.cpp)'", (root / meson).read_text()):
            sources.add(Path(os.path.normpath(meson.parent / name)))
    return sources


def check(root):
    expected = set(filter(is_library_source, tracked_files(root, "src")))
    errors = []
    for build_system, sources in [
        ("CMake", cmake_sources(root)),
        ("Meson", meson_sources(root)),
        ("Bazel", bazel_sources(root)),
    ]:
        sources = set(filter(is_library_source, sources))
        for path in sorted(expected - sources):
            errors.append(f"{build_system} doesn't build {path}")
        for path in sorted(sources - expected):
            errors.append(f"{build_system} builds {path}, which doesn't exist")
    return errors
