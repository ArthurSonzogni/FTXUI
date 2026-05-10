#!/usr/bin/env python3
"""
FTXUI Amalgamator
Generates a single-header (ftxui.hpp) and single-source (ftxui.cpp) version 
of the library, as well as a truly single-file header-only version.
"""
import os
import re
import subprocess
from pathlib import Path

# Configuration
ROOT = Path(__file__).parent.parent
SEARCH_PATHS = [ROOT / "include", ROOT / "src"]
SRC_DIR = ROOT / "src"

# Regex (Stricter: Must be at start of line)
FTXUI_INCLUDE_RE = re.compile(r'^\s*#include\s+[<"]ftxui/(.*)[>"]', re.MULTILINE)
SYSTEM_INCLUDE_RE = re.compile(r'^\s*#include\s+<([^>]+)>', re.MULTILINE)
GUARD_IFNDEF_RE = re.compile(r'^\s*#\s*ifndef\s+(FTXUI_\w+)\s*$', re.MULTILINE)
GUARD_DEFINE_RE = re.compile(r'^\s*#\s*define\s+(FTXUI_\w+)\s*$', re.MULTILINE)
GUARD_ENDIF_RE  = re.compile(r'^\s*#\s*endif\b', re.MULTILINE)

def get_version():
    """Extract version from CMakeLists.txt."""
    try:
        content = (ROOT / "CMakeLists.txt").read_text()
        m = re.search(r'project\(ftxui.*?VERSION\s+([\d\.]+)', content, re.DOTALL)
        return m.group(1) if m else "unknown"
    except:
        return "unknown"

def get_git_hash():
    """Get the current git hash."""
    try:
        return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], 
                                     cwd=ROOT).decode().strip()
    except:
        return "unknown"

VERSION = get_version()
GIT_HASH = get_git_hash()

# --- Templates ---

LICENSE = """// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
"""

VERSION_INFO = f"""//
// FTXUI Amalgamated Version {VERSION} ({GIT_HASH})
"""

USAGE_SPLIT = """//
// Usage (Split version):
// 1. Copy ftxui.hpp and ftxui.cpp into your project.
// 2. Add ftxui.cpp to your source files.
// 3. Ensure you are using C++17 and link against threads (-lpthread).
// 4. On Windows/MSVC, use /utf-8 and define UNICODE / _UNICODE and NOMINMAX.
"""

USAGE_HEADER_ONLY = """//
// Usage (Header-only version):
// 1. Copy ftxui_all.hpp into your project.
// 2. In EXACTLY ONE C++ file, define FTXUI_IMPLEMENTATION before including:
//    #define FTXUI_IMPLEMENTATION
//    #include "ftxui_all.hpp"
// 3. In other files, just #include "ftxui_all.hpp".
// 4. Ensure you are using C++17 and link against threads (-lpthread).
// 5. On Windows/MSVC, use /utf-8 and define UNICODE / _UNICODE and NOMINMAX.
"""

WINDOWS_CHECK = """
// On Windows, FTXUI requires UNICODE and NOMINMAX to be defined.
#if defined(_WIN32)
  #if !defined(UNICODE)
    #error "FTXUI requires UNICODE to be defined on Windows. Please add /DUNICODE /D_UNICODE to your compiler flags."
  #endif
  #if !defined(NOMINMAX)
    #error "FTXUI requires NOMINMAX to be defined on Windows. Please add /DNOMINMAX to your compiler flags."
  #endif
#endif
"""

AMALGAMATED_MARKER = """
// Marker for detection
#define FTXUI_AMALGAMATED
"""

# --- Implementation ---

class Amalgamator:
    def __init__(self):
        self.included_headers = set()
        # header -> set of conditions it was found under. 
        # An empty string in the set means it was found unguarded at least once.
        self.system_includes = {} 

    def _add_system_include(self, header, condition):
        if header not in self.system_includes:
            self.system_includes[header] = set()
        self.system_includes[header].add(condition or "")

    def strip_license(self, lines):
        """Removes FTXUI license block if found in the first 20 lines."""
        copyright_idx = -1
        for i in range(min(len(lines), 20)):
            line = lines[i].strip()
            if line.startswith("//") and "Copyright" in line and "Arthur Sonzogni" in line:
                copyright_idx = i
                break
        
        if copyright_idx == -1:
            return lines

        start_idx = copyright_idx
        while start_idx > 0 and lines[start_idx - 1].strip().startswith("//"):
            start_idx -= 1
        
        end_idx = copyright_idx
        while end_idx < len(lines) - 1 and lines[end_idx + 1].strip().startswith("//"):
            end_idx += 1
        
        return lines[:start_idx] + lines[end_idx + 1:]

    def strip_include_guard(self, lines):
        """Removes the outermost include guard from a header file."""
        content_lines = [l for l in lines if l.strip() and not l.strip().startswith("//")]
        if not content_lines:
            return lines

        m_ifndef = GUARD_IFNDEF_RE.match(content_lines[0].strip())
        if not m_ifndef:
            return lines
        
        guard_symbol = m_ifndef.group(1)

        start_ifndef = -1
        for i, line in enumerate(lines):
            if GUARD_IFNDEF_RE.match(line.strip()):
                start_ifndef = i
                break
        
        start_define = -1
        for i in range(start_ifndef + 1, len(lines)):
            m = GUARD_DEFINE_RE.match(lines[i].strip())
            if m and m.group(1) == guard_symbol:
                start_define = i
                break
        
        last_endif = -1
        for i in range(len(lines) - 1, start_define, -1):
            if GUARD_ENDIF_RE.match(lines[i].strip()):
                last_endif = i
                break
        
        if start_define != -1 and last_endif != -1:
            return lines[start_define + 1 : last_endif]
        
        return lines

    def resolve_header(self, rel_path):
        """Recursively inlines FTXUI headers."""
        if rel_path in self.included_headers:
            return ""
        self.included_headers.add(rel_path)

        path = None
        for p in SEARCH_PATHS:
            if (p / rel_path).exists():
                path = p / rel_path
                break
        
        if not path:
            raise FileNotFoundError(f"Could not find FTXUI include: {rel_path}")

        lines = path.read_text(encoding='utf-8').splitlines(keepends=True)
        lines = self.strip_license(lines)
        lines = self.strip_include_guard(lines)

        content = []
        condition_stack = [] # Stack of (original_if_cond, current_branch_cond)
        for line in lines:
            stripped = line.strip()
            if stripped.startswith("#if"):
                condition_stack.append([stripped, stripped])
            elif stripped.startswith("#elif") and condition_stack:
                condition_stack[-1][1] = stripped.replace("#elif", "#if")
            elif stripped.startswith("#else") and condition_stack:
                root_cond = condition_stack[-1][0]
                if "!" in root_cond:
                    negated = root_cond.replace("!", "")
                elif "defined(" in root_cond:
                    negated = root_cond.replace("if ", "if !")
                else:
                    negated = root_cond.replace("#if ", "#if !")
                condition_stack[-1][1] = negated
            elif stripped.startswith("#endif") and condition_stack:
                condition_stack.pop()

            m = FTXUI_INCLUDE_RE.match(line)
            if m:
                inc = "ftxui/" + m.group(1)
                content.append(self.resolve_header(inc))
                continue

            m = SYSTEM_INCLUDE_RE.match(line)
            if m:
                cond = condition_stack[-1][1] if condition_stack else None
                self._add_system_include(m.group(1), cond)
                continue

            content.append(line)
        
        return "".join(content)

    def write_amalgamated(self, header_raw, source_content):
        """Generates all final amalgamated files."""
        # 1. Build Split Header Parts
        parts = [
            LICENSE,
            VERSION_INFO,
            USAGE_SPLIT,
            "\n#ifndef FTXUI_AMALGAMATED_HPP\n#define FTXUI_AMALGAMATED_HPP\n",
            WINDOWS_CHECK,
            AMALGAMATED_MARKER
        ]

        standard = []
        guarded = {}
        for header, conditions in self.system_includes.items():
            if "" in conditions:
                standard.append(header)
            else:
                cond = sorted(list(conditions))[0]
                if cond not in guarded: guarded[cond] = []
                guarded[cond].append(header)

        for inc in sorted(standard): parts.append(f"#include <{inc}>\n")
        for cond in sorted(guarded.keys()):
            parts.append(f"\n{cond}\n")
            for inc in sorted(guarded[cond]): parts.append(f"#include <{inc}>\n")
            parts.append("#endif\n")
        
        parts.append("\n" + header_raw.strip() + "\n")
        parts.append("\n#endif // FTXUI_AMALGAMATED_HPP\n")
        header_content = "".join(parts)

        # 2. Write ftxui.hpp
        print("Writing ftxui.hpp...")
        with open("ftxui.hpp", "w", encoding='utf-8') as f:
            f.write(header_content)
        
        # 3. Write ftxui.cpp
        print("Writing ftxui.cpp...")
        with open("ftxui.cpp", "w", encoding='utf-8') as f:
            f.write(LICENSE)
            f.write('#include "ftxui.hpp"\n')
            f.write(source_content)

        # 4. Write ftxui_all.hpp
        print("Writing ftxui_all.hpp (Header-only version)...")
        with open("ftxui_all.hpp", "w", encoding='utf-8') as f:
            header_only = header_content.replace(USAGE_SPLIT, USAGE_HEADER_ONLY)
            header_stripped = header_only.rsplit("#endif // FTXUI_AMALGAMATED_HPP", 1)[0]
            f.write(header_stripped)
            f.write("\n#ifdef FTXUI_IMPLEMENTATION\n")
            f.write(source_content)
            f.write("\n#endif // FTXUI_IMPLEMENTATION\n")
            f.write("\n#endif // FTXUI_AMALGAMATED_HPP\n")

    def run(self):
        print("Inlining headers...")
        header_raw = self.resolve_header("ftxui/ftxui.hpp")
        
        print("Processing source files...")
        cpp_files = []
        for root, _, files in os.walk(SRC_DIR):
            for file in files:
                if file.endswith(".cpp") and not any(x in file for x in ["_test.cpp", "_fuzzer.cpp", "loop.cpp"]):
                    cpp_files.append(Path(root) / file)
        
        source_blocks = []
        for path in sorted(cpp_files):
            rel_path = path.relative_to(SRC_DIR / "ftxui")
            lines = path.read_text(encoding='utf-8').splitlines(keepends=True)
            lines = self.strip_license(lines)
            
            block = [f"\n// ---- {rel_path} " + "-" * max(0, 60 - len(str(rel_path))) + "\n"]
            condition_stack = []
            for line in lines:
                stripped = line.strip()
                if stripped.startswith("#if"):
                    condition_stack.append([stripped, stripped])
                elif stripped.startswith("#elif") and condition_stack:
                    condition_stack[-1][1] = stripped.replace("#elif", "#if")
                elif stripped.startswith("#else") and condition_stack:
                    root_cond = condition_stack[-1][0]
                    if "!" in root_cond: negated = root_cond.replace("!", "")
                    elif "defined(" in root_cond: negated = root_cond.replace("if ", "if !")
                    else: negated = root_cond.replace("#if ", "#if !")
                    condition_stack[-1][1] = negated
                elif stripped.startswith("#endif") and condition_stack:
                    condition_stack.pop()

                m = FTXUI_INCLUDE_RE.match(line)
                if m:
                    inc = "ftxui/" + m.group(1)
                    if not (ROOT / "include" / inc).exists():
                        block.append(self.resolve_header(inc))
                    continue
                
                m = SYSTEM_INCLUDE_RE.match(line)
                if m:
                    cond = condition_stack[-1][1] if condition_stack else None
                    self._add_system_include(m.group(1), cond)
                    continue
                
                block.append(line)
            source_blocks.append("".join(block))

        self.write_amalgamated(header_raw, "".join(source_blocks))

if __name__ == "__main__":
    try:
        Amalgamator().run()
        print(f"Success! Generated version {VERSION} ({GIT_HASH})")
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
