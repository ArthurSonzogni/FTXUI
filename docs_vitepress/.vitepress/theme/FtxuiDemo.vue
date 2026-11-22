<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'

const props = withDefaults(defineProps<{
  file: string
  height?: string
  title?: string
}>(), {
  height: '320px',
})

const embedUrl = computed(
  () => withBase(`/demo/embed.html?file=${encodeURIComponent(props.file)}`)
)
const fullscreenUrl = computed(
  () => withBase(`/demo/embed.html?file=${encodeURIComponent(props.file)}`)
)
</script>

<template>
  <div class="ftxui-demo">
    <div class="ftxui-demo-bar">
      <span class="ftxui-demo-title">{{ title || file }}</span>
      <a class="ftxui-demo-link" :href="fullscreenUrl" target="_blank" rel="noopener">
        Open fullscreen ↗
      </a>
    </div>
    <iframe
      :src="embedUrl"
      :style="{ height }"
      loading="lazy"
      title="FTXUI Interactive Demo"
      class="ftxui-demo-frame"
    />
  </div>
</template>

<style scoped>
.ftxui-demo {
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  overflow: hidden;
  margin: 16px 0;
  background: #0d1117;
}
.ftxui-demo-bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 6px 12px;
  background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
  font-size: 0.85rem;
}
.ftxui-demo-title {
  font-family: var(--vp-font-family-mono);
  color: var(--vp-c-text-2);
}
.ftxui-demo-link {
  color: var(--vp-c-brand-1);
  text-decoration: none;
  font-weight: 500;
}
.ftxui-demo-link:hover {
  text-decoration: underline;
}
.ftxui-demo-frame {
  width: 100%;
  border: 0;
  display: block;
  resize: vertical;
}
</style>
