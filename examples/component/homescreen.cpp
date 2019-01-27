#include <cmath>
#include <thread>
#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

int shift = 0;
class Graph {
 public:
  std::vector<int> operator()(int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i) {
      float v = 0;
      v += 0.1 * sin((i + shift) * 0.1);
      v += 0.2 * sin((i + shift+10) * 0.15);
      v += 0.1 * sin((i + shift) * 0.03);
      v *= height;
      v += 0.5 * height;
      output[i] = v;
    }
    return output;
  }
};

class HTopComponent : public Component {
  Graph my_graph;

 public:
  HTopComponent() {}
  ~HTopComponent() override {}

  Element Render() override {
    return
      vbox(
        text(L"Frequency [Mhz]") | hcenter,
        hbox(
          vbox(
            text(L"2400 "), filler(),
            text(L"1200 "), filler(),
            text(L"0% ")
          ),
          graph(std::ref(my_graph))
        ) | flex,
        separator(),
        text(L"Utilization [%]") | hcenter,
        hbox(
          vbox(
            text(L"100 "), filler(),
            text(L"50 "), filler(),
            text(L"0 ")
          ),
          graph(std::ref(my_graph)) | color(Color::RedLight)
        ) | flex,
        separator(),
        text(L"Ram [Go]") | hcenter,
        hbox(
          vbox(
            text(L"8192"), filler(),
            text(L"4096 "), filler(),
            text(L"0 ")
          ),
          graph(std::ref(my_graph)) | color(Color::BlueLight)
        ) | flex
      ) | border;
  }
};

class CompilerComponent : public Component {
  Container container = Container::Horizontal();
    RadioBox compiler;
    Container flag = Container::Vertical();
      CheckBox flag_checkbox[4];
    Container subcontainer = Container::Vertical();
      Container input_container = Container::Horizontal();
        Input input_add;
        Menu input;
      Input executable;

 public:
  ~CompilerComponent() override {}
  CompilerComponent() {
    Add(&container);

    // Compiler ----------------------------------------------------------------
    compiler.entries = {
      L"gcc",
      L"clang",
      L"emcc",
      L"game_maker"
    };
    container.Add(&compiler);

    // Flags    ----------------------------------------------------------------
    container.Add(&flag);
    flag_checkbox[0].label = L"-Wall";
    flag_checkbox[1].label = L"-Werror";
    flag_checkbox[2].label = L"-lpthread";
    flag_checkbox[3].label = L"-O3";
    for(auto& c : flag_checkbox)
      flag.Add(&c);

    container.Add(&subcontainer);
    // Executable    ----------------------------------------------------------------
    executable.placeholder = L"executable";
    subcontainer.Add(&executable);

    // Input    ----------------------------------------------------------------
    subcontainer.Add(&input_container);

    input_add.placeholder = L"input files";
    input_add.on_enter = [this] {
      input.entries.push_back(input_add.content);
      input_add.content = L"";
    };
    input_container.Add(&input_add);
    input_container.Add(&input);
  }

  Element Render() override {
    return
      vbox(
        hbox(
          window(text(L"Compiler"), compiler.Render()),
          window(text(L"Flags"), flag.Render()),
          vbox(
            window(text(L"Executable:"), executable.Render())
              | size(WIDTH, EQUAL, 20),
            window(text(L"Input"),
              hbox(
                vbox(
                  hbox(text(L"Add: "), input_add.Render())
                    | size(WIDTH, EQUAL, 20)
                    | size(HEIGHT, EQUAL, 1),
                  filler()
                ),
                separator(),
                input.Render() | frame | size(HEIGHT, EQUAL, 3) | flex
              )
            ) | size(WIDTH, EQUAL, 60)
          ),
          filler()
        ),
        hflow(RenderCommandLine())
      ) | border;
  }

  Elements RenderCommandLine() {
    Elements line;
    // Compiler
    line.push_back(text(compiler.entries[compiler.selected]) | bold);
    // flags
    for(auto& it : flag_checkbox) {
      if (it.state) {
        line.push_back(text(L" "));
        line.push_back(text(it.label) | dim);
      }
    }
    // Executable
    if (!executable.content.empty()) {
      line.push_back(text(L" -O ") | bold);
      line.push_back(text(executable.content) | color(Color::BlueLight) | bold);
    }
    // Input
    for(auto& it : input.entries) {
      line.push_back(text(L" " + it) | color(Color::RedLight));
    }
    return line;
  }
};

class SpinnerComponent : public Component {
  Element Render() override {
    Elements entries;
    for(int i = 0; i<22; ++i) {
      if (i != 0)
      entries.push_back(
        spinner(i, shift/2)
          | bold
          | size(WIDTH, GREATER_THAN, 2)
          | border
      );
    }
    return hflow(std::move(entries)) | border;
  }
};

class Tab : public Component {
  public:
    Container main_container = Container::Vertical();

    Toggle tab_selection;
    Container container = Container::Tab(&tab_selection.selected);

    HTopComponent htop;
    CompilerComponent compiler;
    SpinnerComponent spinner_component;

    Tab() {
      Add(&main_container);
      main_container.Add(&tab_selection);
      tab_selection.entries = {
        L"compiler",
        L"htop",
        L"spinner"
      };
      main_container.Add(&container);
      container.Add(&compiler);
      container.Add(&htop);
      container.Add(&spinner_component);
    }

    Element Render() override {
      return vbox(
        text(L"FTXUI Demo") | bold | hcenter,
        tab_selection.Render() | hcenter,
        container.Render()
      );
    }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  std::thread update([&screen]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      shift++;
      screen.PostEvent(Event::Custom);
    }
  });

  Tab tab;
  screen.Loop(&tab);

  return 0;
}
