# トラブルシューティング & FAQ

ビルド、実行時、およびバージョン移行でよくある問題とその解決策をまとめています。

## 子コンポーネントにイベントが届かない

**症状:** ボタン、入力欄、メニューなどが画面に描画されるのに、キーボードやマウスで操作しても反応しない。

**原因:** 第1引数に子コンポーネント（または子を含む `Container`）を渡さず、`Renderer([&] { return child->Render(); })` のように書いている。

**解決策:** 必ず `Container` と `Renderer(container, ...)` を組み合わせてください。
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

詳しくは[メンタルモデル](/ja/guide/mental-model)のガイドを参照してください。

---

## 手動リンク時の未定義参照エラー (Undefined References)

**症状:** [`ftxui::Node`](/api/class/node)、[`ftxui::Screen`](/api/class/screen)、[`ftxui::Element`](/api/component#element) などに関するリンカエラーが出る。

**解決策:** 静的ライブラリをリンクする場合はリンク順が重要です（`component` は `dom` に依存し、`dom` は `screen` に依存します）。
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
CMake の場合は `ftxui::ftxui`（または `dom` と `screen` を推移的にリンクする `ftxui::component`）をリンクするだけで済みます。

---

## Linux での MinGW クロスコンパイル (`'mutex' is not a member of 'std'`)

**症状:** Debian/Ubuntu 上で `x86_64-w64-mingw32-g++` を使ってビルドすると、`error: 'mutex' is not a member of 'std'` で失敗する。

**解決策:** Debian/Ubuntu のデフォルトの MinGW コンパイラは `win32` スレッドモデルを使用しており、C++11 の `<mutex>`/`<thread>` が実装されていません。`posix` バリアントに切り替えてください。
- CMake ツールチェインファイルで `CMAKE_CXX_COMPILER` に `x86_64-w64-mingw32-g++-posix` を指定する、または
- `sudo update-alternatives --config x86_64-w64-mingw32-g++` を実行して `x86_64-w64-mingw32-g++-posix` を選択する。

---

## Windows のマクロ衝突 (`RGB`, `DrawText`, `min`, `max`)

**症状:** `<windows.h>` をインクルードした後に `<ftxui/screen/color.hpp>` や `<ftxui/dom/canvas.hpp>` で奇妙なコンパイルエラーが発生する。

**解決策:** `<windows.h>` をインクルードする前に `NOMINMAX` と `WIN32_LEAN_AND_MEAN` を定義してください（または CMake で `-DNOMINMAX` を渡します）。FTXUI 側のヘッダで `RGB` と `DrawText` は `#undef` していますが、`std::min`/`std::max` の衝突を防ぐために `NOMINMAX` の定義が必要です。

---

## ターミナルで色、Unicode、マウスが正常に動作しない

- **色サポートの検出結果を上書きする:**
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **Unicode のブロック文字を無効化し、ASCII フォールバックを使う:**
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 / レガシーコンソール:** Windows 10 より前のレガシーな `conhost` は仮想ターミナルエスケープシーケンス（`ENABLE_VIRTUAL_TERMINAL_PROCESSING`）に対応していません。Windows Terminal など、モダンな VT100 互換ターミナルを使用してください。

---

## FTXUI v6 から v7 への移行

FTXUI v7 では分かりやすさのために3つの主要クラスがリネームされました（後方互換性のため非推奨エイリアスも残されています）。

| FTXUI v6 (非推奨) | FTXUI v7 | ヘッダ |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
