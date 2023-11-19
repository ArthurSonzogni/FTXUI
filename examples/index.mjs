import xterm             from 'https://cdn.jsdelivr.net/npm/xterm@4.18.0/+esm'
import xterm_addon_webgl from 'https://cdn.jsdelivr.net/npm/xterm-addon-webgl@0.11.4/+esm'
import xterm_addon_fit   from 'https://cdn.jsdelivr.net/npm/xterm-addon-fit@0.5.0/+esm'

// Add COOP/COEP via a ServiceWorker to use SharedArrayBuffer
if ("serviceWorker" in navigator && !window.crossOriginIsolated) {
  const url_sw = new URL("./sw.js", location.href);
  const registration = await navigator.serviceWorker.register(url_sw);
  if (registration.active && !navigator.serviceWorker.controller) {
    window.location.reload(); // Reload to ensure the COOP/COEP headers are set.
  }
} 

const example_list = "@EXAMPLES@".split(";");
const url_search_params = new URLSearchParams(window.location.search);

const select = document.getElementById("selectExample"); 
for(const example of example_list) {
  const option = document.createElement("option");
  option.textContent = example;
  option.value = example;
  select.appendChild(option);
}
const example = url_search_params.get("file") || "dom/color_gallery";
select.selectedIndex = example_list.findIndex(path => path == example) || 0;
select.addEventListener("change", () => {
  history.pushState({}, "", "?file=" + example_list[select.selectedIndex]);
  location.reload();
});

const term_element = document.querySelector('#terminal');
const term = new xterm.Terminal();
term.options.scrollback = 0;
term.open(term_element);
const fit_addon = new xterm_addon_fit.FitAddon();
const webgl_addon = new xterm_addon_webgl.WebglAddon();
term.loadAddon(webgl_addon);
term.loadAddon(fit_addon);

const stdin_buffer = [];
const stdout_buffer = [];
const stderr_buffer = [];

const stdin = () => {
  return stdin_buffer.shift() || 0;
}

const stdout = code => {
  if (code == 0) {
    term.write(new Uint8Array(stdout_buffer));
    stdout_buffer.length = 0;
  } else {
    stdout_buffer.push(code)
  }
}

const stderr = code => {
  if (code == 0 || code == 10) {
    console.error(String.fromCodePoint(...stderr_buffer));
    stderr_buffer = [];
  } else {
    stderr_buffer.push(code)
  }
}

const onBinary = e => {
  for(const c of e)
    stdin_buffer.push(c.charCodeAt(0));
}

term.onBinary(onBinary);
term.onData(onBinary)
term.resize(140,43);

window.Module = {
  preRun: () => {
    FS.init(stdin, stdout, stderr);
  },
  postRun: [],
  onRuntimeInitialized: () => {
    if (window.Module._ftxui_on_resize == undefined)
      return;
    fit_addon.fit();

    const resize_handler = () => {
      const {cols, rows} = fit_addon.proposeDimensions();
      term.resize(cols, rows);
      window.Module._ftxui_on_resize(cols, rows);
      fit_addon.fit();
    };
    const resize_observer = new ResizeObserver(resize_handler);
    resize_observer.observe(term_element);
    resize_handler();

    // Disable scrollbar
    //term.write('\x1b[?47h')
  },
};

const words = example.split('/')
words[1] = "ftxui_example_" + words[1] + ".js"
document.querySelector("#example_script").src = words.join('/');
