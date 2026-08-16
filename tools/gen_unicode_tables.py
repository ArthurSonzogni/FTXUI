#!/usr/bin/env python3
"""Regenerate the Unicode tables used by src/ftxui/screen/string.cpp.

    python3 tools/gen_unicode_tables.py

Reads the latest released Unicode Character Database and writes
src/ftxui/screen/string_unicode_tables.ipp. The release the tables were built
from is recorded in the generated file's header.

The script takes no options and is idempotent: until Unicode publishes a new
release, re-running it rewrites the same bytes, so `git diff --exit-code` after
a run tells whether the committed tables are up to date.
"""

import re
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "src/ftxui/screen/string_unicode_tables.ipp"
ENUM = ROOT / "src/ftxui/screen/string_internal.hpp"

UCD = "https://www.unicode.org/Public/UCD/latest/ucd/"
EAST_ASIAN_WIDTH = "EastAsianWidth.txt"
WORD_BREAK_PROPERTY = "auxiliary/WordBreakProperty.txt"

PROLOGUE = """\
// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
//
// Generated from Unicode {version} by tools/gen_unicode_tables.py.
// Do not edit: re-run that script instead.
//
// https://www.unicode.org/Public/{version}/ucd/{width}
// https://www.unicode.org/Public/{version}/ucd/{word_break}
//
// Included by string.cpp from inside its anonymous namespace: Interval,
// WordBreakPropertyInterval and WBP are expected to be declared already.

// clang-format off"""


def fetch(path):
    with urllib.request.urlopen(UCD + path, timeout=60) as response:
        return response.read().decode("utf-8")


def version(text):
    """The release a UCD file belongs to, from its `# Name-15.1.0.txt` header."""
    return re.match(r"#\s*\S+?-([\d.]+)\.txt", text).group(1)


def parse(text):
    """(first, last, value) for every code point range a UCD file assigns."""
    rows = []
    for line in text.splitlines():
        line = line.split("#", 1)[0].strip()
        if ";" not in line:
            continue
        field, value = (part.strip() for part in line.split(";")[:2])
        first, _, last = field.partition("..")
        rows.append((int(first, 16), int(last or first, 16), value))
    return sorted(rows)


def merge(rows):
    """Coalesce ranges that share a value and touch or overlap."""
    merged = []
    for first, last, value in rows:
        if merged and value == merged[-1][2] and first <= merged[-1][1] + 1:
            merged[-1][1] = max(merged[-1][1], last)
        else:
            merged.append([first, last, value])
    return merged


def full_width_intervals(text):
    """Wide and Fullwidth both take two cells, so they are merged together.

    EastAsianWidth.txt documents that unassigned code points in the CJK blocks
    and in planes 2 and 3 default to Wide. Reading only the explicit lines is
    still enough, because the file also spells those reserved ranges out.
    """
    rows = parse(text)
    return merge([(a, b, "W") for a, b, width in rows if width in ("W", "F")])


def word_break_intervals(text):
    rows = parse(text)
    # The enum is the source of truth for the names, so the two cannot drift.
    # A value Unicode adds later must be handled rather than quietly dropped.
    body = re.search(r"enum class WordBreakProperty[^{]*\{(.*?)\}",
                     ENUM.read_text(encoding="utf-8"), re.DOTALL)
    known = set(re.findall(r"\w+", body.group(1)))
    unknown = sorted({value for _, _, value in rows} - known)
    if unknown:
        sys.exit(f"Word_Break values missing from {ENUM.name}: "
                 f"{', '.join(unknown)}")
    return merge(rows)


def emit_full_width(rows):
    yield f"constexpr std::array<Interval, {len(rows)}> g_full_width_characters = {{{{"
    for i in range(0, len(rows), 3):
        yield "    " + " ".join(f"{{0x{a:05x}, 0x{b:05x}}}," for a, b, _ in rows[i:i + 3])
    yield "}};"


def emit_word_break(rows):
    # Wrapped so the declaration stays inside 80 columns however wide the count
    # grows.
    yield f"constexpr std::array<WordBreakPropertyInterval, {len(rows)}>"
    yield "    g_word_break_intervals = {{"
    for first, last, property in rows:
        yield f"        {{0x{first:05X}, 0x{last:05X}, WBP::{property}}},"
    yield "}};"


def main():
    width_text = fetch(EAST_ASIAN_WIDTH)
    width = full_width_intervals(width_text)
    word_break = word_break_intervals(fetch(WORD_BREAK_PROPERTY))

    lines = [PROLOGUE.format(version=version(width_text),
                             width=EAST_ASIAN_WIDTH,
                             word_break=WORD_BREAK_PROPERTY), ""]
    lines += emit_full_width(width)
    lines += [""]
    lines += emit_word_break(word_break)
    lines += ["", "// clang-format on"]
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    print(f"{OUTPUT.relative_to(ROOT)}: Unicode {version(width_text)}, "
          f"{len(width)} full-width and {len(word_break)} word-break intervals")


if __name__ == "__main__":
    main()
