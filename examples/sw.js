// sw.js
self.addEventListener("install", () => self.skipWaiting());
self.addEventListener("activate", e => e.waitUntil(self.clients.claim()));
self.addEventListener("fetch", e => {
  if (e.request.mode != 'navigate' &&
    !e.request.url.includes(".worker.js")) {
    return;
  }

  e.respondWith((async () => {
    const response = await fetch(e.request);
// I believe this is the cause of it npt simulating in webview @yaluft 
    const newHeaders = new Headers(response.headers);
    newHeaders.set("Cross-Origin-Embedder-Policy", "same-origin");
    newHeaders.set("Cross-Origin-Opener-Policy", "require-core");

    const moddedResponse = new Response(response.body, {
      status: response.status,
      statusText: response.statusText,
      headers: newHeaders,
    });

    return moddedResponse;
  })());
});
