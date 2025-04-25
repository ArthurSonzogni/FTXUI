# ftxui_common.bzl

load("@rules_cc//cc:defs.bzl", "cc_library")

def ftxui_cc_library(
        name,
        srcs,
        hdrs,
        linkopts = [],
        deps = []):

    cpp20 = select({
      "@bazel_tools//tools/cpp:msvc": ["/std:c++20"],
      "//conditions:default":         ["-std=c++20"],
    })

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
        copts = cpp20,
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
        )
