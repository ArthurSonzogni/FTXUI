@page installation_bazel Bazel

FTXUI can be integrated into your project using [Bazel](https://bazel.build)
with Bzlmod (Bazel modules).

The library is registered in the [Bazel Central Registry](https://registry.bazel.build/modules/ftxui)

**MODULE.bazel**
```starlark
bazel_dep(name = "ftxui", version = "6.1.9")
```

**BUILD.bazel**
```starlark
cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "@ftxui//:component",
        "@ftxui//:dom",
        "@ftxui//:screen",
    ],
)
```

## Starter Project

You can use the official Bazel starter project for a minimal working setup:

- [ftxui-bazel (starter)](https://github.com/ArthurSonzogni/ftxui-bazel)
 
<div class="section_buttons">
 
| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |
 
</div>
