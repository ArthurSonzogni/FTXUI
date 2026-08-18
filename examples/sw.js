// sw.js
self.addEventListener("install", () => self.skipWaiting());
self.addEventListener("activate", e => e.waitUntil(self.clients.claim()));
self.addEventListener("fetch", e => {
  // A cross-origin isolated document requires its dedicated workers to be
  // served with the COEP header too. Emscripten no longer emits a separate
  // "*.worker.js": pthread workers are spawned from the main "*.js" file, so
  // every same-origin response must carry the headers, not just navigations.
  if (new URL(e.request.url).origin != self.location.origin) {
    return; // Cross-origin responses are opaque: they can't be rewritten.
  }

  // Chrome fails such requests when they are handled by a ServiceWorker.
  if (e.request.cache == "only-if-cached" && e.request.mode != "same-origin") {
    return;
  }

  e.respondWith((async () => {
    const response = await fetch(e.request);

    const newHeaders = new Headers(response.headers);
    newHeaders.set("Cross-Origin-Embedder-Policy", "require-corp");
    newHeaders.set("Cross-Origin-Opener-Policy", "same-origin");

    const moddedResponse = new Response(response.body, {
      status: response.status,
      statusText: response.statusText,
      headers: newHeaders,
    });

    return moddedResponse;
  })());
});
