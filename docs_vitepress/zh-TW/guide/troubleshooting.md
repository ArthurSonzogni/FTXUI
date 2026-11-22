# 疑難排解與常見問題

整理最常見的建置、執行階段與版本升級問題解法。

## 事件沒有傳遞到子元件

**現象**：按鈕、輸入框或選單有正確顯示在畫面上，但鍵盤或滑鼠操作毫無反應。

**原因**：你使用了 `Renderer([&] { return child->Render(); })`，卻忘記把 `child`（或包含所有子元件的 `Container`）當作第一個參數傳入。

**解法**：務必將 `Container` 與 `Renderer(container, ...)` 配對使用：
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

詳情請參閱[心智模型](/zh-TW/guide/mental-model)指南。

---

## 手動連結時出現未定義參考（Undefined References）

**現象**：連結器報錯，提示找不到 [`ftxui::Node`](/api/class/node)、[`ftxui::Screen`](/api/class/screen) 或 [`ftxui::Element`](/api/component#element)。

**解法**：使用靜態函式庫時，連結順序非常重要（`component` 相依於 `dom`，`dom` 又相依於 `screen`）：
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
若使用 CMake，直接連結 `ftxui::ftxui`（或 `ftxui::component`，它會自動帶入 `dom` 與 `screen`）即可。

---

## 在 Linux 上以 MinGW 交叉編譯（`'mutex' is not a member of 'std'`）

**現象**：在 Debian/Ubuntu 上使用 `x86_64-w64-mingw32-g++` 編譯時失敗，出現 `error: 'mutex' is not a member of 'std'`。

**解法**：Debian/Ubuntu 預設的 MinGW 編譯器採用 `win32` 執行緒模型，並未實作 C++11 的 `<mutex>` 與 `<thread>`。請切換為 `posix` 版本：
- 在你的 CMake toolchain 檔案中，將 `CMAKE_CXX_COMPILER` 設為 `x86_64-w64-mingw32-g++-posix`，或是
- 執行 `sudo update-alternatives --config x86_64-w64-mingw32-g++` 並選擇 `x86_64-w64-mingw32-g++-posix`。

---

## Windows 巨集名稱衝突（`RGB`、`DrawText`、`min`、`max`）

**現象**：引入 `<windows.h>` 後，在 `<ftxui/screen/color.hpp>` 或 `<ftxui/dom/canvas.hpp>` 出現奇怪的編譯錯誤。

**解法**：在引入 `<windows.h>` 之前定義 `NOMINMAX` 與 `WIN32_LEAN_AND_MEAN`（或在 CMake 中加上 `-DNOMINMAX`）。FTXUI 在自身標頭檔內已經取消定義 `RGB` 與 `DrawText`，但定義 `NOMINMAX` 可以避免與 `std::min`/`std::max` 衝突。

---

## 終端機中的色彩、Unicode 或滑鼠無法正常運作

- **手動指定色彩支援等級**：
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **停用 Unicode 方塊字元，改用 ASCII 替代字元**：
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 與舊版主控台**：舊版 Windows `conhost`（Windows 10 之前）不支援虛擬終端機跳脫序列（`ENABLE_VIRTUAL_TERMINAL_PROCESSING`）。請改用 Windows Terminal 或其他現代的 VT100 相容終端機。

---

## 從 FTXUI v6 升級至 v7

FTXUI v7 為了語意更清晰，重新命名了三個核心類別（為了向後相容，舊名稱仍以棄用別名保留）：

| FTXUI v6（已棄用） | FTXUI v7 | 標頭檔 |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
