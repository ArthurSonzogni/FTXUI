---
layout: home

hero:
  name: FTXUI
  text: C++ 関数型ターミナルユーザーインターフェース
  tagline: リッチなターミナルUIを構築するための、シンプル・クロスプラットフォーム・依存ゼロのC++ライブラリ。
  actions:
    - theme: brand
      text: はじめに
      link: /ja/guide/getting-started
    - theme: alt
      text: APIリファレンス
      link: /api/
    - theme: alt
      text: ライブサンプル
      link: /examples/

features:
  - title: 関数型 & 高い合成性
    details: hbox、vbox、flexbox、パイプ記法のデコレータを使って、レスポンシブなレイアウトを宣言的に構築できます。
  - title: インタラクティブなコンポーネント
    details: Button、Input、Menu、Slider、Modal、Window、ResizableSplit など、キーボードとマウスに対応したコンポーネントを標準搭載しています。
  - title: クロスプラットフォーム & 依存ゼロ
    details: Linux、macOS、Windows、WebAssembly で動作し、UTF-8、全角CJK文字、TrueColor に対応しています。
---

## FTXUI をブラウザで試す

以下の WebAssembly ターミナル内で、ボタンをクリックしたりキーボードで操作したりできます。

<FtxuiDemo file="component/button" title="Interactive Button Demo (WebAssembly)" height="260px" />
