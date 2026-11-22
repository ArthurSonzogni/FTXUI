---
layout: home

hero:
  name: FTXUI
  text: C++ 函数式终端用户界面库
  tagline: 简洁、跨平台、零依赖的 C++ 终端 UI 库，轻松构建丰富的命令行界面。
  actions:
    - theme: brand
      text: 快速开始
      link: /zh-CN/guide/getting-started
    - theme: alt
      text: API 参考
      link: /api/
    - theme: alt
      text: 在线示例
      link: /examples/

features:
  - title: 函数式与可组合
    details: 使用 hbox、vbox、flexbox 以及支持管道操作符的装饰器，以声明式方式构建响应式布局。
  - title: 交互式组件
    details: 内置 Button、Input、Menu、Slider、Modal、Window 和 ResizableSplit，全面支持键盘与鼠标操作。
  - title: 跨平台且零依赖
    details: 支持 Linux、macOS、Windows 和 WebAssembly，原生支持 UTF-8、全角中日韩字符及 TrueColor 真彩色。
---

## 在线体验 FTXUI

直接在下方的 WebAssembly 终端中点击按钮或使用键盘体验：

<FtxuiDemo file="component/button" title="Interactive Button Demo (WebAssembly)" height="260px" />
