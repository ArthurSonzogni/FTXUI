#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"
#include "ftxui/screen/screen.hpp"

namespace ftxui {

/// @brief Add a filter that will invert the foreground and the background
/// colors.
/// @ingroup dom
Element vscroll_indicator(Element child) {
  class Impl : public NodeDecorator {
    using NodeDecorator::NodeDecorator;

    void Render(Screen& screen) final {
      Node::Render(screen);

      const Box& stencil = screen.stencil;

      float size_inner = box_.y_max - box_.y_min;
      float size_outter = stencil.y_max - stencil.y_min;
      float start_y = stencil.y_min +
                      (stencil.y_min - box_.y_min) * size_outter / size_inner;
      float end_y = stencil.y_min +
                    (stencil.y_max - box_.y_min) * size_outter / size_inner;

      const int x = stencil.x_max;
      for (int y = stencil.y_min; y <= stencil.y_max; ++y) {
        bool up = (2 * y + -1 >= 2 * start_y) && (2 * y + -1 <= 2 * end_y);
        bool down = (2 * y + 0 >= 2 * start_y) && (2 * y + 0 <= 2 * end_y);

        if (up) {
          if (down) {
            screen.PixelAt(x, y).character = "┃";
          } else {
            screen.PixelAt(x, y).character = "╹";
          }
        } else {
          if (down) {
            screen.PixelAt(x, y).character = "╻";
          } else {
            screen.PixelAt(x, y).character = " ";
          }
        }
        screen.PixelAt(x,y).inverted = true;
      }
    };
  };
  return std::make_shared<Impl>(std::move(child));
}

}  // namespace ftxui
