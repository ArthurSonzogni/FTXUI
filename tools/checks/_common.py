"""Helpers shared by the checks."""
import subprocess
from pathlib import Path


def tracked_files(root, *patterns):
    """The files known to git (tracked or new) matching |patterns|."""
    output = subprocess.check_output(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard",
         "--", *patterns],
        cwd=root,
        text=True,
    )
    # Files deleted but not yet staged are still listed.
    return [Path(line) for line in output.splitlines() if (root / line).exists()]


def is_library_source(path):
    """Whether |path| is a .cpp file compiled into the FTXUI libraries."""
    return (
        path.suffix == ".cpp"
        and path.parts[:2] == ("src", "ftxui")
        and not path.stem.endswith(("_test", "_fuzzer"))
    )
