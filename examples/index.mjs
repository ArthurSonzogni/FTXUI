import xterm             from 'https://cdn.jsdelivr.net/npm/xterm@4.18.0/+esm'
import xterm_addon_webgl from 'https://cdn.jsdelivr.net/npm/xterm-addon-webgl@0.11.4/+esm'
import xterm_addon_fit   from 'https://cdn.jsdelivr.net/npm/xterm-addon-fit@0.5.0/+esm'

// Add COOP/COEP via a ServiceWorker to use SharedArrayBuffer
if ("serviceWorker" in navigator && !window.crossOriginIsolated) {
  const url_sw = new URL("./sw.js", location.href);
  const registration = await navigator.serviceWorker.register(url_sw);
  window.location.reload(); // Reload to ensure the COOP/COEP headers are set.
} 

const example_list = "component/button;component/button_animated;component/button_in_frame;component/button_style;component/canvas_animated;component/checkbox;component/checkbox_in_frame;component/collapsible;component/composition;component/custom_loop;component/dropdown;component/dropdown_custom;component/flexbox_gallery;component/focus;component/focus_cursor;component/gallery;component/homescreen;component/input;component/input_style;component/linear_gradient_gallery;component/maybe;component/menu;component/menu2;component/menu_entries;component/menu_entries_animated;component/menu_in_frame;component/menu_in_frame_horizontal;component/menu_multiple;component/menu_style;component/menu_underline_animated_gallery;component/modal_dialog;component/modal_dialog_custom;component/nested_screen;component/print_key_press;component/radiobox;component/radiobox_in_frame;component/renderer;component/resizable_split;component/selection;component/scrollbar;component/slider;component/slider_direction;component/slider_rgb;component/tab_horizontal;component/tab_vertical;component/textarea;component/toggle;component/window;component/with_restored_io;dom/border;dom/border_colored;dom/border_style;dom/canvas;dom/color_gallery;dom/color_info_palette256;dom/color_truecolor_HSV;dom/color_truecolor_RGB;dom/dbox;dom/gauge;dom/gauge_direction;dom/graph;dom/gridbox;dom/hflow;dom/html_like;dom/linear_gradient;dom/package_manager;dom/paragraph;dom/separator;dom/separator_style;dom/size;dom/spinner;dom/style_blink;dom/style_bold;dom/style_color;dom/style_dim;dom/style_gallery;dom/style_hyperlink;dom/style_inverted;dom/style_strikethrough;dom/style_underlined;dom/style_underlined_double;dom/table;dom/vbox_hbox;dom/vflow".split(";");
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
