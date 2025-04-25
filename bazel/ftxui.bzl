# ftxui_common.bzl

load("@rules_cc//cc:defs.bzl", "cc_library")

def cpp17():
    return select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++17"],
        "@rules_cc//cc/compiler:clang-cl": ["/std:c++17"],
        "@rules_cc//cc/compiler:clang": ["-std=c++17"],
        "@rules_cc//cc/compiler:gcc": ["-std=c++17"],
        "//conditions:default": ["-std=c++17"],
    })

def cpp20():
    return select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++20"],
        "@rules_cc//cc/compiler:clang-cl": ["/std:c++20"],
        "@rules_cc//cc/compiler:clang": ["-std=c++20"],
        "@rules_cc//cc/compiler:gcc": ["-std=c++20"],
        "//conditions:default": ["-std=c++20"],
    })

# Force Microsoft Visual Studio to decode sources files in UTF-8, as it should.
def utf8():
    return select({
        "@rules_cc//cc/compiler:msvc-cl": ["/utf-8"],
        "@rules_cc//cc/compiler:clang-cl": ["/utf-8"],
        "//conditions:default": [],
    })

def ftxui_cc_library(
        name,
        srcs,
        hdrs,
        linkopts = [],
        deps = []):

    cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        linkopts = linkopts,
        deps = deps,
        strip_include_prefix = "",
        include_prefix = "",
        includes = [
            "include",
            "src",
        ],
        copts = cpp17() + utf8(),
        visibility = ["//visibility:public"],
    )

def generate_examples():
    cpp_files = native.glob(["examples/**/*.cpp"])

    for src in cpp_files:
        # Skip failing examples due to the color_info_sorted_2d.ipp dependency.
        if src == "examples/component/homescreen.cpp" or \
           src == "examples/dom/color_info_palette256.cpp" or \
           src == "examples/dom/color_gallery.cpp":
            continue

        # Turn "examples/component/button.cpp" → "example_component_button"
        name = src.replace("/", "_").replace(".cpp", "")

        native.cc_binary(
            name = name,
            srcs = [src],
            deps = ["//:component"],
            copts = cpp20() + utf8(),
        )
