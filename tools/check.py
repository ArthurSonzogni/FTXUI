#!/usr/bin/env python3
"""
Run the repository checks.

Every module in tools/checks/ is a check. It exposes:

    def check(root: Path) -> list[str]

returning one message per problem found. Adding a check is adding a module.

Usage:
    ./tools/check.py          # Run every check.
    ./tools/check.py NAME...  # Run the named checks.
"""
import importlib
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
CHECKS = TOOLS / "checks"


def main(names):
    available = sorted(
        p.stem for p in CHECKS.glob("*.py") if not p.name.startswith("_")
    )
    unknown = [name for name in names if name not in available]
    if unknown:
        print(f"Unknown checks: {', '.join(unknown)}", file=sys.stderr)
        print(f"Available: {', '.join(available)}", file=sys.stderr)
        return 2

    sys.path.insert(0, str(TOOLS))
    failed = 0
    for name in names or available:
        errors = importlib.import_module(f"checks.{name}").check(ROOT)
        print(f"{'FAIL' if errors else 'OK'}: {name}")
        for error in errors:
            print(f"  {error}")
        failed += bool(errors)

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
