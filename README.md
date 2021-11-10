<p align="center">
  <img src="./examples/component/homescreen.gif" alt="Demo image"></img>
  <br/>
  <a href="#"><img src="https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white"></img></a>
  <a href="http://opensource.org/licenses/MIT"><img src="https://img.shields.io/github/license/arthursonzogni/FTXUI?color=black"></img></a>
  <a href="#"><img src="https://img.shields.io/github/stars/ArthurSonzogni/FTXUI"></img></a>
  <a href="#"><img src="https://img.shields.io/github/forks/ArthurSonzogni/FTXUI"></img></a>
  <a href="#"><img src="https://img.shields.io/github/repo-size/ArthurSonzogni/FTXUI"></img></a>
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues"><img src="https://img.shields.io/github/issues/ArthurSonzogni/FTXUI"></img></a>
  <a href="https://github.com/ArthurSonzogni/FTXUI/graphs/contributors"><img src="https://img.shields.io/github/contributors/arthursonzogni/FTXUI?color=blue"></img></a>
  
  <br/>
  <a href="https://arthursonzogni.github.io/FTXUI/">Documentation</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Report Bug</a> ·
  <a href="https://arthursonzogni.github.io/FTXUI/examples.html">Examples</a> .
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Request Feature</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/pulls">Send a Pull Request</a>

</p>

## FTXUI

<i>Functional Terminal (X) User interface</i>

A simple C++ library for terminal based user interface.

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

- Webassembly
- Linux
- MacOS
- Windows

## Example
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

# Documentation

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
- [TimeAccumulator](https://github.com/asari555/TimeAccumulator)
- [vantage](https://github.com/gokulmaxi/vantage)

## Hosted on
 * [github](https://github.com/ArthurSonzogni/ftxui)
 * [gitlab](https://gitlab.com/ArthurSonzogni/ftxui)

## External package

It is **highly** recommanded to use cmake FetchContent to depends on FTXUI. This
way you can specify which commit you would like to depends on.

If you don't, the following packages have been created:
- vcpkg ([soon](https://github.com/ArthurSonzogni/FTXUI/issues/112))
- [Arch Linux PKGBUILD](https://aur.archlinux.org/packages/ftxui-git/).
