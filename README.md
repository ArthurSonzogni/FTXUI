# FTXUI

**Functional Terminal (X) User interface**

A simple C++ library for terminal based user interface.

## Demo:
![Demo image](./examples/component/homescreen.gif)

## Feature
 * Functional style. Inspired by
   [[1]](https://hackernoon.com/building-reactive-terminal-interfaces-in-c-d392ce34e649?gi=d9fb9ce35901)
   and [React](https://reactjs.org/)
 * Simple and elegant syntax (in my opinion).
 * Support for [UTF8](https://en.wikipedia.org/wiki/UTF-8) and [fullwidth chars](https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms) (→ 测试)
 * No dependencies.

## Example:
~~~cpp
  vbox(
    hbox(
      text(L"left") | border,
      text(L"middle") | border | flex,
      text(L"right") | border
    ),
    gauge(0.5) | border
  )
~~~

~~~bash
┌────┐┌───────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                         ││right│
└────┘└───────────────────────────────────────────────────────────────┘└─────┘
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

## Build

* [CMake](https://cmake.org) →  [Instructions](./tutorial/build-with-cmake.md)
* [nxxm](https://nxxm.github.io) → [Instructions](./tutorial/build-with-nxxm.md)

## Tutorial
See [Tutorial](./tutorial/tutorial.md)

## Project using FTXUI
None! This is still a newborn project. Feel free to add a link to your project
here.

## Hosted on:
 * [github](https://github.com/ArthurSonzogni/ftxui)
 * [gitlab](https://gitlab.com/ArthurSonzogni/ftxui)
