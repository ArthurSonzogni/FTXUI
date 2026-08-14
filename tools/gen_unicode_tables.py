#!/usr/bin/env python3
"""Generate ftxui's Unicode tables for a given Unicode version.

./gen_unicode_tables.py 17.0.0                # emit both tables
./gen_unicode_tables.py 13.0.0 --check FILE   # compare against the tables already in FILE

"""

import argparse, sys, os, urllib.error, urllib.request, re

BASE = "https://www.unicode.org/Public/{v}/ucd/"
WBP_NAMES = {
    "ALetter",
    "CR",
    "Double_Quote",
    "Extend",
    "ExtendNumLet",
    "Format",
    "Hebrew_Letter",
    "Katakana",
    "LF",
    "MidLetter",
    "MidNum",
    "MidNumLet",
    "Newline",
    "Numeric",
    "Regional_Indicator",
    "Single_Quote",
    "WSegSpace",
    "ZWJ",
}


def fetch(version, path):
    cache = f"cache-{version}-{os.path.basename(path)}"
    if not os.path.exists(cache):
        url = BASE.format(v=version) + path
        try:
            with urllib.request.urlopen(url) as resp:
                # An unreleased version redirects to the draft data, which is still changing: the
                # result would be a table labelled with a version whose contents it does not have.
                if "/draft/" in resp.url:
                    print(
                        f"WARNING: Unicode {version} is not released yet.\n"
                        f"         {url}\n"
                        f"      -> {resp.url}\n"
                        f"         These are DRAFT tables and will change before release.",
                        file=sys.stderr,
                    )
                data = resp.read()
        except urllib.error.HTTPError as e:
            # Almost always a version that does not exist.
            sys.exit(f"{url}\n  {e}\n(is {version} a real Unicode version?)")
        except urllib.error.URLError as e:
            sys.exit(f"could not fetch {url}: {e.reason}")
        # Written whole, so an interrupted fetch cannot leave a truncated cache behind.
        with open(cache, "wb") as f:
            f.write(data)
    return open(cache, encoding="utf-8")


def parse(fh, keep):
    """(first, last, property) for lines whose property is wanted."""
    out = []
    for line in fh:
        line = line.split("#", 1)[0].strip()
        if not line or ";" not in line:
            continue
        field, prop = (p.strip() for p in line.split(";")[:2])
        if prop not in keep:
            continue
        a, _, b = field.partition("..")
        out.append((int(a, 16), int(b or a, 16), prop))
    return sorted(out)


def merge(rows, by_property):
    """Coalesce touching ranges; only ranges of the same property merge when it matters."""
    out = []
    for a, b, p in rows:
        if out and a <= out[-1][1] + 1 and (not by_property or p == out[-1][2]):
            out[-1][1] = max(out[-1][1], b)
        else:
            out.append([a, b, p])
    return out


def widths(version):
    return merge(
        parse(fetch(version, "EastAsianWidth.txt"), {"W", "F"}), by_property=False
    )


def wordbreaks(version):
    return merge(
        parse(fetch(version, "auxiliary/WordBreakProperty.txt"), WBP_NAMES),
        by_property=True,
    )


def emit_widths(rows):
    # Five digits zero padded, matching the hand-written table this replaced, so the columns align.
    yield "constexpr std::array<Interval, %d> g_full_width_characters = {{" % len(rows)
    for i in range(0, len(rows), 3):
        yield "    " + " ".join(f"{{0x{a:05x}, 0x{b:05x}}}," for a, b, _ in rows[i : i + 3])
    yield "}};"


def emit_wbp(rows):
    yield "constexpr std::array<WordBreakPropertyInterval, %d> g_word_break_intervals = {{" % len(
        rows
    )
    for a, b, p in rows:
        yield f"    {{0x{a:05X}, 0x{b:05X}, WBP::{p}}},"
    yield "}};"


def existing(path):
    src = open(path, encoding="utf-8").read()
    w = [
        (int(a, 16), int(b, 16), None)
        for a, b in re.findall(
            r"\{0x([0-9a-fA-F]+), 0x([0-9a-fA-F]+)\},",
            src[
                src.index("g_full_width_characters") : src.index(
                    "struct WordBreakPropertyInterval"
                )
            ],
        )
    ]
    seg = src[src.index("g_word_break_intervals") :]
    seg = seg[: seg.index("}};")]
    wb = [
        (int(a, 16), int(b, 16), p)
        for a, b, p in re.findall(
            r"\{0x([0-9a-fA-F]+), 0x([0-9a-fA-F]+), WBP::(\w+)\}", seg
        )
    ]
    return sorted(w), sorted(wb)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        epilog="The version is explicit rather than 'latest' because 'latest' serves a different "
        "release every September: a regenerated table would quietly mean something different each "
        "time, and nothing would record which version a given table represents.",
    )
    ap.add_argument("version", help="Unicode version to generate from, e.g. 17.0.0")
    ap.add_argument(
        "--check",
        metavar="FILE",
        help="compare against the tables already in FILE instead of emitting them; "
        "exits non-zero if they differ",
    )
    args = ap.parse_args()

    # The UCD is published under a three-part version; "13" and "15.1" are how people say them.
    version = args.version
    if not re.fullmatch(r"\d+(\.\d+){0,2}", version):
        ap.error(f"'{version}' is not a Unicode version (expected something like 17.0.0)")
    version += ".0" * (2 - version.count("."))

    w = [tuple(x) for x in widths(version)]
    wb = [tuple(x) for x in wordbreaks(version)]

    if args.check:
        path = args.check

        curr_w, curr_wb = existing(path)
        mine_w = sorted((a, b, None) for a, b, _ in w)
        ok_w = mine_w == curr_w
        ok_wb = sorted(wb) == curr_wb
        print(f"Unicode {version} vs {os.path.basename(path)}")
        print(
            f"  full-width : generated {len(mine_w):>5}, in source {len(curr_w):>5}  {'MATCH' if ok_w else 'DIFFER'}"
        )
        print(
            f"  word-break : generated {len(wb):>5}, in source {len(curr_wb):>5}  {'MATCH' if ok_wb else 'DIFFER'}"
        )
        for label, mine, theirs in (
            ("full-width", mine_w, curr_w),
            ("word-break", sorted(wb), curr_wb),
        ):
            if mine != theirs:
                mine_set, theirs_set = set(mine), set(theirs)
                only_mine = [r for r in mine if r not in theirs_set][:6]
                only_theirs = [r for r in theirs if r not in mine_set][:6]
                if only_mine:
                    print(f"    {label}: only generated  {only_mine}")
                if only_theirs:
                    print(f"    {label}: only in source  {only_theirs}")
        return 0 if (ok_w and ok_wb) else 1
    for line in emit_widths([tuple(r) for r in w]):
        print(line)
    print()
    for line in emit_wbp(wb):
        print(line)
    return 0


sys.exit(main())
