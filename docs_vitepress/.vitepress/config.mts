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

      // Skip comments (usually italicized or starting with // or /*)
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

const guideSidebar = [
  {
    text: 'Guide',
    items: [
      { text: 'Getting Started', link: '/guide/getting-started' },
      { text: 'Mental Model', link: '/guide/mental-model' },
      { text: 'Cookbook', link: '/guide/cookbook' },
      { text: 'Cheat Sheet', link: '/guide/cheatsheet' },
      { text: 'Events, Focus & Mouse', link: '/guide/events-and-focus' },
      { text: 'Custom Widgets', link: '/guide/custom-widgets' },
      { text: 'Troubleshooting & FAQ', link: '/guide/troubleshooting' },
      { text: 'Screen Module', link: '/guide/screen' },
      { text: 'DOM Module', link: '/guide/dom' },
      { text: 'Component Module', link: '/guide/component' },
      { text: 'C++20 Modules', link: '/guide/cpp20-modules' },
      { text: 'POSIX Pipe Support', link: '/guide/posix-pipe' },
    ],
  },
  {
    text: 'Installation',
    collapsed: false,
    items: [
      { text: 'Overview', link: '/install/' },
      { text: 'Build Systems', link: '/install/build-systems' },
      { text: 'Package Managers', link: '/install/package-managers' },
      { text: 'Linux Distros & Single Header', link: '/install/distros-and-header' },
    ],
  },
]

function ftxuiWasmExamplesPlugin() {
  const attachMiddleware = (middlewares: any) => {
    // Serve locally built ASYNCIFY WASM examples from build-wasm/examples/ first,
    // falling back to GitHub Pages for examples not built locally.
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

const LANG_LABELS: Record<string, string> = {
  af: 'Afrikaans',
  ar: 'العربية',
  bg: 'Български',
  ca: 'Català',
  cs: 'Čeština',
  da: 'Dansk',
  de: 'Deutsch',
  el: 'Ελληνικά',
  eo: 'Esperanto',
  es: 'Español',
  fi: 'Suomi',
  fr: 'Français',
  hi: 'हिन्दी',
  hr: 'Hrvatski',
  hu: 'Magyar',
  hy: 'Հայերեն',
  id: 'Bahasa Indonesia',
  it: 'Italiano',
  ja: '日本語',
  ko: '한국어',
  lt: 'Lietuvių',
  lv: 'Latviešu',
  mk: 'Македонски',
  nl: 'Nederlands',
  no: 'Norsk',
  pl: 'Polski',
  pt: 'Português',
  ro: 'Română',
  ru: 'Русский',
  sk: 'Slovenčina',
  sl: 'Slovenščina',
  sr: 'Српски',
  sv: 'Svenska',
  tr: 'Türkçe',
  uk: 'Українська',
  vi: 'Tiếng Việt',
  'zh-CH': '中文 (简体)',
  'zh-TW': '中文 (繁體)',
}

// Auto-detect any translated VitePress locale folders placed alongside English.
const detectedLocales: Record<string, any> = {}
for (const [code, label] of Object.entries(LANG_LABELS)) {
  const dir = path.resolve(process.cwd(), code)
  if (fs.existsSync(dir) && fs.statSync(dir).isDirectory()) {
    detectedLocales[code] = {
      label,
      lang: code,
      dir: code === 'ar' ? 'rtl' : 'ltr',
    }
  }
}

const localesConfig =
  Object.keys(detectedLocales).length > 0
    ? {
        root: { label: 'English', lang: 'en' },
        ...detectedLocales,
      }
    : undefined

const currentVersion = process.env.VITEPRESS_VERSION || 'main'
const versionItems: Array<{ text: string; link: string }> = process.env.VITEPRESS_VERSIONS
  ? JSON.parse(process.env.VITEPRESS_VERSIONS)
  : [
      { text: 'main (latest)', link: '/' },
      { text: 'v7.0.3 (Doxygen archive)', link: 'https://arthursonzogni.github.io/FTXUI/en/v7.0.3/' },
      { text: 'v6.1.9 (Doxygen archive)', link: 'https://arthursonzogni.github.io/FTXUI/en/v6.1.9/' },
    ]

const externalLangItems: Array<{ text: string; link: string }> = process.env.VITEPRESS_LANGS
  ? JSON.parse(process.env.VITEPRESS_LANGS)
  : []

const navItems: any[] = [
  { text: 'Guide', link: '/guide/getting-started' },
  { text: 'Installation', link: '/install/' },
  { text: 'API Reference', link: '/api/' },
  { text: 'Examples', link: '/examples/' },
  {
    text: currentVersion === 'main' ? 'main (latest)' : currentVersion,
    items: versionItems,
  },
]

if (!localesConfig && externalLangItems.length > 0) {
  navItems.push({
    text: 'Languages',
    items: externalLangItems,
  })
}

export default defineConfig({
  base: siteBase,
  title: 'FTXUI',
  description: 'C++ Functional Terminal User Interface',
  cleanUrls: true,
  lastUpdated: true,
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: withSiteBase('/favicon.svg') }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: 'FTXUI' }],
    ['meta', { property: 'og:title', content: 'FTXUI: C++ Functional Terminal User Interface' }],
    ['meta', { property: 'og:description', content: 'A simple, cross-platform C++17 library for building rich interactive terminal user interfaces with zero dependencies.' }],
    ['meta', { property: 'og:url', content: 'https://arthursonzogni.github.io/FTXUI/' }],
    ['meta', { name: 'twitter:card', content: 'summary' }],
  ],
  ...(localesConfig ? { locales: localesConfig } : {}),

  vite: {
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
            // Avoid nested <a> tags when inline code is already inside a Markdown link.
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
    outline: {
      level: [2, 3],
      label: 'On this page',
    },

    search: {
      provider: 'local',
    },

    nav: navItems,

    sidebar: {
      '/guide/': guideSidebar,
      '/install/': guideSidebar,
      '/api/': apiSidebar,
      '/examples/': examplesSidebar,
    },

    editLink: {
      pattern: 'https://github.com/ArthurSonzogni/FTXUI/edit/main/docs_vitepress/:path',
      text: 'Edit this page on GitHub',
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/ArthurSonzogni/FTXUI' },
    ],
  },
})
