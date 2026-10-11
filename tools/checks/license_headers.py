"""
C++ files must start with the license header.

./tools/format.sh adds it to the files missing it.
"""
import re

from checks._common import tracked_files

HEADER = re.compile(
    r"// Copyright \d{4} Arthur Sonzogni\. All rights reserved\.\n"
    r"// Use of this source code is governed by the MIT license that can be "
    r"found in\n"
    r"// the LICENSE file\.\n"
)


def check(root):
    return [
        f"{path} doesn't start with the license header"
        for path in tracked_files(
            root, "src/*.cpp", "src/*.hpp", "include/*.hpp", "examples/*.cpp"
        )
        if not HEADER.match((root / path).read_text(encoding="utf-8"))
    ]
