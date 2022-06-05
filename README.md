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
  <a href="https://codecov.io/gh/ArthurSonzogni/FTXUI">
    <img src="https://codecov.io/gh/ArthurSonzogni/FTXUI/branch/master/graph/badge.svg?token=C41FdRpNVA"/>
  </a>

  
  <br/>
  <a href="https://arthursonzogni.github.io/FTXUI/">Documentation</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Report a Bug</a> ·
  <a href="https://arthursonzogni.github.io/FTXUI/examples.html">Examples</a> .
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Request Feature</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/pulls">Send a Pull Request</a>

</p>

## FTXUI

<i>Functional Terminal (X) User interface</i>

A simple C++ library for terminal based user interfaces!

## Feature
 * Functional style. Inspired by
   [[1]](https://hackernoon.com/building-reactive-terminal-interfaces-in-c-d392ce34e649?gi=d9fb9ce35901)
   and [React](https://reactjs.org/)
 * Simple and elegant syntax (in my opinion)
 * Keyboard & mouse navigation.
 * Support for [UTF8](https://en.wikipedia.org/wiki/UTF-8) and [fullwidth chars](https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms) (→ 测试)
 * Support for animations. [Demo 1](https://arthursonzogni.com/FTXUI/examples/?file=component/menu_underline_animated_gallery), [Demo 2](https://arthursonzogni.com/FTXUI/examples/?file=component/button_style)
 * Support for drawing. [Demo](https://arthursonzogni.com/FTXUI/examples/?file=component/canvas_animated)
 * No dependencies
 * Cross platform: Linux/MacOS (main target), WebAssembly, Windows (Thanks to contributors!).
 * Learn by [examples](#documentation), and [tutorials](#documentation)
 * Multiple packages: CMake [FetchContent](https://bewagner.net/programming/2020/05/02/cmake-fetchcontent/) (preferred), vcpkg, pkgbuild, conan.
 * Good practises: documentation, tests, fuzzers, performance tests, automated CI, automated packaging, etc...

## Documentation

- [Starter example project](https://github.com/ArthurSonzogni/ftxui-starter)
- [Documentation](https://arthursonzogni.github.io/FTXUI/)
- [Examples (WebAssembly)](https://arthursonzogni.com/FTXUI/examples/)
- [Build using CMake](https://github.com/ArthurSonzogni/FTXUI/blob/master/doc/mainpage.md#using-cmake)

## Operating systems

This is expected to be cross platform. This supports / tests:
- WebAssembly
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

## Short gallery

#### DOM

This module defines a hierarchical set of Element. An element manages layout and can be responsive to the terminal dimensions.

They are declared in [<ftxui/dom/elements.hpp>](https://arthursonzogni.github.io/FTXUI/elements_8hpp_source.html
)
  
<details><summary>Layout</summary>

Element can be arranged together:
  - horizontally with `hbox`
  - vertically with `vbox`
  - inside a grid with `gridbox`
  - wrap along one direction using the `flexbox`.
  
Element can become flexible using the the `flex` decorator.
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2vbox_hbox_8cpp-example.html) using `hbox`, `vbox` and `filler`.

![image](https://user-images.githubusercontent.com/4759106/147242524-7103b5d9-1a92-4e2d-ac70-b3d6740061e3.png)
  
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2gridbox_8cpp-example.htmlp) using gridbox:

![image](https://user-images.githubusercontent.com/4759106/147242972-0db1f2e9-0790-496f-86e6-ed2c604f7a73.png)

[Example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/hflow.cpp) using flexbox:

![image](https://user-images.githubusercontent.com/4759106/147243064-780ac7cc-605b-475f-94b8-cf7c4aed03a5.png)

[See](https://arthursonzogni.github.io/FTXUI/examples_2dom_2hflow_8cpp-example.html) also this [demo](https://arthursonzogni.com/FTXUI/examples/?file=component/flexbox).

</details>

<details><summary>Style</summary>

An element can be decorated using the functions:
  - `bold`
  - `dim`
  - `inverted`
  - `underlined`
  - `blink`
  - `color`
  - `bgcolor`

[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2style_gallery_8cpp-example.html)

![image](https://user-images.githubusercontent.com/4759106/147244118-380bf834-9e33-40df-9ff0-07c10f2598ef.png)
  
FTXUI supports the pipe operator. It means: `decorator1(decorator2(element))` and `element | decorator1 | decorator2` can be used.
  
</details>

<details><summary>Colors</summary>

FTXUI support every color palette:

Color [gallery](https://arthursonzogni.github.io/FTXUI/examples_2dom_2color_gallery_8cpp-example.html):
![image](https://user-images.githubusercontent.com/4759106/147248595-04c7245a-5b85-4544-809d-a5984fc6f9e7.png)

</details>
  
<details><summary>Border and separator</summary>

Use decorator border and element separator() to subdivide your UI:
  
```cpp
auto document = vbox({
    text("top"),
    separator(),
    text("bottom"),
}) | border;

```

[Demo](https://arthursonzogni.github.io/FTXUI/examples_2dom_2separator_8cpp-example.html):
  
![image](https://user-images.githubusercontent.com/4759106/147244514-4135f24b-fb8e-4067-8896-bc53545583f7.png)
  
</details>

<details><summary>Text and paragraph</summary>

A simple piece of text is represented using `text("content")`.

To support text wrapping following spaces the following functions are provided:
```cpp
Element paragraph(std::string text);
Element paragraphAlignLeft(std::string text);
Element paragraphAlignRight(std::string text);
Element paragraphAlignCenter(std::string text);
Element paragraphAlignJustify(std::string text);
```
  
[Paragraph example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2paragraph_8cpp-example.html)
  
![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/4759106/147251370-983a06e7-6f41-4113-92b8-942f43d34d06.gif)

</details>

<details><summary>Table</summary>

A class to easily style a table of data.

[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2table_8cpp-example.html):
  
![image](https://user-images.githubusercontent.com/4759106/147250766-77d8ec9e-cf2b-486d-9866-1fd9f1bd2e6b.png)

</details>

<details><summary>Canvas</summary>

Drawing can be made on a Canvas, using braille, block, or simple characters:
  
Simple [example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/canvas.cpp):
  
![image](https://user-images.githubusercontent.com/4759106/147245843-76cc62fb-ccb4-421b-aacf-939f9afb42fe.png)

Complex [examples](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/component/canvas_animated.cpp):
  
![ezgif com-gif-maker (3)](https://user-images.githubusercontent.com/4759106/147250538-783a8246-98e0-4a25-b032-3bd3710549d1.gif)  
</details>

#### Component

The ftxui/component is needed when you want to produce dynamic UI, reactive to the user's input. It defines a set of ftxui::Component. A component reacts to Events (keyboard, mouse, resize, ...) and Render Element (see previous section).

Prebuilt components are declared in [<ftxui/component/component.hpp>](https://arthursonzogni.github.io/FTXUI/component_8hpp_source.html)

<details><summary>Gallery</summary>

[Gallery](https://arthursonzogni.github.io/FTXUI/examples_2component_2gallery_8cpp-example.html) of multiple components. ([demo](https://arthursonzogni.com/FTXUI/examples/?file=component/gallery))

![image](https://user-images.githubusercontent.com/4759106/147247330-b60beb9f-e665-48b4-81c0-4b01ee95bc66.png)

</details>

<details><summary>Radiobox</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2radiobox_8cpp-example.html):
  
![image](https://user-images.githubusercontent.com/4759106/147246401-809d14a5-6621-4e36-8dd9-a2d75ef2a94e.png)

</details>

<details><summary>Checkbox</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2checkbox_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147246646-b86926a9-1ef9-4efb-af98-48a9b62acd81.png)

</details>

<details><summary>Input</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2input_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147247671-f1d6f606-1845-4e94-a4a0-d4273e9ae6bd.png)

</details>

<details><summary>Toggle</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2toggle_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147249383-e2201cf1-b7b8-4a5a-916f-d761e3e7ae40.png)

</details>


<details><summary>Slider</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2slider_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147249265-7e2cad75-082c-436e-affe-44a550c480ab.png)

</details>


<details><summary>Menu</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2menu_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147247822-0035fd6f-bb13-4b3a-b057-77eb9291582f.png)

</details>


<details><summary>ResizableSplit</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2resizable_split_8cpp-example.html):

![ezgif com-gif-maker](https://user-images.githubusercontent.com/4759106/147248372-c55512fe-9b96-4b08-a1df-d05cf2cae431.gif)  
</details>


<details><summary>Dropdown</summary>

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2dropdown_8cpp-example.html):

![youtube-video-gif (3)](https://user-images.githubusercontent.com/4759106/147246982-1e821751-531c-4e1f-bc37-2fa290e143cd.gif)

</details>

<details><summary>Tab</summary>

[Vertical](https://arthursonzogni.github.io/FTXUI/examples_2component_2tab_vertical_8cpp-example.html):
  
![ezgif com-gif-maker (1)](https://user-images.githubusercontent.com/4759106/147250144-22ff044a-4773-4ff7-a49c-12ba4034acb4.gif)

[Horizontal](https://arthursonzogni.github.io/FTXUI/examples_2component_2tab_horizontal_8cpp-example.html):
  
  ![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/4759106/147250217-fe447e0f-7a99-4e08-948a-995087d9b40e.gif)

  

</details>





## Project using FTXUI

Feel free to add your projects here:
- [json-tui](https://github.com/ArthurSonzogni/json-tui)
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
- [tabdeeli](https://github.com/typon/tabdeeli)
- [tiles](https://github.com/tusharpm/tiles)
- [cachyos-cli-installer](https://github.com/cachyos/new-cli-installer)
- [beagle-config](https://github.com/SAtacker/beagle-config)

## [cpp-best-practices/game_jam](https://github.com/cpp-best-practices/game_jam)

Several games using the FTXUI have been made during the Game Jam:
- [TermBreaker](https://github.com/ArthurSonzogni/termBreaker) [**[Play web version]**](https://arthursonzogni.com/TermBreaker/)
- [Minesweeper Marathon](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/minesweeper_marathon.md) [**[Play web version]**](https://barlasgarden.com/minesweeper/index.html)
- [Grand Rounds](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/grandrounds.md)
- [LightsRound](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/LightsRound.v.0.1.0.md)
- [DanteO](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/danteo.md)
- [Sumo](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/sumo.md)
- [Drag Me aROUND](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/drag_me_around.md)
- [DisarmSelfDestruct](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/LightsRound.v.0.1.0.md)
- [TheWorld](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/TheWorld.md)
- [smoothlife](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/smoothlife.md)
- [Consu](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/consu.md)

## External package

It is **highly** recommended to use CMake FetchContent to depend on FTXUI. This
way you can specify which commit you would like to depend on.
```cmake
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v2.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
```

If you don't, the following packages have been created:
- [vcpkg](https://vcpkg.info/port/ftxui)
- [Arch Linux PKGBUILD](https://aur.archlinux.org/packages/ftxui-git/).
- [conan.io](https://conan.io/center/ftxui)

## Contributors

<a href="https://github.com/ArthurSonzogni/FTXUI/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=ArthurSonzogni/FTXUI" />
</a>
