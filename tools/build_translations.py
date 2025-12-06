#!/usr/bin/env python3
"""
FTXUI → translations translator, v2.9 (Full Rich Console Output).
- Diff-aware and cost-optimized.
- AGGRESSIVE Throttling for Free Tier (max ~5-6 RPM).
- Parses JSON streams (Robust) for accurate token usage tracking.
- DEBUG: Prints the full raw output stream as formatted, syntax-highlighted YAML using 'rich'.
- CONSOLE: All logging and output now uses rich markup and styling.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import time
import sys
from collections import deque
from pathlib import Path
from typing import Any, Dict, List, Optional
# If 'rich' or 'pyyaml' is not installed, install them via: pip install rich pyyaml
from rich.console import Console
from rich.syntax import Syntax
import yaml

# Initialize rich console once for colored printing
# Using style="dark_sea_green4" for a pleasant terminal default color
console = Console()

# ---------------------------------------------------------------------------
# Config & Constants
# ---------------------------------------------------------------------------
FTXUI_REPO_URL = "git@github.com:ArthurSonzogni/FTXUI.git"
TRANSLATIONS_REPO_URL = "git@github.com:ArthurSonzogni/ftxui-translations.git"
MODEL = "gemini-2.5-flash"

# --- FREE TIER LIMITS (Conservative) ---
LIMIT_RPM = 1_000           # Max requests per minute
LIMIT_TPM = 1_000_000   # Tokens per minute
POST_REQUEST_DELAY = 2 # Seconds to SLEEP after every request.

CPP_EXT = {".cppm", ".cpp", ".hpp", ".h", ".ipp"}
MD_EXT = {".md"}
TRANSLATABLE_EXT = CPP_EXT | MD_EXT

ALLOWED_TOOLS = ",".join([
    "list_directory",
    "read_file",
    "write_file",
    "glob",
    "search_file_content",
    "replace",
    "read_many_files",
])

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

# ---------------------------------------------------------------------------
# Rich Console Helpers
# ---------------------------------------------------------------------------

def print_step(msg: str):
    # Big, bold step indicator
    console.print(f"\n[bold deep_sky_blue1]==> {msg}[/]", highlight=False)

def print_info(msg: str):
    # Standard informational message
    console.print(f"[cyan]  -> {msg}[/cyan]", highlight=False)

def print_success(msg: str):
    # Successful outcome
    console.print(f"[green]  ✓ {msg}[/green]", highlight=False)

def print_warn(msg: str):
    # Warning message
    console.print(f"[yellow]  ! {msg}[/yellow]", highlight=False)

def print_err(msg: str):
    # Critical error message with reverse styling
    console.print(f"[bold white on red]  X {msg}[/]", highlight=False)

# ---------------------------------------------------------------------------
# Rate Limiter
# ---------------------------------------------------------------------------
class RateLimiter:
    def __init__(self, rpm_limit: int, tpm_limit: int):
        self.rpm_limit = rpm_limit
        self.tpm_limit = tpm_limit
        self.requests: deque[float] = deque()
        self.tokens: deque[tuple[float, int]] = deque()
        self.session_requests = 0

    def _cleanup(self, now: float):
        """Remove entries older than 60 seconds."""
        window_start = now - 60.0
        while self.requests and self.requests[0] < window_start:
            self.requests.popleft()
        while self.tokens and self.tokens[0][0] < window_start:
            self.tokens.popleft()

    def wait_for_capacity(self, estimated_tokens: int = 1000):
        while True:
            now = time.time()
            self._cleanup(now)
            
            if len(self.requests) >= self.rpm_limit:
                wait_time = 60.0 - (now - self.requests[0]) + 1.0
                print_warn(f"RPM limit reached ({self.rpm_limit}). Cooling down for {wait_time:.1f}s...")
                time.sleep(wait_time)
                continue

            current_tpm = sum(count for _, count in self.tokens)
            if current_tpm + estimated_tokens > self.tpm_limit:
                if self.tokens:
                    wait_time = 60.0 - (now - self.tokens[0][0]) + 1.0
                    print_warn(f"TPM limit saturation ({current_tpm}/{self.tpm_limit}). Cooling down for {wait_time:.1f}s...")
                    time.sleep(wait_time)
                    continue
            
            break

    def record_usage(self, input_tok: int, output_tok: int):
        now = time.time()
        total = input_tok + output_tok
        self.requests.append(now)
        self.tokens.append((now, total))
        print_info(f"Usage recorded: {total} tokens (In: {input_tok}, Out: {output_tok})")
    
    def increment_session_counter(self):
        self.session_requests += 1

limiter = RateLimiter(LIMIT_RPM, LIMIT_TPM)

# ---------------------------------------------------------------------------
# Prompts
# ---------------------------------------------------------------------------
AGENT_NEW_FILE_PROMPT = """\
You are an autonomous documentation translator.

GOAL
- Translate a single, NEW file to {lang_name} ("{lang_code}").
- The file at {tx_root}/{rel_path} is currently a copy of the English source.
- Translate IN-PLACE.

WORKFLOW
1. Read {tx_root}/{rel_path}
2. Translate ONLY documentation:
   * C++ comments (//, /* ... */)
   * Doxygen comments (///, /** ... */).
   * Prose in Markdown.
3. DO NOT translate/modify:
   * C/C++ code, identifiers, includes, macros.
   * Doxygen commands/params.
   * Markdown code fences/URLs.
4. Overwrite {tx_root}/{rel_path} with the translation.

TOOLS: {allowed_tools}.
"""

AGENT_DIFF_FILE_PROMPT = """\
You are an autonomous documentation translator.

GOAL
- Update existing translation: {tx_root}/{rel_path}
- Target: "{lang_name}" ("{lang_code}").

CONTEXT: SOURCE DIFF
The English source has changed. `git diff`:
```diff
{diff}
```

WORKFLOW
1. Read {tx_root}/{rel_path}
2. Analyze the `diff`.
3. For each changed English section:
   a. Find corresponding old translation.
   b. Generate new translation for the new English text.
   c. Update the file using `replace`.
4. ONLY update text where the source changed.
5. DO NOT translate code.

TOOLS: {allowed_tools}.
"""

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def run(cmd: List[str], cwd: Path | None = None, check: bool = True) -> str:
    proc = subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)
    if check and proc.returncode != 0:
        raise RuntimeError(f"Command failed: {' '.join(cmd)}\nStderr: {proc.stderr}")
    return proc.stdout.strip()

def ensure_repo(path: Path, url: str) -> None:
    if path.exists() and (path / ".git").is_dir():
        run(["git", "fetch", "--all", "--prune"], cwd=path)
        return
    if path.exists():
        shutil.rmtree(path)
    run(["git", "clone", url, str(path)])

def update_to_head(path: Path) -> None:
    try:
        ref = run(["git", "symbolic-ref", "--short", "refs/remotes/origin/HEAD"], cwd=path)
        default_branch = ref.split("/")[-1]
    except Exception:
        default_branch = "main"
    run(["git", "checkout", default_branch], cwd=path)
    run(["git", "pull", "--ff-only"], cwd=path)

def checkout_or_create_branch(repo: Path, branch: str) -> None:
    if run(["git", "branch", "--list", branch], cwd=repo):
        run(["git", "checkout", branch], cwd=repo)
        run(["git", "pull", "--ff-only"], cwd=repo, check=False)
    elif run(["git", "ls-remote", "--heads", "origin", branch], cwd=repo):
        run(["git", "checkout", "-t", f"origin/{branch}"], cwd=repo)
        run(["git", "pull", "--ff-only"], cwd=repo)
    else:
        run(["git", "checkout", "-b", branch], cwd=repo)

def ensure_gemini() -> None:
    if not shutil.which("gemini"):
        print_err("gemini CLI not found. Install it and set GEMINI_API_KEY.")
        sys.exit(1)

def parse_and_accumulate_usage(line_str: str) -> bool:
    """
    Parses a JSON line from the gemini CLI stream.
    Updates the global limiter if usage data is found, checking for both
    'usageMetadata' (standard) and 'stats' (error summary) formats.
    """
    try:
        data = json.loads(line_str)
    except json.JSONDecodeError:
        return False

    # 1. Look for standard usage keys (camelCase or snake_case)
    usage = data.get("usageMetadata") or data.get("usage_metadata")

    # 2. Look for nested usage (e.g. inside 'result' or 'candidates')
    if not usage and "result" in data and isinstance(data["result"], dict):
        usage = data["result"].get("usageMetadata") or data["result"].get("usage_metadata")
    
    # 3. CRITICAL: Check for 'stats' key in error/result chunks (CLI-specific format)
    if not usage:
        usage = data.get("stats")

    # If found, record it
    if usage:
        # Prioritize standard keys, fall back to 'stats' snake_case keys if needed.
        prompt_tok = usage.get("promptTokenCount") or usage.get("prompt_token_count") or usage.get("input_tokens") or 0
        cand_tok = usage.get("candidatesTokenCount") or usage.get("candidates_token_count") or usage.get("output_tokens") or 0
        total_tok = usage.get("totalTokenCount") or usage.get("total_token_count") or usage.get("total_tokens") or 0
        
        if total_tok > 0:
            console.print() # Newline before recording usage for clean output
            limiter.record_usage(prompt_tok, cand_tok)
    
    if "error" in data:
        console.print()
        print_err(f"API Error: {data['error']}")
    
    return True

def run_gemini_agent(workdir: Path, prompt: str) -> None:
    limiter.wait_for_capacity(estimated_tokens=500)
    limiter.increment_session_counter()

    cmd = [
        "gemini",
        "--model", MODEL,
        f"--allowed-tools={ALLOWED_TOOLS}",
        "--output-format", "stream-json",
        "--approval-mode", "auto_edit",
        prompt,
    ]
    
    print_info(f"Invoking Agent ({MODEL})...")
    
    process = subprocess.Popen(
        cmd, 
        cwd=workdir, 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE, 
        text=True, 
        bufsize=1
    )

    if process.stdout:
        for line in process.stdout:
            line = line.strip()
            if not line:
                continue
            
            try:
                # Load the JSON data
                data = json.loads(line)
                
                # Convert Python object (from JSON) to YAML string for readability
                # Use safe_dump for security and sort_keys=False to maintain streaming order
                yaml_output = yaml.safe_dump(data, indent=2, sort_keys=False)
                
                console.print("--- Agent Stream Chunk (YAML) ---", style="bold magenta")
                # Use rich.syntax.Syntax for colored YAML output with monokai theme
                syntax = Syntax(yaml_output, "yaml", theme="monokai", word_wrap=True)
                console.print(syntax, justify="left")
                
                # Still run the usage tracking logic on the raw line string
                parse_and_accumulate_usage(line)

            except json.JSONDecodeError:
                # Handle non-JSON output (e.g., occasional non-stream lines from the CLI)
                print(f"  [Agent Non-JSON Output] {line}")
            
    _, stderr_str = process.communicate()
    
    # No final print() needed, as usage recording handles the newlines.

    if process.returncode != 0:
        print_err("Gemini Agent failed.")
        console.print(stderr_str)
        raise RuntimeError("Agent execution failed")
    
    print_success("Agent finished task.")

    if POST_REQUEST_DELAY > 0:
        print_info(f"Cooling down for {POST_REQUEST_DELAY}s ...")
        time.sleep(POST_REQUEST_DELAY)

# ---------------------------------------------------------------------------
# Filesystem
# ---------------------------------------------------------------------------

def is_hidden(rel: Path) -> bool:
    return any(p.startswith(".") for p in rel.parts)

def list_repo_files(root: Path) -> List[Path]:
    out: List[Path] = []
    for p in root.rglob("*"):
        if p.is_dir(): continue
        rel = p.relative_to(root)
        if is_hidden(rel): continue
        out.append(rel)
    return out

def load_cache(path: Path) -> Dict[str, Any]:
    if path.exists():
        try:
            with open(path, "r") as f: return json.load(f)
        except: pass
    return {}

def save_cache(path: Path, data: Dict[str, Any]) -> None:
    with open(path, "w") as f: json.dump(data, f, indent=2)

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--langs", nargs="+", required=True, help="Language codes")
    args = parser.parse_args()

    ensure_gemini()
    
    root_dir = Path.cwd()
    build_dir = root_dir / "build_translation"
    build_dir.mkdir(parents=True, exist_ok=True)
    
    ftxui_dir = build_dir / "ftxui"
    tx_dir = build_dir / "translations"

    print_step("Ensuring source repo (FTXUI)...")
    ensure_repo(ftxui_dir, FTXUI_REPO_URL)
    update_to_head(ftxui_dir)
    ftxui_head = run(["git", "rev-parse", "HEAD"], cwd=ftxui_dir)
    print_info(f"FTXUI HEAD: {ftxui_head[:8]}")

    print_step("Ensuring translations repo...")
    ensure_repo(tx_dir, TRANSLATIONS_REPO_URL)
    update_to_head(tx_dir)

    print_step("Scanning files...")
    all_files = list_repo_files(ftxui_dir)
    source_set = {str(p) for p in all_files}
    all_files.sort()

    for lang_code in args.langs:
        lang_name = LANG_NAMES.get(lang_code, lang_code)
        print_step(f"Processing Language: {lang_name} ({lang_code})")
        
        checkout_or_create_branch(tx_dir, lang_code)
        
        cache_path = tx_dir / CACHE_FILE
        cache = load_cache(cache_path)
        last_hash = cache.get("last_processed_ftxui_commit")

        if last_hash == ftxui_head:
            print_success("Up to date. Skipping.")
            continue

        changed_set = set()
        if last_hash:
            diff_files = run(["git", "diff", "--name-only", last_hash, ftxui_head], cwd=ftxui_dir, check=False)
            changed_set = set(diff_files.splitlines())
            print_info(f"Changes detected: {len(changed_set)} files changed since {last_hash[:8]}")
        else:
            print_info("No history found. Full scan.")

        for p in tx_dir.rglob("*"):
            if p.is_dir(): continue
            rel = p.relative_to(tx_dir)
            if is_hidden(rel) or str(rel) == CACHE_FILE: continue
            if str(rel) not in source_set:
                print_warn(f"Removing orphan: {rel}")
                p.unlink()

        processed_count = 0
        total_files = len(all_files)

        for idx, rel in enumerate(all_files, 1):
            src = ftxui_dir / rel
            dst = tx_dir / rel
            rel_s = str(rel)
            dst.parent.mkdir(parents=True, exist_ok=True)
            
            is_translatable = rel.suffix.lower() in TRANSLATABLE_EXT
            is_changed = rel_s in changed_set
            dst_exists = dst.exists()

            prefix = f"[{idx}/{total_files}] {rel_s}"

            if not is_translatable:
                if is_changed or not dst_exists:
                    print_info(f"{prefix} -> Copying (Asset)")
                    shutil.copy2(src, dst)
                    processed_count += 1
                continue

            if dst_exists and last_hash and not is_changed:
                continue

            console.print("-" * 60, style="dim")
            
            if not dst_exists or not last_hash:
                print_info(f"{prefix} -> New Translation")
                shutil.copy2(src, dst)
                prompt = AGENT_NEW_FILE_PROMPT.format(
                    tx_root="translations", rel_path=rel_s,
                    lang_code=lang_code, lang_name=lang_name,
                    allowed_tools=ALLOWED_TOOLS
                )
                run_gemini_agent(build_dir, prompt)
                processed_count += 1

            elif is_changed:
                print_info(f"{prefix} -> Updating (Diff-based)")
                diff_out = run(["git", "diff", last_hash, ftxui_head, "--", rel_s], cwd=ftxui_dir)
                
                if len(diff_out) > 20_000:
                    print_warn(f"Diff too large ({len(diff_out)} chars). Copying source for full re-translation.")
                    shutil.copy2(src, dst)
                    prompt = AGENT_NEW_FILE_PROMPT.format(
                        tx_root="translations", rel_path=rel_s,
                        lang_code=lang_code, lang_name=lang_name,
                        allowed_tools=ALLOWED_TOOLS
                    )
                else:
                    prompt = AGENT_DIFF_FILE_PROMPT.format(
                        tx_root="translations", rel_path=rel_s,
                        lang_code=lang_code, lang_name=lang_name,
                        allowed_tools=ALLOWED_TOOLS,
                        diff=diff_out
                    )
                
                run_gemini_agent(build_dir, prompt)
                processed_count += 1

        if processed_count > 0:
            cache["last_processed_ftxui_commit"] = ftxui_head
            save_cache(cache_path, cache)
            
            status = run(["git", "status", "--porcelain"], cwd=tx_dir, check=False)
            if status.strip():
                print_step("Committing changes...")
                run(["git", "add", "-A"], cwd=tx_dir)
                run(["git", "commit", "-m", f"{lang_name}: update translations to {ftxui_head[:8]}"], cwd=tx_dir, check=False)
                run(["git", "push", "--set-upstream", "origin", lang_code], cwd=tx_dir)
                print_success("Pushed.")
            else:
                print_success("No file changes detected after processing.")
        else:
            print_success("Nothing to process.")

    print_step("All Done.")

if __name__ == "__main__":
    main()
