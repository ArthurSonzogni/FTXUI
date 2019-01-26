#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/graph.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

class Graph : public ftxui::GraphFunction {
 public:
  std::vector<int> operator()(int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i) {
      float v = 0;
      v += 0.1 * sin((i + shift) * 0.1);
      v += 0.2 * sin((i + shift+10) * 0.15);
      v += 0.1 * sin((i + shift) * 0.03);
      // v += 0.2*sin((i+shift)*0.3);
      // v += 0.1*sin((i+shift)*0.9);
      v *= height;
      v += 0.5 * height;
      output[i] = v;
    }
    return output;
  }
  int shift = 0;
};

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  using namespace std::chrono_literals;

  Graph my_graph;

  std::string reset_position;
  for (int i = 0;; ++i) {
    auto document =
      window(text(L"Your graphs"),
        hbox(
          vbox(
            graph(my_graph), separator(),
            graph(my_graph) | inverted
          ) | flex,
          separator(),
          vbox(
            graph(my_graph) | color(Color::BlueLight), separator(),
            graph(my_graph) | color(Color::RedLight), separator(),
            graph(my_graph) | color(Color::YellowLight)
          ) | flex
        )
      ) | size(HEIGHT, GREATER_THAN, 40);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString() << std::flush;

    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.03s);
    my_graph.shift++;
  }

  return 0;
}
