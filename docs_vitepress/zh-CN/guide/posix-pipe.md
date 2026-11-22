# POSIX 管道输入

::: warning 平台支持
此特性仅支持 Linux 和 macOS，不支持 Windows 和 WebAssembly。
:::

## 什么是 POSIX 管道？

POSIX 管道是两个独立程序之间通信的机制：一个程序的输出可以直接作为另一个程序的输入，就像一条单向传输数据的管道。

**示例：**

假设你想先列出目录中的文件，然后交互式地筛选它们：

- `ls`：列出文件。
- `interactive_grep`：一个基于 FTXUI 编写的交互式文本过滤程序，支持实时键入搜索。

你可以用管道符（`|`）把它们连起来：

```bash
ls -l | interactive_grep
```

执行流程如下：
1. `ls -l` 列出文件及其详细信息。
2. `|` 将这份输出直接传给 `interactive_grep`。
3. `interactive_grep` 接收列表并显示在终端中。由于它是 FTXUI 应用，即使初始数据来自 `ls` 的管道输入，你依然可以继续通过键盘输入来筛选列表。

## FTXUI 如何处理管道输入

了解了 POSIX 管道之后，再来看看 FTXUI 是如何支持它的。

FTXUI 允许你的应用一边从其他程序（例如管道）读取数据，一边正常响应用户的键盘交互，非常适合构建交互式命令行数据处理工具。

通常情况下，FTXUI 应用会从 `stdin` 读取所有输入。但当 FTXUI 检测到 `stdin` 连接到管道输出（即有数据正通过管道传入应用）时，它会自动切换为从 `/dev/tty` 读取交互式键盘输入。这样一来，程序既能读完管道中的数据，又能持续接收用户的键盘操作。

此特性**默认开启**。

如果你的 FTXUI 应用需要同时读取管道数据并响应键盘交互，通常无需做任何额外配置：

```cpp
auto app = App::Fullscreen();
// app.HandlePipedInput(true); // Enabled by default
app.Loop(component);
```

## 关闭管道输入处理

如果你不需要此特性，或者它与你的自定义输入处理逻辑冲突，可以在启动主循环之前调用 `HandlePipedInput(false)`：

```cpp
auto app = App::Fullscreen();
app.HandlePipedInput(false);
app.Loop(component);
```
