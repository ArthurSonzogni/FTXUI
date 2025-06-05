@page installation_xmake XMake
@tableofcontents

## XMake Package (Unofficial)

FTXUI is available in the [xmake-repo](https://github.com/xmake-io/xmake-repo).

Example `xmake.lua` snippet:

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

Refer to the [XMake documentation](https://xmake.io) for further options.

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>
