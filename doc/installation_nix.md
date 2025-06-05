@page installation_nix Nix
@tableofcontents

## Nix Flake (Official)

FTXUI ships with a `flake.nix` providing both packages and a development shell.

### Build the Library

```bash
nix build github:ArthurSonzogni/FTXUI
```

The resulting package is accessible via the `result` link.

### Use as a Dependency

Add FTXUI to your flake inputs:

```nix
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

Then reference `ftxui.packages.<system>.ftxui` in your outputs.

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>
