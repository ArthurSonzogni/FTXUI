# FTXUI Bazel Integration Test

This directory contains integration tests to verify that FTXUI can be properly consumed as an external dependency using Bazel with Bzlmod.

## Purpose

These tests ensure that:
- FTXUI's public API is correctly exposed to external projects
- Both single-target (`@ftxui//:ftxui`) and submodule-based dependencies work correctly
- Headers are properly re-exported and accessible from downstream projects

## Build Instructions

To build all targets:

```bash
bazel build //... --enable_bzlmod --override_module=ftxui=../..
```

To build individual targets:

```bash
# Test using individual submodules
bazel build //:smoke --enable_bzlmod --override_module=ftxui=../..

# Test using the single ftxui target
bazel build //:smoke_single_dependency --enable_bzlmod --override_module=ftxui=../..
```

## Run the Examples

```bash
# Run the submodules version
./bazel-bin/smoke

# Run the single-target version
./bazel-bin/smoke_single_dependency
```
