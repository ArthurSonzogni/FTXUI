#!/usr/bin/env python3
"""
Translate the FTXUI documentation into the ftxui-translations repository.

Uses the `claude` CLI with the logged-in subscription (no API key), so it is
free but bounded by the subscription quota. To use as little of it as
possible:
- Only comment blocks (C++) and paragraphs (Markdown) are sent. Code is never
  sent nor returned; the files are rebuilt locally.
- Each language branch keeps a translation memory (English block -> translated
  block). Only blocks missing from it are translated, so an upstream change
  costs only the blocks it touches.
- Each batch is a single turn, without tools, system prompt, or MCP servers.

It is resumable: the memory is committed after each batch. When the quota is
exhausted, the run pushes what it has and exits. The next run continues.

Usage:
  tools/build_translations.py --langs fr ja zh-CN

Nightly background run (crontab -e):
  0 2 * * * cd ~/FTXUI && flock -n /tmp/ftxui-tx.lock \
      tools/build_translations.py --langs fr it zh-CN zh-TW zh-HK ja es pt de ru ko \
      >> ~/ftxui-translations.log 2>&1
"""

from __future__ import annotations

import argparse
import difflib
import filecmp
import fnmatch
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Tuple

FTXUI_REPO_URL = "git@github.com:ArthurSonzogni/FTXUI.git"
TRANSLATIONS_REPO_URL = "git@github.com:ArthurSonzogni/ftxui-translations.git"

# Files whose documentation is translated. Everything else is copied as-is.
TRANSLATE_PATTERNS = ["README.md", "doc/*.md", "include/*.hpp", "src/*.cpp",
                      "src/*.hpp", "src/*.cppm", "src/*.ipp", "examples/*.cpp"]
SKIP_PATTERNS = ["*_test.cpp", "*_fuzzer.cpp"]

# Blocks kept as-is: license headers, include-what-you-use annotations, linter
# directives, markers like `// namespace ftxui`, and lone HTML tags.
KEEP_BLOCK = re.compile(
    r"\s*(//\s*(Copyright|for\b|namespace\b|NOLINT|IWYU|clang-format|static$)"
    r"|</?[a-z][^>]*>\s*$)")

# Characters of English text per claude call.
BATCH_MAX_CHARS = 20_000

LANG_NAMES = {
    "fr": "French",
    "it": "Italian",
    "zh-CN": "Simplified Chinese",
    "zh-TW": "Traditional Chinese",
    "zh-HK": "Hong Kong Chinese",
    "ja": "Japanese",
    "es": "Spanish",
    "pt": "Portuguese",
    "de": "German",
    "ru": "Russian",
    "ko": "Korean",
}

CACHE_FILE = "translation_cache.json"

SYSTEM_PROMPT = """\
You translate the documentation of the FTXUI C++ library (a terminal UI
library) from English into {lang_name} ("{lang_code}").

The input is a JSON array of {{"id", "file", "text"}}. Each text is either a
block of C++ comments, or a Markdown paragraph. Reply with only a JSON object
mapping each id to its translated text.

RULES
- Keep the comment markers (//, ///, /*, */, *) and the line structure of each
  comment. Every line of a comment must stay a comment.
- Keep code, identifiers, Doxygen commands (@param name, @brief, \\ref, ...),
  inline `code`, URLs, anchors ({{#...}}), HTML tags, and Markdown syntax.
- Keep leading indentation.
- If a text has nothing to translate, return it unchanged.
"""

Pieces = List[Tuple[bool, str]]  # (is_block, text)


class QuotaExhausted(Exception):
    pass


def log(msg: str) -> None:
    print(msg, flush=True)


def run(cmd: List[str], cwd: Path | None = None, check: bool = True) -> str:
    proc = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    if check and proc.returncode != 0:
        raise RuntimeError(f"Command failed: {' '.join(cmd)}\n{proc.stderr}")
    return proc.stdout.strip()


# ---------------------------------------------------------------------------
# Splitting files into code and translatable blocks.
# ---------------------------------------------------------------------------

def split_cpp_comments(text: str) -> Pieces:
    """Splits C++ source into (is_comment, text) segments."""
    out: Pieces = []
    n = len(text)
    start = i = 0
    while i < n:
        c = text[i]
        if text.startswith("//", i) or text.startswith("/*", i):
            if text[i + 1] == "/":
                end = text.find("\n", i)
                end = n if end < 0 else end
            else:
                end = text.find("*/", i + 2)
                end = n if end < 0 else end + 2
            out.append((False, text[start:i]))
            out.append((True, text[i:end]))
            start = i = end
        elif c == "R" and text.startswith('"', i + 1) and \
                not (text[i - 1:i].isalnum() or text[i - 1:i] == "_"):
            m = re.match(r'R"([^()\\\s]{0,16})\(', text[i:])
            if not m:
                i += 1
                continue
            end = text.find(")" + m.group(1) + '"', i + m.end())
            i = n if end < 0 else end + len(m.group(1)) + 2
        elif c == '"' or (c == "'" and not text[i - 1:i].isdigit()):
            i += 1
            while i < n and text[i] != c and text[i] != "\n":
                i += 2 if text[i] == "\\" else 1
            i += 1
        else:
            i += 1
    out.append((False, text[start:]))
    return [p for p in out if p[1]]


def split_cpp(text: str) -> Pieces:
    """Groups comments on consecutive lines into blocks."""
    pieces: Pieces = []
    for is_comment, seg in split_cpp_comments(text):
        if is_comment and len(pieces) >= 2 and pieces[-1][1].isspace() and \
                pieces[-1][1].count("\n") == 1 and pieces[-2][0]:
            sep = pieces.pop()[1]
            pieces[-1] = (True, pieces[-1][1] + sep + seg)
        else:
            pieces.append((is_comment, seg))
    return pieces


def split_md(text: str) -> Pieces:
    """Splits Markdown into paragraphs, keeping fenced code blocks as code."""
    pieces: Pieces = []
    for i, part in enumerate(re.split(r"(^```.*?^```[^\n]*$)", text,
                                      flags=re.M | re.S)):
        if i % 2:
            pieces.append((False, part))
            continue
        for j, para in enumerate(re.split(r"(\n\s*\n)", part)):
            pieces.append((j % 2 == 0 and not para.isspace(), para))
    return [p for p in pieces if p[1]]


def split(rel: str, text: str) -> Pieces:
    pieces = split_md(text) if rel.endswith(".md") else split_cpp(text)
    # Blocks without prose are kept as-is.
    return [(b and len(re.findall(r"[A-Za-z]", t)) >= 3 and
             not KEEP_BLOCK.match(t), t) for b, t in pieces]


URL = re.compile(r"https?://[^\s)\]}>\"']+")
DOXYGEN = re.compile(r"[@\\]\w+")


def is_valid(english: str, translated: str) -> bool:
    """Rejects a translation that lost, gained, or mangled content."""
    urls = lambda x: [u.rstrip(".,;:\u3002\u3001\uff0c\uff1b\uff1a") for u in URL.findall(x)]
    if urls(english) != urls(translated):
        return False
    if DOXYGEN.findall(english) != DOXYGEN.findall(translated):
        return False
    # A translation is never that much longer or shorter than its source.
    if not 0.3 * len(english) - 40 < len(translated) < 2.2 * len(english) + 40:
        return False
    if not english.lstrip().startswith(("//", "/*")):  # Markdown
        return translated.count("```") == english.count("```")
    # Must be comments only, and closed: the sentinel must remain code.
    segs = split_cpp_comments(translated + "\n@")
    return segs[-1] == (False, "\n@") and \
        all(c or s.isspace() for c, s in segs[:-1])


def render(pieces: Pieces, memory: Dict[str, str]) -> str:
    return "".join(memory.get(t, t) if b else t for b, t in pieces)


# ---------------------------------------------------------------------------
# Git and repository helpers.
# ---------------------------------------------------------------------------

def ensure_repo(path: Path, url: str) -> None:
    if (path / ".git").is_dir():
        run(["git", "fetch", "--all", "--prune"], cwd=path)
        return
    if path.exists():
        shutil.rmtree(path)
    run(["git", "clone", url, str(path)])


def update_to_head(path: Path) -> None:
    run(["git", "checkout", "main"], cwd=path)
    run(["git", "pull", "--ff-only"], cwd=path)


def checkout_or_create_branch(repo: Path, branch: str) -> None:
    if run(["git", "branch", "--list", branch], cwd=repo):
        run(["git", "checkout", branch], cwd=repo)
        run(["git", "pull", "--ff-only"], cwd=repo, check=False)
    elif run(["git", "ls-remote", "--heads", "origin", branch], cwd=repo):
        run(["git", "checkout", "-t", f"origin/{branch}"], cwd=repo)
    else:
        run(["git", "checkout", "-b", branch], cwd=repo)


def commit(tx_dir: Path, message: str) -> None:
    if run(["git", "status", "--porcelain"], cwd=tx_dir):
        run(["git", "add", "-A"], cwd=tx_dir)
        run(["git", "commit", "-m", message], cwd=tx_dir)


def is_translatable(rel: str) -> bool:
    name = Path(rel).name
    if any(fnmatch.fnmatch(name, p) for p in SKIP_PATTERNS):
        return False
    return any(fnmatch.fnmatch(rel, p) for p in TRANSLATE_PATTERNS)


def list_blobs(repo: Path, ref: str) -> Dict[str, str]:
    """Maps each tracked file path to its blob hash at `ref`."""
    out = run(["git", "ls-tree", "-r", ref], cwd=repo)
    blobs = {}
    for line in out.splitlines():
        meta, path = line.split("\t", 1)
        blobs[path] = meta.split()[2]
    return blobs


# ---------------------------------------------------------------------------
# Translation memory.
# ---------------------------------------------------------------------------

def load_memory(tx_dir: Path, ftxui_dir: Path) -> Dict[str, str]:
    path = tx_dir / CACHE_FILE
    if not path.exists():
        return {}
    data = json.loads(path.read_text())
    if "memory" in data:
        # Drop entries that no longer pass the checks below.
        return {e: t for e, t in data["memory"].items() if is_valid(e, t)}

    # Older formats recorded which English source each file was translated
    # from. Pair its blocks with the translated file's to seed the memory.
    if "sources" in data:
        sources = data["sources"]
    elif "last_processed_ftxui_commit" in data:
        sources = list_blobs(ftxui_dir, data["last_processed_ftxui_commit"])
    else:
        return {}
    memory: Dict[str, str] = {}
    for rel, blob in sources.items():
        dst = tx_dir / rel
        if not is_translatable(rel) or not dst.exists():
            continue
        english = split(rel, run(["git", "cat-file", "blob", blob],
                                 cwd=ftxui_dir))
        translated = split(rel, dst.read_text())
        # Align on the code, ignoring whitespace and comments. Blocks between
        # identical code are translations of each other.
        keys = [[None if b else re.sub(r"\s", "", "".join(
                    s for c, s in split_cpp_comments(t) if not c))
                 for b, t in pieces] for pieces in (english, translated)]
        matcher = difflib.SequenceMatcher(None, *keys, autojunk=False)
        for tag, i1, i2, j1, _ in matcher.get_opcodes():
            if tag != "equal":
                continue
            for (b, e), (_, t) in zip(english[i1:i2], translated[j1:]):
                if b and e != t and is_valid(e, t):
                    memory[e] = t
    log(f"  imported {len(memory)} blocks from the previous translation")
    return memory


def save_memory(tx_dir: Path, memory: Dict[str, str]) -> None:
    data = {"memory": dict(sorted(memory.items()))}
    (tx_dir / CACHE_FILE).write_text(
        json.dumps(data, indent=1, ensure_ascii=False) + "\n")


# ---------------------------------------------------------------------------
# Claude.
# ---------------------------------------------------------------------------

def run_claude(workdir: Path, lang_code: str, model: str,
               items: List[dict]) -> Dict[str, str]:
    # Unset ANTHROPIC_API_KEY so the subscription is used, never the API.
    env = {k: v for k, v in os.environ.items() if k != "ANTHROPIC_API_KEY"}
    system = SYSTEM_PROMPT.format(lang_name=LANG_NAMES[lang_code],
                                  lang_code=lang_code)
    cmd = [
        "claude", "-p",
        "--model", model,
        "--effort", "low",
        "--output-format", "json",
        "--system-prompt", system,
        "--tools=",
        "--strict-mcp-config",
        "--setting-sources=",
        "--no-session-persistence",
    ]
    proc = subprocess.run(cmd, cwd=workdir, env=env, text=True,
                          capture_output=True,
                          input=json.dumps(items, ensure_ascii=False))
    try:
        result = json.loads(proc.stdout)
    except json.JSONDecodeError:
        result = {"is_error": True, "result": proc.stdout + proc.stderr}

    if result.get("is_error") or proc.returncode != 0:
        # Most failures here are the usage limit. Either way, stop and let the
        # next run retry this batch.
        raise QuotaExhausted(str(result.get("result", ""))[:500])

    usage = result.get("usage", {})
    log(f"    {result.get('duration_ms', 0) // 1000}s, "
        f"{usage.get('output_tokens', 0)} output tokens")
    text = result.get("result", "")
    try:
        return json.loads(text[text.index("{"):text.rindex("}") + 1])
    except ValueError:
        log("    ! could not parse the reply, skipping the batch")
        return {}


def make_batches(items: List[dict]) -> List[List[dict]]:
    batches: List[List[dict]] = [[]]
    size = 0
    for item in items:
        if batches[-1] and size + len(item["text"]) > BATCH_MAX_CHARS:
            batches.append([])
            size = 0
        batches[-1].append(item)
        size += len(item["text"])
    return [b for b in batches if b]


# ---------------------------------------------------------------------------
# Main.
# ---------------------------------------------------------------------------

def process_language(lang_code: str, ftxui_dir: Path, tx_dir: Path,
                     model: str) -> None:
    lang_name = LANG_NAMES[lang_code]
    log(f"\n==> {lang_name} ({lang_code})")
    checkout_or_create_branch(tx_dir, lang_code)

    ftxui_head = run(["git", "rev-parse", "--short", "HEAD"], cwd=ftxui_dir)
    blobs = list_blobs(ftxui_dir, "HEAD")
    memory = load_memory(tx_dir, ftxui_dir)

    # Remove files that no longer exist upstream.
    for p in list(tx_dir.rglob("*")):
        rel = str(p.relative_to(tx_dir))
        if p.is_dir() or rel.startswith(".") or "/." in rel or rel == CACHE_FILE:
            continue
        if rel not in blobs:
            p.unlink()

    files: Dict[str, Pieces] = {}
    for rel in sorted(blobs):
        src = ftxui_dir / rel
        dst = tx_dir / rel
        if is_translatable(rel):
            files[rel] = split(rel, src.read_text())
        elif not dst.exists() or not filecmp.cmp(src, dst, shallow=False):
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)

    # Drop entries no longer used, then list what is missing.
    used = {t for pieces in files.values() for b, t in pieces if b}
    memory = {e: t for e, t in memory.items() if e in used}
    missing: List[dict] = []
    seen = set(memory)
    for rel, pieces in files.items():
        for b, t in pieces:
            if b and t not in seen:
                seen.add(t)
                missing.append({"id": str(len(missing)), "file": rel,
                                "text": t})

    def write_files() -> None:
        for rel, pieces in files.items():
            dst = tx_dir / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            dst.write_text(render(pieces, memory))
        save_memory(tx_dir, memory)

    write_files()
    commit(tx_dir, f"{lang_name}: sync to {ftxui_head}")

    batches = make_batches(missing)
    log(f"  {len(memory)} blocks translated, {len(missing)} missing, "
        f"{len(batches)} batches")
    try:
        for i, batch in enumerate(batches, 1):
            log(f"  [{i}/{len(batches)}] {len(batch)} blocks")
            reply = run_claude(tx_dir, lang_code, model, batch)
            rejected = 0
            for item in batch:
                t = reply.get(item["id"])
                if isinstance(t, str) and is_valid(item["text"], t):
                    memory[item["text"]] = t
                else:
                    rejected += 1
            if rejected:
                log(f"    ! {rejected} blocks rejected, retried next run")
            write_files()
            commit(tx_dir, f"{lang_name}: translate {len(batch)} blocks "
                           f"({ftxui_head})")
    finally:
        run(["git", "checkout", "--", "."], cwd=tx_dir, check=False)
        run(["git", "clean", "-fd"], cwd=tx_dir, check=False)
        run(["git", "push", "--set-upstream", "origin", lang_code], cwd=tx_dir)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--langs", nargs="+", required=True,
                        choices=list(LANG_NAMES), help="Language codes")
    parser.add_argument("--model", default="sonnet")
    args = parser.parse_args()

    if not shutil.which("claude"):
        sys.exit("claude CLI not found.")

    build_dir = Path.cwd() / "build_translation"
    build_dir.mkdir(parents=True, exist_ok=True)
    ftxui_dir = build_dir / "ftxui"
    tx_dir = build_dir / "translations"

    ensure_repo(ftxui_dir, FTXUI_REPO_URL)
    update_to_head(ftxui_dir)
    ensure_repo(tx_dir, TRANSLATIONS_REPO_URL)
    update_to_head(tx_dir)

    try:
        for lang_code in args.langs:
            process_language(lang_code, ftxui_dir, tx_dir, args.model)
    except QuotaExhausted as e:
        log(f"\nStopping, claude failed (likely usage limit): {e}")
        return
    log("\nAll done.")


if __name__ == "__main__":
    main()
