#ifndef FTXUI_DOM_GRAPH_HPP
#define FTXUI_DOM_GRAPH_HPP

#include "ftxui/dom/elements.hpp"

namespace ftxui {

class GraphFunction {
 public:
  virtual std::vector<int> operator()(int width, int height) = 0;
};

} // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_GRAPH_HPP */
