
<p align="center">
  <img src="https://github.com/ArthurSonzogni/FTXUI/assets/4759106/6925b6da-0a7e-49d9-883c-c890e1f36007" alt="Demo image"></img>
  <br/>
  <a href="#"><img src="https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white"></img></a>
  <a href="http://opensource.org/licenses/MIT"><img src="https://img.shields.io/github/license/arthursonzogni/FTXUI?color=black"></img></a>
  <a href="#"><img src="https://img.shields.io/github/stars/ArthurSonzogni/FTXUI"></img></a>
  <a href="#"><img src="https://img.shields.io/github/forks/ArthurSonzogni/FTXUI"></img></a>
  <a href="#"><img src="https://img.shields.io/github/repo-size/ArthurSonzogni/FTXUI"></img></a>
  <a href="https://github.com/ArthurSonzogni/FTXUI/graphs/contributors"><img src="https://img.shields.io/github/contributors/arthursonzogni/FTXUI?color=blue"></img></a>
  <br/>
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues"><img src="https://img.shields.io/github/issues/ArthurSonzogni/FTXUI"></img></a>
  <a href="https://repology.org/project/ftxui/versions"><img src="https://repology.org/badge/latest-versions/ftxui.svg" alt="latest packaged version(s)"></a>
  <a href="https://codecov.io/gh/ArthurSonzogni/FTXUI">
    <img src="https://codecov.io/gh/ArthurSonzogni/FTXUI/branch/master/graph/badge.svg?token=C41FdRpNVA"/>
  </a>

  
  <br/>
  <a href="https://arthursonzogni.github.io/FTXUI/">Documentation</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Report a Bug</a> ·
  <a href="https://arthursonzogni.github.io/FTXUI/examples/">Examples</a> .
  <a href="https://github.com/ArthurSonzogni/FTXUI/issues">Request Feature</a> ·
  <a href="https://github.com/ArthurSonzogni/FTXUI/pulls">Send a Pull Request</a>

  <br/>
  Choice other language version: 
  <a href="https://github.com/ArthurSonzogni/FTXUI/">English</a> |
  <a href="https://github.com/ArthurSonzogni/FTXUI/laguages/zh-TW">台灣繁體</a> |
  <a href="https://github.com/ArthurSonzogni/FTXUI/laguages/zh-HK">港澳繁體</a>
</p>

# FTXUI

<i>指令式终端 (X) UI界面 (√)</i>

一个基于终端的C++简单跨平台UI库

## 特点

 * 函数式风格。灵感来源自：[1](https://hackernoon.com/building-reactive-terminal-interfaces-in-c-d392ce34e649?gi=d9fb9ce35901) 和 [React](https://reactjs.org/)
 * （在我看来）简单而优雅的语法
 * 使用鼠标和键盘进行操作。
 * 支持 [UTF8](https://en.wikipedia.org/wiki/UTF-8) 和 [全角符号](https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms) （→ 测试）<!--译者注：我至今没有看懂这个(→ 测试)是干什么的，这玩意写在markdown里面似乎没什么用处-->
 * 支持动画效果。 [示例 1](https://arthursonzogni.github.io/FTXUI/examples/?file=component/menu_underline_animated_gallery), [示例 2](https://arthursonzogni.github.io/FTXUI/examples/?file=component/button_style)
 * 支持绘图。 [示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/canvas_animated)
 * 无依赖。
 * [C++20模块支持](https://arthursonzogni.github.io/FTXUI/cpp20-modules.html)
 * **跨平台**: Linux\/MacOS （主要目标）， WebAssembly， Windows （感谢社区的贡献者们！）.
 * 通过 [示例](#documentation) 和 [教程](#documentation) 学习
 * 多种软件包:
     - CMake [FetchContent]([https://bewagner.net/programming/2020/05/02/cmake-fetchcontent/](https://cmake.org/cmake/help/latest/module/FetchContent.html)) （推荐）
     - [Bazel](https://registry.bazel.build/modules/ftxui)
     - [vcpkg](https://vcpkg.link/ports/ftxui)
     - [Conan](https://conan.io/center/recipes/ftxui) [Debian package](https://tracker.debian.org/pkg/ftxui)
     - [Ubuntu package](https://launchpad.net/ubuntu/+source/ftxui)
     - [Arch Linux](https://aur.archlinux.org/packages/ftxui/)
    - [OpenSUSE](https://build.opensuse.org/package/show/devel:libraries:c_c++/ftxui)
    - [XMake](https://xmake.io) 仓库的 [软件包](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua)
    - [Nix](https://github.com/ArthurSonzogni/FTXUI/blob/main/flake.nix)
 * 良好的工程实践：完善的文档、测试、模糊测试、性能测试、自动化CI、自动化打包等。

## 相关文档

- [使用CMake开始](https://github.com/ArthurSonzogni/ftxui-starter)
- [使用Bazel开始](https://github.com/ArthurSonzogni/ftxui-bazel)
- [FTXUI文档库](https://arthursonzogni.github.io/FTXUI/)
- [FTXUI文档库-简体中文](https://xiaoditx.github.io/docs/ftxui%E4%B8%AD%E6%96%87%E7%BF%BB%E8%AF%91/)
- [示例 (WebAssembly)](https://arthursonzogni.github.io/FTXUI/examples/)
- [使用CMake构建](https://arthursonzogni.github.io/FTXUI/#build-cmake)
- [使用Bazel构建](https://arthursonzogni.github.io/FTXUI/#build-bazel)

## 示例
~~~cpp
    vbox({
      hbox({
        text("one") | border,
        text("two") | border | flex,
        text("three") | border | flex,
      }),

      gauge(0.25) | color(Color::Red),
      gauge(0.50) | color(Color::White),
      gauge(0.75) | color(Color::Blue),
    });
~~~

![image](https://github.com/ArthurSonzogni/FTXUI/assets/4759106/569bf043-4e85-4245-aad5-2324572135c4)

## 组件展示

#### DOM（界面元素树）

这个模块定义了一套层级化结构的元素（Element）。元素自动的管理布局并且可以响应终端的尺寸变化。（译者注：意思是编程时只需声明组件间的包含关系而不需要手动的管理每个组件的位置与大小）

这些元素在 [<ftxui/dom/elements.hpp>](https://arthursonzogni.github.io/FTXUI/elements_8hpp_source.html
) 中声明：
  
<details><summary>Layout（布局）</summary>

元素可以一下面的方式排布在一起:
  - 使用`hbox`水平排列
  - 使用`vbox`垂直排列
  - `gridbox`进行网格状的排列
  - 使用`flexbox`沿着一个方向排列，自动换行
  
元素可以通过`flex`修饰符而具有弹性。
  
使用`hbox`，`vbox`和`filler`的[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/vbox_hbox)：

![示例图片](https://user-images.githubusercontent.com/4759106/147242524-7103b5d9-1a92-4e2d-ac70-b3d6740061e3.png)
  
  
使用`gridbox`的[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/gridbox)：

![示例图片](https://user-images.githubusercontent.com/4759106/147242972-0db1f2e9-0790-496f-86e6-ed2c604f7a73.png)

使用`flexbox`的[示例](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/hflow.cpp)：

![示例图片](https://user-images.githubusercontent.com/4759106/147243064-780ac7cc-605b-475f-94b8-cf7c4aed03a5.png)

另外还请[看](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/hflow)这样一个[demo](https://arthursonzogni.github.io/FTXUI/examples/?file=component/flexbox)。

</details>

<details><summary>Style（风格）</summary>

元素可以被这些函数修饰：
  - `bold`
  - `italic`
  - `dim`
  - `inverted`
  - `underlined`
  - `underlinedDouble`
  - `blink`
  - `strikethrough`
  - `color`
  - `bgcolor`
  - `hyperlink`

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/style_gallery)：

![示例图片](https://user-images.githubusercontent.com/4759106/147244118-380bf834-9e33-40df-9ff0-07c10f2598ef.png)
  
FTXUI支持管道操作。这意味着你可以使用： `decorator1(decorator2(element))`以及`element | decorator1 | decorator2`。（译者注：这种管道操作不依赖C++版本而是通过重载运算符实现的）
  
</details>

<details><summary>Color（颜色）</summary>


FTXUI支持全部主流颜色调色板（译者注：包括16色、256色、真彩24bits）:

`color`的[展示](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/color_gallery)：
![色彩展示图](https://user-images.githubusercontent.com/4759106/147248595-04c7245a-5b85-4544-809d-a5984fc6f9e7.png)

</details>
  
<details><summary>border和separator</summary>

使用修饰符`border`和元素`separator()`来划分您的UI：
  
```cpp
auto document = vbox({
    text("top"),
    separator(),
    text("bottom"),
}) | border;

```

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/separator)：
  
![示例图片](https://user-images.githubusercontent.com/4759106/147244514-4135f24b-fb8e-4067-8896-bc53545583f7.png)
  
</details>

<details><summary>Text和Paragraph</summary>

一个简单的文本使用`text("内容")`表示。

FTXUI为了支持文本自动在空格处断航，提供了下面这些函数：
```cpp
Element paragraph(std::string text);
Element paragraphAlignLeft(std::string text);
Element paragraphAlignRight(std::string text);
Element paragraphAlignCenter(std::string text);
Element paragraphAlignJustify(std::string text);
```
  
[段落的演示](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/paragraph)
  
![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/4759106/147251370-983a06e7-6f41-4113-92b8-942f43d34d06.gif)

</details>

<details><summary>Table（表格）</summary>

一个可以轻松设置表格格式和数据的类。

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/table)：
  
![示例图片](https://user-images.githubusercontent.com/4759106/147250766-77d8ec9e-cf2b-486d-9866-1fd9f1bd2e6b.png)

</details>

<details><summary>Canvas（画布）</summary>

可以利用盲文、方块或者简单的字符在一个画布（Canvas）上绘图：（译者注：盲文可以在终端绘制比普通字符更加精细的点）
  
简单的[示例](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/canvas.cpp)：
  
![示例图片](https://user-images.githubusercontent.com/4759106/147245843-76cc62fb-ccb4-421b-aacf-939f9afb42fe.png)

复杂的[示例](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/component/canvas_animated.cpp)：
  
![ezgif com-gif-maker (3)](https://user-images.githubusercontent.com/4759106/147250538-783a8246-98e0-4a25-b032-3bd3710549d1.gif)  
</details>

#### Component（组件）

ftxui/component可用于制作动态的UI，会对用户的输入有所响应。它定义了一套`ftxui::Component`也就是组件。组件响应事件（如键盘输入、鼠标点击、窗口大小调整...）并且被渲染为元素（`Element`，见前一节“[DOM（界面元素树）](#dom界面元素树)”）。

预先构建的组件被声明在了[<ftxui/component/component.hpp>](https://arthursonzogni.github.io/FTXUI/component_8hpp_source.html)

<details><summary>展示</summary>

多种组件的[展示](https://arthursonzogni.github.io/FTXUI/examples/?file=component/gallery) 。 （[示例程序](https://arthursonzogni.github.io/FTXUI/examples/?file=component/gallery)）

![展示图片](https://user-images.githubusercontent.com/4759106/147247330-b60beb9f-e665-48b4-81c0-4b01ee95bc66.png)

</details>

<details><summary>Radiobox（单选框）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/radiobox)：
  
![示例图片](https://user-images.githubusercontent.com/4759106/147246401-809d14a5-6621-4e36-8dd9-a2d75ef2a94e.png)

</details>

<details><summary>Checkbox（复选框）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/checkbox)：

![示例图片](https://user-images.githubusercontent.com/4759106/147246646-b86926a9-1ef9-4efb-af98-48a9b62acd81.png)

</details>

<details><summary>Input（输入框）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/input)：

![示例图片](https://user-images.githubusercontent.com/4759106/147247671-f1d6f606-1845-4e94-a4a0-d4273e9ae6bd.png)

</details>

<details><summary>Toggle（切换按钮）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/toggle):

![示例图片](https://user-images.githubusercontent.com/4759106/147249383-e2201cf1-b7b8-4a5a-916f-d761e3e7ae40.png)

</details>


<details><summary>Slider（进度条）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/slider):

![示例图片](https://user-images.githubusercontent.com/4759106/147249265-7e2cad75-082c-436e-affe-44a550c480ab.png)

</details>


<details><summary>Menu（菜单）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/menu):

![示例图片](https://user-images.githubusercontent.com/4759106/147247822-0035fd6f-bb13-4b3a-b057-77eb9291582f.png)

</details>


<details><summary>ResizableSplit（可调分割线）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/resizable_split):

![ezgif com-gif-maker](https://user-images.githubusercontent.com/4759106/147248372-c55512fe-9b96-4b08-a1df-d05cf2cae431.gif)  
</details>


<details><summary>Dropdown（下拉式菜单）</summary>

[示例](https://arthursonzogni.github.io/FTXUI/examples/?file=component/dropdown):

![youtube-video-gif (3)](https://user-images.githubusercontent.com/4759106/147246982-1e821751-531c-4e1f-bc37-2fa290e143cd.gif)

</details>

<details><summary>Tab（标签选项卡）</summary>

[示例：垂直排列（vertical）](https://arthursonzogni.github.io/FTXUI/examples/?file=component/tab_vertical):
  
![ezgif com-gif-maker (1)](https://user-images.githubusercontent.com/4759106/147250144-22ff044a-4773-4ff7-a49c-12ba4034acb4.gif)

[示例：水平排列（Horizontal）](https://arthursonzogni.github.io/FTXUI/examples/?file=component/tab_horizontal):
  
  ![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/4759106/147250217-fe447e0f-7a99-4e08-948a-995087d9b40e.gif)

  

</details>

## FTXUI的第三方组件库

*想为FTXUI加入自己的组件？请随意在这里添加你开发的组件*

- [ftxui-grid-container](https://github.com/mingsheng13/grid-container-ftxui)
- [ftxui-ip-input](https://github.com/mingsheng13/ip-input-ftxui)
- [ftxui-image-view](https://github.com/ljrrjl/ftxui-image-view.git)：用于图像显示


## 使用FTXUI的项目

请自由的把你使用FTXUI开发的项目添加到下面:
- [json-tui](https://github.com/ArthurSonzogni/json-tui)
- [git-tui](https://github.com/ArthurSonzogni/git-tui)
- [ostree-tui](https://github.com/AP-Sensing/ostree-tui)
- [rgb-tui](https://github.com/ArthurSonzogni/rgb-tui)
- [chrome-log-beautifier](https://github.com/ArthurSonzogni/chrome-log-beautifier)
- [x86-64 CPU Architecture Simulation](https://github.com/AnisBdz/CPU)
- [ltuiny](https://github.com/adrianoviana87/ltuiny)
- [i3-termdialogs](https://github.com/mibli/i3-termdialogs)
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
- [turing_cmd](https://github.com/DanArmor/turing_cmd)
- [StartUp](https://github.com/StubbornVegeta/StartUp)
- [eCAL monitor](https://github.com/eclipse-ecal/ecal)
- [Path Finder](https://github.com/Ruebled/Path_Finder)
- [rw-tui](https://github.com/LeeKyuHyuk/rw-tui)
- [resource-monitor](https://github.com/catalincd/resource-monitor)
- [ftxuiFileReader](https://github.com/J0sephDavis/ftxuiFileReader)
- [ftxui_CPUMeter](https://github.com/tzzzzzzzx/ftxui_CPUMeter)
- [Captain's log](https://github.com/nikoladucak/caps-log)
- [FTowerX](https://github.com/MhmRhm/FTowerX)
- [Caravan](https://github.com/r3w0p/caravan)
- [Step-Writer](https://github.com/BrianAnakPintar/step-writer)
- [XJ music](https://github.com/xjmusic/xjmusic)
- [UDP chat](https://github.com/Sergeydigl3/udp-chat-tui)
- [2048-cpp](https://github.com/Chessom/2048-cpp)
- [Memory game](https://github.com/mikolajlubiak/memory)
- [Terminal Animation](https://github.com/mikolajlubiak/terminal_animation)
- [pciex](https://github.com/s0nx/pciex)
- [Fallout terminal hacking](https://github.com/gshigin/yet-another-fallout-terminal-hacking-game)
- [Lazylist](https://github.com/zhuyongqi9/lazylist)
- [TUISIC](https://github.com/Dark-Kernel/tuisic)
- [inLimbo](https://github.com/nots1dd/inLimbo)
- [BestEdrOfTheMarket](https://github.com/Xacone/BestEdrOfTheMarket)
- [terminal-rain](https://github.com/Oakamoore/terminal-rain)
- [keywords](https://github.com/Oakamoore/keywords) ([在线游玩 :heart:](https://oakamoore.itch.io/keywords))
- [FTB - tertminal file browser](https://github.com/Cyxuan0311/FTB)
- [openJuice](https://github.com/mikomikotaishi/openJuice)
- [SHOOT!](https://github.com/ShingZhanho/ENGG1340-Project-25Spring)
- [VerifySN (Fast Hash Tool)](https://github.com/d06i/verifySN)
- [tic-tac-toe](https://github.com/birland/tic-tac-toe)
- [typing-speed-test](https://github.com/ymcx/typing-speed-test)

### [cpp-best-practices/game_jam](https://github.com/cpp-best-practices/game_jam)

Game Jam已经做出了一些使用FTXUI开发的游戏：
- [TermBreaker](https://github.com/ArthurSonzogni/termBreaker) [**[在线游玩]**](https://arthursonzogni.com/TermBreaker/)
- [Minesweeper Marathon](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/minesweeper_marathon.md) [**[在线游玩]**](https://barlasgarden.com/minesweeper/index.html)
- [Grand Rounds](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/grandrounds.md)
- [LightsRound](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/LightsRound.v.0.1.0.md)
- [DanteO](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/danteo.md)
- [Sumo](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/sumo.md)
- [Drag Me aROUND](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/drag_me_around.md)
- [DisarmSelfDestruct](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/DisarmSelfDestruct.md)
- [TheWorld](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/TheWorld.md)
- [smoothlife](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/smoothlife.md)
- [Consu](https://github.com/cpp-best-practices/game_jam/blob/main/Jam1_April_2022/consu.md)



## 使用CMake构建FTXUI项目

**强烈**推荐使用CMake FetchContent来让你的项目依赖FTXUI，这样你就可以明确你想依赖哪一个提交版本。
```cmake
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(你的目标 PRIVATE
    # 选择一个子模块
    ftxui::component
    ftxui::dom
    ftxui::screen
)
```

## 使用Bazel构建FTXUI项目

**MODULE.bazel**
```starlark
bazel_dep(
    name = "ftxui",
    version = "v6.1.9",
)
```

**BUILD.bazel**
```starlark
cc_binary(
    name = "你的目标",
    srcs = ["你的源文件.cc"],
    deps = [
        "@ftxui//:component",
        "@ftxui//:dom",
        "@ftxui//:screen",
    ],
)
```


# 使用别的工具构建：
如果你不用上面介绍的两个工具，FTXUI也可以被以下包管理工具使用：
- CMake [FetchContent]([https://bewagner.net/programming/2020/05/02/cmake-fetchcontent/](https://cmake.org/cmake/help/latest/module/FetchContent.html)) （推荐）,
- [Bazel](https://registry.bazel.build/modules/ftxui),
- [vcpkg](https://vcpkg.link/ports/ftxui),
- [Conan](https://conan.io/center/recipes/ftxui)
- [Debian package](https://tracker.debian.org/pkg/ftxui),
- [Ubuntu package](https://launchpad.net/ubuntu/+source/ftxui),
- [Arch Linux](https://aur.archlinux.org/packages/ftxui/),
- [OpenSUSE](https://build.opensuse.org/package/show/devel:libraries:c_c++/ftxui),
[Nix](https://github.com/ArthurSonzogni/FTXUI/blob/main/flake.nix),
[![Packaging status](https://repology.org/badge/vertical-allrepos/libftxui.svg)](https://repology.org/project/libftxui/versions)

如果你选择自己构建并链接FTXUI，`ftxui-component`必须在相对于其他FTXUI库的链接顺序中位于第一个。

```bash
g++ . . . -lftxui-component -lftxui-dom -lftxui-screen . . .
```

使用模块来构建FTXUI项目，请查阅[相关文档](https://arthursonzogni.github.io/FTXUI/cpp20-modules.html)

## 贡献者

<a href="https://github.com/ArthurSonzogni/FTXUI/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=ArthurSonzogni/FTXUI" />
</a>
