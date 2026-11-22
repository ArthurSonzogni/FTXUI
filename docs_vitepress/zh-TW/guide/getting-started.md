# 快速開始

本指南將帶你把 FTXUI 加入專案、渲染靜態版面配置，並打造一個小型的互動式應用程式。

## 1. 將 FTXUI 加入你的專案

::: code-group

```cmake [CMake (FetchContent)]
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ftxui::ftxui)
```

```cpp [單一標頭檔]
// Download ftxui_all.hpp from the GitHub release artifacts:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

```python [Bazel (MODULE.bazel)]
bazel_dep(name = "ftxui", version = "7.0.3")
```

```ini [Meson (subprojects/ftxui.wrap)]
[wrap-git]
url = https://github.com/ArthurSonzogni/FTXUI.git
revision = v7.0.3
```

```json [vcpkg (vcpkg.json)]
{
  "dependencies": ["ftxui"]
}
```

```ini [Conan (conanfile.txt)]
[requires]
ftxui/7.0.3

[generators]
CMakeDeps
CMakeToolchain
```

:::

需要更多細節嗎？請參閱完整的[安裝指南](/zh-TW/install/)。

---

## 2. 靜態版面配置（DOM）

使用 `hbox`、`vbox` 以及可透過管線串接的裝飾器（`| border`、`| bold`、`| flex`）來建構宣告式文件：

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  Element title = text("FTXUI Dashboard") | bold | center;
  Element left_panel = text("Left Panel") | border;
  Element right_panel = vbox({
                            text("Main Content Area") | flex,
                            separator(),
                            text("Status: Ready") | dim,
                        })
                        | border
                        | flex;

  Element document = vbox({
      title,
      separator(),
      hbox({left_panel, right_panel}) | flex,
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
}
```

---

## 3. 加入互動功能（Component）

用 `Menu`、`Input`、`Button` 等元件包裝普通的 C++ 狀態變數，並交給 `App` 執行事件迴圈：

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  std::vector<std::string> entries = {"Dashboard", "Settings", "Logs"};
  int selected = 0;

  auto menu = Menu(&entries, &selected) | border;

  auto app = App::TerminalOutput();
  app.Loop(menu);
}
```

<FtxuiDemo file="component/menu_style" title="Interactive Menu Demo (click or use arrow keys)" height="260px" />

---

## 4. 組合版面配置、快捷鍵與互動視窗（Modal）

結合 `Container`（負責事件與焦點路由）、`Renderer(container, ...)`（負責自訂版面配置）與 `CatchEvent`（負責鍵盤快捷鍵）：

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  auto app = App::Fullscreen();
  std::string name;
  bool show_about = false;

  auto input = Input(&name, "Enter your name...");
  auto about_btn = Button("About", [&] { show_about = true; });
  auto quit_btn = Button("Quit", app.ExitLoopClosure());

  auto buttons = Container::Horizontal({about_btn, quit_btn});
  auto controls = Container::Vertical({input, buttons});

  auto main_ui = Renderer(controls, [&] {
    auto greeting =
        text("Hello " + (name.empty() ? "FTXUI" : name) + "!") | bold | hcenter;
    auto input_row = hbox({text(" Name: "), input->Render()});
    auto button_row = hbox({about_btn->Render(), quit_btn->Render()}) | hcenter;

    return vbox({
               greeting,
               separator(),
               input_row,
               separator(),
               button_row,
           })
           | border
           | center;
  });

  auto modal = Button("Close (Esc)", [&] { show_about = false; })
               | border
               | clear_under;

  auto root = main_ui
              | Modal(modal, &show_about)
              | CatchEvent([&](Event e) {
                  if (show_about && e == Event::Escape) {
                    show_about = false;
                    return true;
                  }
                  return false;
                });

  app.Loop(root);
}
```

---

## 下一步

- [心智模型](/zh-TW/guide/mental-model)：了解 Element、Component 與事件路由的運作方式。
- [實用食譜](/zh-TW/guide/cookbook)：背景執行緒、捲動檢視、動態清單與互動視窗的常見寫法。
- [Elements](/api/dom) 與 [Components](/api/component) API 參考手冊。
- [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter) 範本儲存庫，或參閱 [`/llms.txt`](/llms.txt) / [`/llms-full.txt`](/llms-full.txt)。
