# ftxui_common.bzl

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_binary")

# Microsoft terminal is a bit buggy ¯\_(ツ)_/¯ and MSVC uses bad defaults.
def windows_copts():
    MSVC_COPTS = [
      # Microsoft Visual Studio must decode sources files as UTF-8.
      "/utf-8",

      # Microsoft Visual Studio must interpret the codepoint using unicode.
      "/DUNICODE",
      "/D_UNICODE",

      # Fallback for Microsoft Terminal.
      # This
      # - Replace missing font symbols by others.
      # - Reduce screen position pooling frequency to deals against a Microsoft
      #   race condition. This was fixed in 2020, but clients never not updated.
      #   - https://github.com/microsoft/terminal/pull/7583
      #   - https://github.com/ArthurSonzogni/FTXUI/issues/136
      "/DFTXUI_MICROSOFT_TERMINAL_FALLBACK",
    ]

    WINDOWS_COPTS = [
      # Fallback for Microsoft Terminal.
      # This
      # - Replace missing font symbols by others.
      # - Reduce screen position pooling frequency to deals against a Microsoft
      #   race condition. This was fixed in 2020, but clients are still using
      #   old versions.
      #   - https://github.com/microsoft/terminal/pull/7583
      #   - https://github.com/ArthurSonzogni/FTXUI/issues/136
      "-DFTXUI_MICROSOFT_TERMINAL_FALLBACK",
    ];
    
    return select({
        # MSVC:
        "@rules_cc//cc/compiler:msvc-cl": MSVC_COPTS,
        "@rules_cc//cc/compiler:clang-cl": MSVC_COPTS,
        "@platforms//os:windows": WINDOWS_COPTS,
        "//conditions:default": [],
    })

def pthread_linkopts():
    return select({
        # With MSVC, threading is already built-in (you don't need -pthread.
        "@rules_cc//cc/compiler:msvc-cl": [],
        "@rules_cc//cc/compiler:clang-cl": [],
        "@rules_cc//cc/compiler:clang": ["-pthread"],
        "@rules_cc//cc/compiler:gcc": ["-pthread"],
        "//conditions:default": ["-pthread"],
    })

def ftxui_cc_library(
        name,
        srcs = [],
        hdrs = [],
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
        copts = windows_copts(),
        visibility = ["//visibility:public"],
    )

# Compile all the examples in the examples/ directory.
# This is useful to check the Bazel is always synchronized against CMake
# definitions.
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

        cc_binary(
            name = name,
            srcs = [src],
            deps = [
              ":component",
              ":dom",
              ":screen",
            ],
            copts = windows_copts(),
        )
