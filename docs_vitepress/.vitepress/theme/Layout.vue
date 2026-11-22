<script setup lang="ts">
import DefaultTheme from 'vitepress/theme'

const { Layout } = DefaultTheme
// Injected by VitePress config (`vite.define`)
declare const __FTXUI_VERSION__: string
const version = typeof __FTXUI_VERSION__ !== 'undefined' ? __FTXUI_VERSION__ : 'main'
const isHistorical = version !== 'main'
</script>

<template>
  <Layout>
    <template #doc-before>
      <div v-if="isHistorical" class="version-banner">
        You are viewing documentation for <strong>{{ version }}</strong>.
        <a href="https://arthursonzogni.github.io/FTXUI/">Switch to latest (main)</a>.
      </div>
    </template>
  </Layout>
</template>

<style scoped>
.version-banner {
  margin-bottom: 20px;
  padding: 10px 14px;
  border-radius: 8px;
  border: 1px solid var(--vp-c-warning-2);
  background: var(--vp-c-warning-soft);
  color: var(--vp-c-text-1);
  font-size: 0.9rem;
}
.version-banner a {
  color: var(--vp-c-brand-1);
  font-weight: 600;
  text-decoration: underline;
}
</style>
