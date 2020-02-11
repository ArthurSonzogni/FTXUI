#include "ftxui/dom/elements.hpp"

namespace ftxui {

const wchar_t charset[] = L" ▗▐▖▄▟▌▙█";

class Graph : public Node {
 public:
  Graph(GraphFunction graph_function) : graph_function_(graph_function) {}
  ~Graph() override {}

  void ComputeRequirement() override {
    requirement_.flex.x = 1;
    requirement_.flex.y = 1;
    requirement_.min.x = 1;
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    int width = (box_.x_max - box_.x_min + 1) * 2;
    int height = (box_.y_max - box_.y_min + 1) * 2;
    auto data = graph_function_(width, height);
    int i = 0;
    for (int x = box_.x_min; x <= box_.x_max; ++x) {
      int height_1 = 2 * box_.y_max - data[i++];
      int height_2 = 2 * box_.y_max - data[i++];
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        int yy = 2 * y;
        int i_1 = yy < height_1 ? 0 : yy == height_1 ? 3 : 6;
        int i_2 = yy < height_2 ? 0 : yy == height_2 ? 1 : 2;
        wchar_t pix = charset[i_1 + i_2];
        screen.at(x, y) = pix;
      }
    }
  }

 private:
  GraphFunction graph_function_;
};

std::unique_ptr<Node> graph(GraphFunction graph_function) {
  return std::make_unique<Graph>(graph_function);
}

}  // namespace ftxui
