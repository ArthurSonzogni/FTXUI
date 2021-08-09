# FTXUI

[![issues][badge.issues]][issues]
[![license][badge.license]][license]
[![contributors][badge.contributors]][contributors]

[badge.issues]: https://img.shields.io/github/issues-raw/arthursonzogni/FTXUI
[badge.license]: https://img.shields.io/github/license/arthursonzogni/FTXUI?color=black
[badge.contributors]: https://img.shields.io/github/contributors/arthursonzogni/FTXUI?color=blue

[issues]: https://github.com/ArthurSonzogni/FTXUI/issues
[license]: http://opensource.org/licenses/MIT
[contributors]: https://github.com/ArthurSonzogni/FTXUI/graphs/contributors

**Functional Terminal (X) User interface**

A simple C++ library for terminal based user interface.

## Demo:
![Demo image](./examples/component/homescreen.gif)

## Feature
 * Functional style. Inspired by
   [[1]](https://hackernoon.com/building-reactive-terminal-interfaces-in-c-d392ce34e649?gi=d9fb9ce35901)
   and [React](https://reactjs.org/)
 * Simple and elegant syntax (in my opinion).
 * Support for [UTF8](https://en.wikipedia.org/wiki/UTF-8) and [fullwidth chars](https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms) (→ 测试).
 * No dependencies.
 * Cross platform. Linux/mac (main target), Windows (experimental thanks to contributors), WebAssembly.
 * Keyboard & mouse navigation.

## Operating systems
- [![linux-emscripten][badge.linux-emscripten]][link.linux-emscripten]
- [![linux-gcc][badge.linux-gcc]][link.linux-gcc]
[![linux-clang][badge.linux-clang]][link.linux-clang]
- [![windows-msvc][badge.windows-msvc]][link.windows-msvc]
- [![mac-clang][badge.mac-clang]][link.mac-clang]

[badge.linux-gcc]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-gcc.yaml/badge.svg?branch=master
[badge.linux-clang]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-clang.yaml/badge.svg?branch=master
[badge.linux-emscripten]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-emscripten.yaml/badge.svg?branch=master
[badge.windows-msvc]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/windows-msvc.yaml/badge.svg?branch=master
[badge.mac-clang]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/mac-clang.yaml/badge.svg?branch=master

[link.linux-gcc]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-gcc.yaml
[link.linux-clang]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-clang.yaml
[link.linux-emscripten]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/linux-emscripten.yaml
[link.windows-msvc]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/windows-msvc.yaml
[link.mac-clang]: https://github.com/ArthurSonzogni/FTXUI/actions/workflows/mac-clang.yaml

## Example:
~~~cpp
  vbox({
    hbox({
      text("left") | border,
      text("middle") | border | flex,
      text("right") | border,
    }),
    gauge(0.5) | border,
  });
~~~

~~~bash
┌────┐┌───────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                         ││right│
└────┘└───────────────────────────────────────────────────────────────┘└─────┘
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

# Documentation:

- [Starter example project](https://github.com/ArthurSonzogni/ftxui-starter)
- [Documentation](https://arthursonzogni.github.io/FTXUI/)
- [Examples (WebAssembly)](https://arthursonzogni.com/FTXUI/examples/)
- [Build using CMake](https://arthursonzogni.com/FTXUI/doc/#build-using-cmake)
- [Build using nxxm](https://arthursonzogni.com/FTXUI/doc/#build-using-cmake)

## Project using FTXUI

Feel free to add your projects here:
- [git-tui](https://github.com/ArthurSonzogni/git-tui)
- [rgb-tui](https://github.com/ArthurSonzogni/rgb-tui)
- [chrome-log-beautifier](https://github.com/ArthurSonzogni/chrome-log-beautifier)
- [x86-64 CPU Architecture Simulation](https://github.com/AnisBdz/CPU)
- [ltuiny](https://github.com/adrianoviana87/ltuiny)
- [i3-termdialogs](https://github.com/mibli/i3-termdialogs)
- [Just-Fast](https://github.com/GiuseppeCesarano/just-fast)
- [simpPRU](https://github.com/VedantParanjape/simpPRU)
- [Pigeon ROS TUI](https://github.com/PigeonSensei/Pigeon_ros_tui)
- [hastur](https://github.com/robinlinden/hastur)
- [CryptoCalculator](https://github.com/brevis/CryptoCalculator)
- [todoman](https://github.com/aaleino/todoman)

## Hosted on:
 * [github](https://github.com/ArthurSonzogni/ftxui)
 * [gitlab](https://gitlab.com/ArthurSonzogni/ftxui)

## External package:

It is **highly** recommanded to use cmake FetchContent to depends on FTXUI. This
way you can specify which commit you would like to depends on.

If you don't, the following packages have been created:
- vcpkg ([soon](https://github.com/ArthurSonzogni/FTXUI/issues/112))
- [Arch Linux PKGBUILD](https://aur.archlinux.org/packages/ftxui-git/).
