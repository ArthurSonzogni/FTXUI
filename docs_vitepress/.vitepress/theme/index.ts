import DefaultTheme from 'vitepress/theme'
import { withBase } from 'vitepress'
import Layout from './Layout.vue'
import FtxuiDemo from './FtxuiDemo.vue'
import './custom.css'

export default {
  extends: DefaultTheme,
  Layout,
  enhanceApp({ app }) {
    app.component('FtxuiDemo', FtxuiDemo)
    if (typeof window !== 'undefined') {
      const params = new URLSearchParams(window.location.search)
      const file = params.get('file')
      if (file && /\/examples\/?$/.test(window.location.pathname)) {
        window.location.replace(withBase(`/demo/embed.html?file=${encodeURIComponent(file)}`))
      }
    }
  },
}
