import fs from 'node:fs'
import path from 'node:path'
import { defineConfig } from 'vitepress'
import apiSidebar from './api_sidebar.json'
import apiSymbols from './api_symbols.json'
import examplesSidebar from './examples_sidebar.json'

const rawBase = process.env.VITEPRESS_BASE || '/'
const siteBase = rawBase.endsWith('/') ? rawBase : `${rawBase}/`
const withSiteBase = (url: string) =>
  url.startsWith('/') ? `${siteBase.replace(/\/$/, '')}${url}` : url

const symbolMap: Record<string, string> = apiSymbols.symbols
const headerMap: Record<string, string> = apiSymbols.headers

function ftxuiCodeLinkTransformer() {
  return {
    name: 'ftxui-code-links',
    span(hast: any) {
      if (this.options.lang !== 'cpp' && this.options.lang !== 'c++') return
      if (!hast.children || hast.children.length !== 1) return
      const child = hast.children[0]
      if (child.type !== 'text' || typeof child.value !== 'string') return

      const text: string = child.value

      // Skip comments
      if (text.trimStart().startsWith('//') || text.trimStart().startsWith('/*')) return

      // 1. Check for <ftxui/...hpp> or "ftxui/...hpp" header includes
      const hdrMatch = text.match(/^(["<])(ftxui\/[a-zA-Z0-9_/]+\.hpp)([">])$/)
      if (hdrMatch && headerMap[hdrMatch[2]]) {
        hast.children = [
          { type: 'text', value: hdrMatch[1] },
          {
            type: 'element',
            tagName: 'a',
            properties: { href: withSiteBase(headerMap[hdrMatch[2]]), class: 'code-ref' },
            children: [{ type: 'text', value: hdrMatch[2] }],
          },
          { type: 'text', value: hdrMatch[3] },
        ]
        return
      }

      // 2. Skip other string literals
      const trimmed = text.trim()
      if ((trimmed.startsWith('"') && trimmed.endsWith('"')) || (trimmed.startsWith("'") && trimmed.endsWith("'"))) {
        return
      }

      // 3. Split identifiers and link known FTXUI symbols
      const parts = text.split(/(\b[A-Za-z_][A-Za-z0-9_]*\b)/)
      let changed = false
      const newChildren: any[] = []
      for (const part of parts) {
        if (!part) continue
        const url = Object.prototype.hasOwnProperty.call(symbolMap, part) ? symbolMap[part] : undefined
        if (url) {
          changed = true
          newChildren.push({
            type: 'element',
            tagName: 'a',
            properties: { href: withSiteBase(url), class: 'code-ref' },
            children: [{ type: 'text', value: part }],
          })
        } else {
          newChildren.push({ type: 'text', value: part })
        }
      }
      if (changed) {
        hast.children = newChildren
      }
    },
  }
}

function ftxuiWasmExamplesPlugin() {
  const attachMiddleware = (middlewares: any) => {
    const localWasmDir = path.resolve(process.cwd(), '../build-wasm/examples')
    middlewares.use('/wasm-examples', async (req: any, res: any, next: any) => {
      try {
        const cleanPath = (req.url || '/').split('?')[0]
        const localFile = path.join(localWasmDir, cleanPath)
        if (fs.existsSync(localFile) && fs.statSync(localFile).isFile()) {
          res.statusCode = 200
          if (localFile.endsWith('.wasm')) res.setHeader('Content-Type', 'application/wasm')
          else if (localFile.endsWith('.js') || localFile.endsWith('.mjs')) res.setHeader('Content-Type', 'application/javascript; charset=utf-8')
          res.end(fs.readFileSync(localFile))
          return
        }
        const upstreamUrl = `https://arthursonzogni.github.io/FTXUI/examples${cleanPath}`
        const upstream = await fetch(upstreamUrl)
        res.statusCode = upstream.status
        const ct = upstream.headers.get('content-type')
        if (ct) res.setHeader('Content-Type', ct)
        const buf = Buffer.from(await upstream.arrayBuffer())
        res.end(buf)
      } catch (err) {
        next(err)
      }
    })
  }

  return {
    name: 'ftxui-wasm-examples',
    configureServer(server: any) {
      attachMiddleware(server.middlewares)
    },
    configurePreviewServer(server: any) {
      attachMiddleware(server.middlewares)
    },
  }
}

const currentVersion = process.env.VITEPRESS_VERSION || 'main'
const singleLocale = process.env.VITEPRESS_SINGLE_LOCALE === '1'
const ignoreDeadLinks = process.env.VITEPRESS_IGNORE_DEAD_LINKS === '1'

const versionItems: Array<{ text: string; link: string; target?: string }> = process.env.VITEPRESS_VERSIONS
  ? JSON.parse(process.env.VITEPRESS_VERSIONS)
  : [
      { text: 'main (latest)', link: '/' },
      { text: 'v7.0.3', link: 'https://arthursonzogni.github.io/FTXUI/en/v7.0.3/', target: '_self' },
      { text: 'v6.1.9', link: 'https://arthursonzogni.github.io/FTXUI/en/v6.1.9/', target: '_self' },
    ]

const versionNavItem = {
  text: currentVersion === 'main' ? 'main (latest)' : currentVersion,
  items: versionItems,
}

interface LocaleStrings {
  label: string
  lang: string
  description: string
  nav: { guide: string; install: string; api: string; examples: string }
  sidebar: {
    guide: string
    gettingStarted: string
    mentalModel: string
    cookbook: string
    cheatsheet: string
    eventsFocus: string
    customWidgets: string
    troubleshooting: string
    screen: string
    dom: string
    component: string
    cpp20Modules: string
    posixPipe: string
    install: string
    overview: string
    buildSystems: string
    packageManagers: string
    distrosHeader: string
  }
  outline: string
  editText: string
  prev: string
  next: string
  returnToTop: string
  sidebarMenu: string
  darkMode: string
}

function makeGuideSidebar(prefix: string, s: LocaleStrings['sidebar']) {
  const p = prefix ? `/${prefix}` : ''
  return [
    {
      text: s.guide,
      items: [
        { text: s.gettingStarted, link: `${p}/guide/getting-started` },
        { text: s.mentalModel, link: `${p}/guide/mental-model` },
        { text: s.cookbook, link: `${p}/guide/cookbook` },
        { text: s.cheatsheet, link: `${p}/guide/cheatsheet` },
        { text: s.eventsFocus, link: `${p}/guide/events-and-focus` },
        { text: s.customWidgets, link: `${p}/guide/custom-widgets` },
        { text: s.troubleshooting, link: `${p}/guide/troubleshooting` },
        { text: s.screen, link: `${p}/guide/screen` },
        { text: s.dom, link: `${p}/guide/dom` },
        { text: s.component, link: `${p}/guide/component` },
        { text: s.cpp20Modules, link: `${p}/guide/cpp20-modules` },
        { text: s.posixPipe, link: `${p}/guide/posix-pipe` },
      ],
    },
    {
      text: s.install,
      collapsed: false,
      items: [
        { text: s.overview, link: `${p}/install/` },
        { text: s.buildSystems, link: `${p}/install/build-systems` },
        { text: s.packageManagers, link: `${p}/install/package-managers` },
        { text: s.distrosHeader, link: `${p}/install/distros-and-header` },
      ],
    },
  ]
}

function makeLocaleEntry(prefix: string, s: LocaleStrings) {
  const p = prefix ? `/${prefix}` : ''
  const guide = makeGuideSidebar(prefix, s.sidebar)
  return {
    label: s.label,
    lang: s.lang,
    description: s.description,
    themeConfig: {
      nav: [
        { text: s.nav.guide, link: `${p}/guide/getting-started` },
        { text: s.nav.install, link: `${p}/install/` },
        { text: s.nav.api, link: '/api/' },
        { text: s.nav.examples, link: '/examples/' },
        versionNavItem,
      ],
      sidebar: {
        [`${p}/guide/`]: guide,
        [`${p}/install/`]: guide,
        '/api/': apiSidebar,
        '/examples/': examplesSidebar,
      },
      outline: {
        level: [2, 3] as [number, number],
        label: s.outline,
      },
      docFooter: {
        prev: s.prev,
        next: s.next,
      },
      returnToTopLabel: s.returnToTop,
      sidebarMenuLabel: s.sidebarMenu,
      darkModeSwitchLabel: s.darkMode,
      editLink: {
        pattern: 'https://github.com/ArthurSonzogni/FTXUI/edit/main/docs_vitepress/:path',
        text: s.editText,
      },
    },
  }
}

const EN_STRINGS: LocaleStrings = {
  label: 'English',
  lang: 'en',
  description: 'C++ Functional Terminal User Interface',
  nav: { guide: 'Guide', install: 'Installation', api: 'API Reference', examples: 'Examples' },
  sidebar: {
    guide: 'Guide',
    gettingStarted: 'Getting Started',
    mentalModel: 'Mental Model',
    cookbook: 'Cookbook',
    cheatsheet: 'Cheat Sheet',
    eventsFocus: 'Events, Focus & Mouse',
    customWidgets: 'Custom Widgets',
    troubleshooting: 'Troubleshooting & FAQ',
    screen: 'Screen Module',
    dom: 'DOM Module',
    component: 'Component Module',
    cpp20Modules: 'C++20 Modules',
    posixPipe: 'POSIX Pipe Support',
    install: 'Installation',
    overview: 'Overview',
    buildSystems: 'Build Systems',
    packageManagers: 'Package Managers',
    distrosHeader: 'Linux Distros & Single Header',
  },
  outline: 'On this page',
  editText: 'Edit this page on GitHub',
  prev: 'Previous page',
  next: 'Next page',
  returnToTop: 'Return to top',
  sidebarMenu: 'Menu',
  darkMode: 'Appearance',
}

const TRANSLATED_LOCALES: Record<string, LocaleStrings> = {
  fr: {
    label: 'Français',
    lang: 'fr',
    description: 'Interface utilisateur terminal fonctionnelle en C++',
    nav: { guide: 'Guide', install: 'Installation', api: 'Référence API', examples: 'Exemples' },
    sidebar: {
      guide: 'Guide',
      gettingStarted: 'Démarrage rapide',
      mentalModel: 'Modèle mental',
      cookbook: 'Recettes',
      cheatsheet: 'Aide-mémoire',
      eventsFocus: 'Événements, focus et souris',
      customWidgets: 'Widgets personnalisés',
      troubleshooting: 'Dépannage et FAQ',
      screen: 'Module Screen',
      dom: 'Module DOM',
      component: 'Module Component',
      cpp20Modules: 'Modules C++20',
      posixPipe: 'Support des tubes POSIX',
      install: 'Installation',
      overview: 'Vue d\'ensemble',
      buildSystems: 'Systèmes de compilation',
      packageManagers: 'Gestionnaires de paquets',
      distrosHeader: 'Distributions Linux et en-tête unique',
    },
    outline: 'Sur cette page',
    editText: 'Modifier cette page sur GitHub',
    prev: 'Page précédente',
    next: 'Page suivante',
    returnToTop: 'Retour en haut',
    sidebarMenu: 'Menu',
    darkMode: 'Apparence',
  },
  es: {
    label: 'Español',
    lang: 'es',
    description: 'Interfaz de usuario funcional para terminal en C++',
    nav: { guide: 'Guía', install: 'Instalación', api: 'Referencia API', examples: 'Ejemplos' },
    sidebar: {
      guide: 'Guía',
      gettingStarted: 'Primeros pasos',
      mentalModel: 'Modelo mental',
      cookbook: 'Recetario',
      cheatsheet: 'Hoja de referencia',
      eventsFocus: 'Eventos, foco y ratón',
      customWidgets: 'Widgets personalizados',
      troubleshooting: 'Solución de problemas y FAQ',
      screen: 'Módulo Screen',
      dom: 'Módulo DOM',
      component: 'Módulo Component',
      cpp20Modules: 'Módulos C++20',
      posixPipe: 'Soporte de tuberías POSIX',
      install: 'Instalación',
      overview: 'Resumen',
      buildSystems: 'Sistemas de compilación',
      packageManagers: 'Gestores de paquetes',
      distrosHeader: 'Distribuciones Linux y cabecera única',
    },
    outline: 'En esta página',
    editText: 'Editar esta página en GitHub',
    prev: 'Página anterior',
    next: 'Página siguiente',
    returnToTop: 'Volver arriba',
    sidebarMenu: 'Menú',
    darkMode: 'Apariencia',
  },
  ja: {
    label: '日本語',
    lang: 'ja',
    description: 'C++ 関数型ターミナルユーザーインターフェースライブラリ',
    nav: { guide: 'ガイド', install: 'インストール', api: 'API リファレンス', examples: 'サンプル' },
    sidebar: {
      guide: 'ガイド',
      gettingStarted: 'はじめに',
      mentalModel: '基本コンセプト',
      cookbook: 'クックブック',
      cheatsheet: 'チートシート',
      eventsFocus: 'イベント・フォーカス・マウス',
      customWidgets: 'カスタムウィジェット',
      troubleshooting: 'トラブルシューティングと FAQ',
      screen: 'Screen モジュール',
      dom: 'DOM モジュール',
      component: 'Component モジュール',
      cpp20Modules: 'C++20 モジュール',
      posixPipe: 'POSIX パイプ対応',
      install: 'インストール',
      overview: '概要',
      buildSystems: 'ビルドシステム',
      packageManagers: 'パッケージマネージャー',
      distrosHeader: 'Linux ディストリビューションと単一ヘッダー',
    },
    outline: 'このページの内容',
    editText: 'GitHub でこのページを編集',
    prev: '前のページ',
    next: '次のページ',
    returnToTop: 'トップへ戻る',
    sidebarMenu: 'メニュー',
    darkMode: '外観',
  },
  'zh-CN': {
    label: '中文 (简体)',
    lang: 'zh-CN',
    description: 'C++ 函数式终端用户界面库',
    nav: { guide: '指南', install: '安装', api: 'API 参考', examples: '示例' },
    sidebar: {
      guide: '指南',
      gettingStarted: '快速开始',
      mentalModel: '核心概念',
      cookbook: '实用示例',
      cheatsheet: '速查表',
      eventsFocus: '事件、焦点与鼠标',
      customWidgets: '自定义组件',
      troubleshooting: '常见问题与排错',
      screen: 'Screen 模块',
      dom: 'DOM 模块',
      component: 'Component 模块',
      cpp20Modules: 'C++20 模块',
      posixPipe: 'POSIX 管道支持',
      install: '安装',
      overview: '概览',
      buildSystems: '构建系统',
      packageManagers: '包管理器',
      distrosHeader: 'Linux 发行版与单头文件',
    },
    outline: '本页目录',
    editText: '在 GitHub 上编辑此页',
    prev: '上一页',
    next: '下一页',
    returnToTop: '返回顶部',
    sidebarMenu: '菜单',
    darkMode: '外观',
  },
  'zh-TW': {
    label: '中文 (繁體)',
    lang: 'zh-TW',
    description: 'C++ 函數式終端機使用者介面函式庫',
    nav: { guide: '指南', install: '安裝', api: 'API 參考', examples: '範例' },
    sidebar: {
      guide: '指南',
      gettingStarted: '快速開始',
      mentalModel: '核心概念',
      cookbook: '實用範例',
      cheatsheet: '速查表',
      eventsFocus: '事件、焦點與滑鼠',
      customWidgets: '自訂元件',
      troubleshooting: '常見問題與排錯',
      screen: 'Screen 模組',
      dom: 'DOM 模組',
      component: 'Component 模組',
      cpp20Modules: 'C++20 模組',
      posixPipe: 'POSIX 管線支援',
      install: '安裝',
      overview: '概覽',
      buildSystems: '建置系統',
      packageManagers: '套件管理器',
      distrosHeader: 'Linux 發行版與單一標頭檔',
    },
    outline: '本頁目錄',
    editText: '在 GitHub 上編輯此頁',
    prev: '上一頁',
    next: '下一頁',
    returnToTop: '返回頂部',
    sidebarMenu: '選單',
    darkMode: '外觀',
  },
}

const activeLocales: Record<string, any> = {}
if (!singleLocale) {
  for (const [code, strings] of Object.entries(TRANSLATED_LOCALES)) {
    const dir = path.resolve(process.cwd(), code)
    if (fs.existsSync(dir) && fs.statSync(dir).isDirectory()) {
      activeLocales[code] = makeLocaleEntry(code, strings)
    }
  }
}

const hasMultipleLocales = Object.keys(activeLocales).length > 0
const rootLocale = makeLocaleEntry('', EN_STRINGS)

const srcExclude = singleLocale
  ? Object.keys(TRANSLATED_LOCALES).map((code) => `${code}/**`)
  : []

export default defineConfig({
  base: siteBase,
  title: 'FTXUI',
  description: EN_STRINGS.description,
  cleanUrls: true,
  lastUpdated: true,
  ignoreDeadLinks,
  srcExclude,
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: withSiteBase('/favicon.svg') }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'FTXUI' }],
    ['meta', { property: 'og:title', content: 'FTXUI: C++ Functional Terminal User Interface' }],
    ['meta', { property: 'og:description', content: 'A simple, cross-platform C++17 library for building rich interactive terminal user interfaces with zero dependencies.' }],
    ['meta', { property: 'og:url', content: 'https://arthursonzogni.github.io/FTXUI/' }],
    ['meta', { name: 'twitter:card', content: 'summary' }],
  ],

  ...(hasMultipleLocales
    ? {
        locales: {
          root: rootLocale,
          ...activeLocales,
        },
      }
    : {}),

  vite: {
    define: {
      __FTXUI_VERSION__: JSON.stringify(currentVersion),
    },
    plugins: [ftxuiWasmExamplesPlugin()],
  },

  markdown: {
    codeTransformers: [ftxuiCodeLinkTransformer()],
    config(md) {
      const defaultCodeInline = md.renderer.rules.code_inline!
      const nonCppPrefix = /^(?:\/|\.\/|\.\.\/|https?:\/\/|-|\$\s|cmake\b|git\b|npm\b|make\b|ninja\b|ctest\b|sudo\b|apt\b|brew\b|vcpkg\b|conan\b|pacman\b|nix\b|xmake\b|meson\b|bazel\b|emcmake\b|python3?\b|g\+\+\b|clang\+\+\b|docker\b)/i
      const plainFilename = /^[A-Za-z0-9_.\-\/]+\.(?:txt|md|json|yaml|yml|toml|cmake|bazel|bzl|build|in|js|mjs|ts|mts|css|html|wasm|sh|py|lock|xml|nix|cpp|hpp|h|c)$/

      const looksLikeCpp = (code: string): boolean => {
        const s = code.trim()
        if (!s || s.includes('\n') || s.length > 220) return false
        if (Object.prototype.hasOwnProperty.call(symbolMap, s)) return true
        if (Object.prototype.hasOwnProperty.call(headerMap, s)) return true
        if (nonCppPrefix.test(s)) return false
        if (plainFilename.test(s)) return false
        return (
          /[({};]|::|->|\s\|\s|\s=\s|^#include\b|^(?:auto|const|int|float|double|bool|void|struct|class|using|return|std::|ftxui::)\b/.test(s)
        )
      }

      md.renderer.rules.code_inline = (tokens, idx, options, env, self) => {
        const code = tokens[idx].content
        if (options.highlight && looksLikeCpp(code)) {
          const highlighted = options.highlight(code, 'cpp', '')
          const match = highlighted.match(/<span class="line">([\s\S]*?)<\/span><\/code>/)
          if (match) {
            let inner = match[1]
            const insideLink = idx > 0 && tokens[idx - 1].type === 'link_open'
            if (insideLink) {
              inner = inner.replace(/<a\b[^>]*>/g, '').replace(/<\/a>/g, '')
            }
            return `<code class="shiki-inline" v-pre>${inner}</code>`
          }
        }
        return defaultCodeInline(tokens, idx, options, env, self)
      }
    },
  },

  themeConfig: {
    search: {
      provider: 'local',
    },
    ...(hasMultipleLocales ? {} : rootLocale.themeConfig),
    socialLinks: [
      { icon: 'github', link: 'https://github.com/ArthurSonzogni/FTXUI' },
    ],
  },
})
