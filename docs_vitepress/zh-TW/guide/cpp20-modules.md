# C++20 模組（Modules）

::: warning 實驗性功能
這項功能仍在開發階段，未來版本中的 API 可能會有所變動。如果你遇到任何問題或有建議，歡迎在 GitHub 上開啟 Issue。
:::

FTXUI 目前實驗性地支援 [C++20 模組（modules）](https://en.cppreference.com/w/cpp/language/modules)，以縮短編譯時間並改善程式碼組織結構。函式庫的每個部分都有對應的模組，並依各個標頭檔拆分為模組分割區（partitions）。

建置 FTXUI 專案本身時，請啟用 `FTXUI_BUILD_MODULES` 選項來編譯 C++20 模組，例如搭配 CMake 與 Ninja：

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

::: info 注意
使用模組需要 C++20 編譯器、CMake 3.20 或更新版本，以及相容的建置產生器（例如 Ninja）。Makefile 產生器不支援 C++20 模組。
:::

接著在你的程式碼中，就能照常匯入並使用模組：

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  app.Loop(button);
  return 0;
}
```

寫下 `import ftxui;` 等同於引入所有 `<ftxui/**/*.hpp>` 標頭檔，透過單一模組即可取得整個函式庫的功能。

若要在 CMake 中正確找到並連結模組，請使用 `target_link_libraries`，以套用正確的編譯器與連結器旗標：

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### 模組列表

雖然 `import ftxui;` 會匯出整個函式庫，但 FTXUI 本身是分層設計的。如果你只需要特定功能，可以直接匯入獨立模組：

- `ftxui`（整合模組，會重新匯出下列所有模組）
    - `ftxui.component`（互動元件、事件與事件迴圈）
    - `ftxui.dom`（透過 Element 進行版面配置與樣式設定）
    - `ftxui.screen`（終端機渲染、字元格與色彩）
    - `ftxui.util`（內部公用程式）

例如：
```cpp
import ftxui.screen;
import ftxui.dom;

// Use only screen and dom functionalities...
```
