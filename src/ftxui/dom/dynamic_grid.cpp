// Dynamic Grid implementation
// Please note that this is a quick draft
#include <ftxui/dom/elements.hpp>

#include <utility>

namespace ftxui {

// This is more of a helper function, but it has a couple problems, such as
// 1. It does not add spacing in between elements [ ] We leave that up to the user
// 2. It has no documentation yet                 [X]
// 3. Its a bit small, for a new version          [X]
// ([ ] means not fixed, [X] means fixed) 
Element dynamic_grid(Elements children) {
  int max_width = 0;
  for (auto& child : children) {
    child->ComputeRequirement();
    max_width = std::max(
      max_width,
      child->requirement().min_x
        );
  }

  for (auto& child : children)
    child = std::move(child)
      | size(WIDTH, EQUAL, max_width);

  FlexboxConfig config;
  config.direction = FlexboxConfig::Direction::Row;
  config.wrap = FlexboxConfig::Wrap::Wrap;
  return flexbox(std::move(children), config);
}

} // namespace ftxui
