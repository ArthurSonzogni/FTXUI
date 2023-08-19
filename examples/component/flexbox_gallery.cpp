// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stddef.h>  // for size_t
#include <memory>    // for shared_ptr, __shared_ptr_access, allocator
#include <string>  // for string, basic_string, to_string, operator+, char_traits
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Vertical, Checkbox, Horizontal, Renderer, ResizableSplitBottom, ResizableSplitRight
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, window, operator|, vbox, hbox, Element, flexbox, bgcolor, filler, flex, size, border, hcenter, color, EQUAL, bold, dim, notflex, xflex_grow, yflex_grow, HEIGHT, WIDTH
#include "ftxui/dom/flexbox_config.hpp"  // for FlexboxConfig, FlexboxConfig::AlignContent, FlexboxConfig::JustifyContent, FlexboxConfig::AlignContent::Center, FlexboxConfig::AlignItems, FlexboxConfig::Direction, FlexboxConfig::JustifyContent::Center, FlexboxConfig::Wrap
#include "ftxui/screen/color.hpp"        // for Color, Color::Black

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  int direction_index = 0;
  int wrap_index = 0;
  int justify_content_index = 0;
  int align_items_index = 0;
  int align_content_index = 0;

  std::vector<std::string> directions = {
      "Row",
      "RowInversed",
      "Column",
      "ColumnInversed",
  };

  std::vector<std::string> wraps = {
      "NoWrap",
      "Wrap",
      "WrapInversed",
  };

  std::vector<std::string> justify_content = {
      "FlexStart",    "FlexEnd",     "Center",      "Stretch",
      "SpaceBetween", "SpaceAround", "SpaceEvenly",
  };

  std::vector<std::string> align_items = {
      "FlexStart",
      "FlexEnd",
      "Center",
      "Stretch",
  };

  std::vector<std::string> align_content = {
      "FlexStart",    "FlexEnd",     "Center",      "Stretch",
      "SpaceBetween", "SpaceAround", "SpaceEvenly",
  };

  auto radiobox_direction = Radiobox(&directions, &direction_index);
  auto radiobox_wrap = Radiobox(&wraps, &wrap_index);
  auto radiobox_justify_content =
      Radiobox(&justify_content, &justify_content_index);
  auto radiobox_align_items = Radiobox(&align_items, &align_items_index);
  auto radiobox_align_content = Radiobox(&align_content, &align_content_index);

  bool element_xflex_grow = false;
  bool element_yflex_grow = false;
  bool group_xflex_grow = true;
  bool group_yflex_grow = true;
  auto checkbox_element_xflex_grow =
      Checkbox("element |= xflex_grow", &element_xflex_grow);
  auto checkbox_element_yflex_grow =
      Checkbox("element |= yflex_grow", &element_yflex_grow);
  auto checkbox_group_xflex_grow =
      Checkbox("group |= xflex_grow", &group_xflex_grow);
  auto checkbox_group_yflex_grow =
      Checkbox("group |= yflex_grow", &group_yflex_grow);

  auto make_box = [&](size_t dimx, size_t dimy, size_t index) {
    std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
    auto element = window(text(title) | hcenter | bold,
                          text(std::to_string(index)) | hcenter | dim) |
                   size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy) |
                   bgcolor(Color::HSV(index * 25, 255, 255)) |
                   color(Color::Black);
    if (element_xflex_grow)
      element = element | xflex_grow;
    if (element_yflex_grow)
      element = element | yflex_grow;
    return element;
  };

  auto content_renderer = Renderer([&] {
    FlexboxConfig config;
    config.direction = static_cast<FlexboxConfig::Direction>(direction_index);
    config.wrap = static_cast<FlexboxConfig::Wrap>(wrap_index);
    config.justify_content =
        static_cast<FlexboxConfig::JustifyContent>(justify_content_index);
    config.align_items =
        static_cast<FlexboxConfig::AlignItems>(align_items_index);
    config.align_content =
        static_cast<FlexboxConfig::AlignContent>(align_content_index);

    auto group = flexbox(
        {
            make_box(8, 4, 0),
            make_box(9, 6, 1),
            make_box(11, 6, 2),
            make_box(10, 4, 3),
            make_box(13, 7, 4),
            make_box(12, 4, 5),
            make_box(12, 5, 6),
            make_box(10, 4, 7),
            make_box(12, 4, 8),
            make_box(10, 5, 9),
        },
        config);

    group = group | bgcolor(Color::Black);

    group = group | notflex;

    if (!group_xflex_grow)
      group = hbox(group, filler());
    if (!group_yflex_grow)
      group = vbox(group, filler());

    group = group | flex;
    return group;
  });

  auto center = FlexboxConfig()
                    .Set(FlexboxConfig::JustifyContent::Center)
                    .Set(FlexboxConfig::AlignContent::Center);
  int space_right = 10;
  int space_bottom = 1;
  content_renderer = ResizableSplitRight(
      Renderer([&] { return flexbox({text("resizable")}, center); }),
      content_renderer, &space_right);
  content_renderer = ResizableSplitBottom(
      Renderer([&] { return flexbox({text("resizable")}, center); }),
      content_renderer, &space_bottom);

  auto main_container = Container::Vertical({
      Container::Horizontal({
          radiobox_direction,
          radiobox_wrap,
          Container::Vertical({
              checkbox_element_xflex_grow,
              checkbox_element_yflex_grow,
              checkbox_group_xflex_grow,
              checkbox_group_yflex_grow,
          }),
      }),
      Container::Horizontal({
          radiobox_justify_content,
          radiobox_align_items,
          radiobox_align_content,
      }),
      content_renderer,
  });

  auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        vbox({hbox({
                  window(text("FlexboxConfig::Direction"),
                         radiobox_direction->Render()),
                  window(text("FlexboxConfig::Wrap"), radiobox_wrap->Render()),
                  window(text("Misc:"),
                         vbox({
                             checkbox_element_xflex_grow->Render(),
                             checkbox_element_yflex_grow->Render(),
                             checkbox_group_xflex_grow->Render(),
                             checkbox_group_yflex_grow->Render(),
                         })),
              }),
              hbox({
                  window(text("FlexboxConfig::JustifyContent"),
                         radiobox_justify_content->Render()),
                  window(text("FlexboxConfig::AlignItems"),
                         radiobox_align_items->Render()),
                  window(text("FlexboxConfig::AlignContent"),
                         radiobox_align_content->Render()),
              })}),
        content_renderer->Render() | flex | border,
    });
  });

  screen.Loop(main_renderer);

  return 0;
}
