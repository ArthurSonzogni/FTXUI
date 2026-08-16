#!/usr/bin/env python3
"""Regenerate the Unicode tables used by src/ftxui/screen/string.cpp.

    python3 tools/gen_unicode_tables.py

Writes src/ftxui/screen/string_unicode_tables.ipp from the Unicode Character
Database. To adopt a newer Unicode release, bump UNICODE_VERSION and re-run.

The version is pinned rather than "latest": "latest" serves a different release
every September, so a regenerated table would quietly mean something different
each time, and nothing would record which version a table represents. That is
how a table documented as Unicode 13.0.0 came to be four releases behind.

Re-running without bumping the version rewrites the same bytes, so
`git diff --exit-code` after a run says whether the committed table really is
the version it claims to be.
"""

import re
import sys
import urllib.request
from pathlib import Path

UNICODE_VERSION = "17.0.0"

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "src/ftxui/screen/string_unicode_tables.ipp"
ENUM = ROOT / "src/ftxui/screen/string_internal.hpp"

UCD = f"https://www.unicode.org/Public/{UNICODE_VERSION}/ucd/"
EAST_ASIAN_WIDTH = UCD + "EastAsianWidth.txt"
WORD_BREAK_PROPERTY = UCD + "auxiliary/WordBreakProperty.txt"

PROLOGUE = """\
// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
//
// Generated from Unicode {version} by tools/gen_unicode_tables.py.
// Do not edit: bump UNICODE_VERSION in that script and re-run it.
//
// {width_url}
// {word_break_url}
//
// Included by string.cpp from inside its anonymous namespace: Interval,
// WordBreakPropertyInterval and WBP are expected to be declared already.

// clang-format off"""


def fetch(url):
    with urllib.request.urlopen(url, timeout=60) as response:
        # An unreleased version redirects to the draft data, which is still
        # changing: the table would carry a version whose content it does not
        # have.
        if "/draft/" in response.url:
            sys.exit(f"Unicode {UNICODE_VERSION} is not released yet:\n"
                     f"  {url}\n  -> {response.url}")
        return response.read().decode("utf-8")


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


def full_width_intervals():
    """Wide and Fullwidth both take two cells, so they are merged together.

    EastAsianWidth.txt documents that unassigned code points in the CJK blocks
    and in planes 2 and 3 default to Wide. Reading only the explicit lines is
    still enough, because the file also spells those reserved ranges out.
    """
    rows = parse(fetch(EAST_ASIAN_WIDTH))
    return merge([(a, b, "W") for a, b, width in rows if width in ("W", "F")])


def word_break_intervals():
    rows = parse(fetch(WORD_BREAK_PROPERTY))
    # The enum is the source of truth for the names, so the two cannot drift.
    # A value Unicode adds later must be handled rather than quietly dropped.
    body = re.search(r"enum class WordBreakProperty[^{]*\{(.*?)\}",
                     ENUM.read_text(encoding="utf-8"), re.DOTALL)
    known = set(re.findall(r"\w+", body.group(1)))
    unknown = sorted({value for _, _, value in rows} - known)
    if unknown:
        sys.exit(f"Unicode {UNICODE_VERSION} has Word_Break values missing from "
                 f"{ENUM.name}: {', '.join(unknown)}")
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
    width = full_width_intervals()
    word_break = word_break_intervals()

    lines = [PROLOGUE.format(version=UNICODE_VERSION,
                             width_url=EAST_ASIAN_WIDTH,
                             word_break_url=WORD_BREAK_PROPERTY), ""]
    lines += emit_full_width(width)
    lines += [""]
    lines += emit_word_break(word_break)
    lines += ["", "// clang-format on"]
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    print(f"{OUTPUT.relative_to(ROOT)}: Unicode {UNICODE_VERSION}, "
          f"{len(width)} full-width and {len(word_break)} word-break intervals")


if __name__ == "__main__":
    main()
