project('ftxui', 'cpp',
  version: run_command('python3', '-c', 'import re; print(re.search(r"project\(ftxui\s+.*?VERSION\s+([0-9.]+)", open("CMakeLists.txt").read(), re.DOTALL).group(1))', check: true).stdout().strip(),
  license: 'MIT',
  default_options: [
    'cpp_std=c++17',
    'buildtype=release',
    'default_library=static',
    'b_pie=true',
  ],
  meson_version: '>=1.1.0',
)

# Force Win32 to UNICODE
if host_machine.system() == 'windows'
  add_project_arguments('-DUNICODE', '-D_UNICODE', '-D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
  cpp = meson.get_compiler('cpp')
  if cpp.get_id() == 'msvc' or cpp.get_id() == 'clang-cl'
    add_project_arguments('/utf-8', '/wd4244', '/wd4267', language: 'cpp')
  endif
endif

# Options
ftxui_build_examples = get_option('examples')
ftxui_build_tests = get_option('tests')

# Include directories
ftxui_public_inc = include_directories('include')
ftxui_private_inc = include_directories('src', 'include')

# Subdirectories for sources
subdir('src/ftxui/screen')
subdir('src/ftxui/dom')
subdir('src/ftxui/component')

# --- screen library ---
ftxui_screen = library('ftxui-screen',
  screen_sources,
  include_directories: ftxui_private_inc,
  install: true,
)

ftxui_screen_dep = declare_dependency(
  include_directories: ftxui_public_inc,
  link_with: ftxui_screen,
)

# --- dom library ---
ftxui_dom = library('ftxui-dom',
  dom_sources,
  include_directories: ftxui_private_inc,
  link_with: ftxui_screen,
  install: true,
)

ftxui_dom_dep = declare_dependency(
  include_directories: ftxui_public_inc,
  link_with: [ftxui_dom, ftxui_screen],
)

# --- component library ---
ftxui_component = library('ftxui-component',
  component_sources,
  include_directories: ftxui_private_inc,
  link_with: [ftxui_dom, ftxui_screen],
  install: true,
)

ftxui_component_dep = declare_dependency(
  include_directories: ftxui_public_inc,
  link_with: [ftxui_component, ftxui_dom, ftxui_screen],
)

# Install headers
install_subdir('include/ftxui', install_dir: get_option('includedir'))

# --- Tests ---
if ftxui_build_tests
  # Try system gtest first, fallback to subproject (wrap)
  gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'], required: true)
  gtest_main_dep = dependency('gtest_main', fallback: ['gtest', 'gtest_main_dep'], required: true)

  test_sources = files(
    'src/ftxui/component/animation_test.cpp',
    'src/ftxui/component/button_test.cpp',
    'src/ftxui/component/collapsible_test.cpp',
    'src/ftxui/component/component_test.cpp',
    'src/ftxui/component/container_test.cpp',
    'src/ftxui/component/dropdown_test.cpp',
    'src/ftxui/component/hoverable_test.cpp',
    'src/ftxui/component/input_test.cpp',
    'src/ftxui/component/menu_test.cpp',
    'src/ftxui/component/modal_test.cpp',
    'src/ftxui/component/radiobox_test.cpp',
    'src/ftxui/component/resizable_split_test.cpp',
    'src/ftxui/component/app_test.cpp',
    'src/ftxui/component/slider_test.cpp',
    'src/ftxui/component/task_test.cpp',
    'src/ftxui/component/terminal_input_parser_test.cpp',
    'src/ftxui/component/toggle_test.cpp',
    'src/ftxui/dom/blink_test.cpp',
    'src/ftxui/dom/bold_test.cpp',
    'src/ftxui/dom/border_test.cpp',
    'src/ftxui/dom/canvas_test.cpp',
    'src/ftxui/dom/color_test.cpp',
    'src/ftxui/dom/dbox_test.cpp',
    'src/ftxui/dom/dim_test.cpp',
    'src/ftxui/dom/flexbox_helper_test.cpp',
    'src/ftxui/dom/flexbox_test.cpp',
    'src/ftxui/dom/gauge_test.cpp',
    'src/ftxui/dom/gridbox_test.cpp',
    'src/ftxui/dom/hbox_test.cpp',
    'src/ftxui/dom/hyperlink_test.cpp',
    'src/ftxui/dom/italic_test.cpp',
    'src/ftxui/dom/linear_gradient_test.cpp',
    'src/ftxui/dom/scroll_indicator_test.cpp',
    'src/ftxui/dom/selection_test.cpp',
    'src/ftxui/dom/separator_test.cpp',
    'src/ftxui/dom/spinner_test.cpp',
    'src/ftxui/dom/table_test.cpp',
    'src/ftxui/dom/text_test.cpp',
    'src/ftxui/dom/underlined_test.cpp',
    'src/ftxui/dom/vbox_test.cpp',
    'src/ftxui/screen/color_test.cpp',
    'src/ftxui/screen/compatibility_test.cpp',
    'src/ftxui/screen/string_test.cpp',
    'src/ftxui/util/ref_test.cpp',
  )

  ftxui_tests = executable('ftxui-tests',
    test_sources,
    include_directories: ftxui_private_inc,
    link_with: [ftxui_component, ftxui_dom, ftxui_screen],
    dependencies: [gtest_dep, gtest_main_dep],
    override_options: ['cpp_std=c++20'],
  )

  test('ftxui-tests', ftxui_tests)
endif

# --- Examples ---
if ftxui_build_examples
  subdir('examples')
endif
