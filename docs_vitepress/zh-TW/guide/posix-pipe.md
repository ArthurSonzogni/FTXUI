# FTXUI 中的 POSIX 管線輸入

::: warning 平台支援
此功能僅適用於 Linux 與 macOS，不支援 Windows 或 WebAssembly。
:::

## 什麼是 POSIX 管線（Pipe）？

POSIX 管線讓兩個獨立的程式能夠互相傳遞資料：一個程式將輸出直接送給另一個程式當作輸入，就像一條單向的資料通道。

**範例：**

假設你想列出檔案清單，接著再以互動方式進行篩選：

- `ls`：列出檔案清單。
- `interactive_grep`：一個使用 FTXUI 開發的應用程式，用來篩選文字並讓你即時輸入條件。

你可以用管線符號（`|`）將兩者串接起來：

```bash
ls -l | interactive_grep
```

這段指令的執行流程如下：
1. `ls -l` 列出詳細的檔案清單。
2. `|` 將這份清單直接傳送給 `interactive_grep`。
3. `interactive_grep` 接收清單並顯示在畫面上。因為它是 FTXUI 應用程式，即使初始資料來自 `ls`，你依然可以直接透過鍵盤輸入來篩選清單。

## FTXUI 如何處理管線輸入

了解 POSIX 管線後，來看看 FTXUI 是怎麼處理的。

FTXUI 讓你的應用程式能夠從其他程式（例如透過管線）讀取資料，同時依然允許使用者用鍵盤互動。這對於需要處理串流資料的互動式命令列工具非常實用。

一般情況下，FTXUI 應用程式會從 `stdin` 接收所有輸入。不過，當 FTXUI 偵測到 `stdin` 連接到管線輸出（表示有資料正透過管線送入你的程式）時，它會自動切換為從 `/dev/tty` 讀取互動式鍵盤輸入。如此一來，應用程式在處理管線資料的同時，仍能正常接收使用者操作。

這項功能**預設為開啟**。

如果你的 FTXUI 應用程式需要讀取管線資料並回應鍵盤輸入，通常不需要額外設定任何東西：

```cpp
auto app = App::Fullscreen();
// app.HandlePipedInput(true); // Enabled by default
app.Loop(component);
```

## 關閉管線輸入處理

如果你不需要這項功能，或者它與你自訂的輸入處理機制衝突，可以在啟動主迴圈之前呼叫 `HandlePipedInput(false)`：

```cpp
auto app = App::Fullscreen();
app.HandlePipedInput(false);
app.Loop(component);
```
