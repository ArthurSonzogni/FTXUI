"""
Source files must have unique names.

Some build systems, like Visual Studio's own, put every object file in a single
directory. Two sources with the same name overwrite each other's object file,
and the symbols of one of them go missing at link time. See #896.
"""
from collections import defaultdict

from checks._common import tracked_files


def check(root):
    paths_by_name = defaultdict(list)
    for path in tracked_files(root, "src/*.cpp"):
        paths_by_name[path.name].append(path)

    return [
        f"{name} is used by several files: {', '.join(map(str, paths))}"
        for name, paths in sorted(paths_by_name.items())
        if len(paths) > 1
    ]
