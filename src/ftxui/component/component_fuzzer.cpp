// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cassert>
#include <vector>
#include "ftxui/component/component.hpp"
#include "ftxui/component/terminal_input_parser.hpp"

using namespace ftxui;
namespace {

bool GeneratorBool(const char*& data, size_t& size) {
  if (size == 0) {
    return false;
  }

  auto out = bool(data[0] % 2);
  data++;
  size--;
  return out;
}

std::string GeneratorString(const char*& data, size_t& size) {
  int index = 0;
  while (index < size && data[index])
    ++index;

  auto out = std::string(data, data + index);
  data += index;
  size -= index;

  // The input component do not support invalid UTF8 yet.
  try {
    to_wstring(out);
  } catch (...) {
    return "0";
  }
  return std::move(out);
}

int GeneratorInt(const char* data, size_t size) {
  if (size == 0)
    return 0;
  auto out = int(data[0]);
  data++;
  size--;
  return out;
}

Color GeneratorColor(const char* data, size_t size) {
  return Color::RGB(GeneratorInt(data, size), GeneratorInt(data, size),
                    GeneratorInt(data, size));
}

AnimatedColorOption GeneratorAnimatedColorOption(const char* data,
                                                 size_t size) {
  AnimatedColorOption option;
  option.enabled = GeneratorBool(data, size);
  option.inactive = GeneratorColor(data, size);
  option.active = GeneratorColor(data, size);
  option.duration = std::chrono::milliseconds(GeneratorInt(data, size));
  return option;
}

AnimatedColorsOption GeneratorAnimatedColorsOptions(const char* data,
                                                    size_t size) {
  AnimatedColorsOption option;
  option.background = GeneratorAnimatedColorOption(data, size);
  option.foreground = GeneratorAnimatedColorOption(data, size);
  return option;
}

ButtonOption GeneratorButtonOption(const char* data, size_t size) {
  ButtonOption option;
  option.animated_colors = GeneratorAnimatedColorsOptions(data, size);
  return option;
}

UnderlineOption GeneratorUnderlineOption(const char* data, size_t size) {
  UnderlineOption option;
  option.enabled = GeneratorBool(data, size);
  option.color_active = GeneratorColor(data, size);
  option.color_inactive = GeneratorColor(data, size);
  option.leader_duration = std::chrono::milliseconds(GeneratorInt(data, size));
  option.follower_duration =
      std::chrono::milliseconds(GeneratorInt(data, size));
  option.leader_delay = std::chrono::milliseconds(GeneratorInt(data, size));
  option.follower_delay = std::chrono::milliseconds(GeneratorInt(data, size));
  return option;
}

MenuEntryOption GeneratorMenuEntryOption(const char* data, size_t size) {
  MenuEntryOption option;
  option.animated_colors = GeneratorAnimatedColorsOptions(data, size);
  return option;
}

MenuOption GeneratorMenuOption(const char* data, size_t size) {
  MenuOption option;
  option.underline = GeneratorUnderlineOption(data, size);
  option.entries_option = GeneratorMenuEntryOption(data, size);
  option.direction = static_cast<Direction>(GeneratorInt(data, size) % 4);
  return option;
}

bool g_bool;
int g_int;
std::vector<std::string> g_list;

Components GeneratorComponents(const char*& data, size_t& size, int depth);

Component GeneratorComponent(const char*& data, size_t& size, int depth) {
  depth--;
  int value = GeneratorInt(data, size);
  if (depth <= 0)
    return Button(GeneratorString(data, size), [] {});

  constexpr int value_max = 19;
  value = (value % value_max + value_max) % value_max;
  switch (value) {
    case 0:
      return Button(
          GeneratorString(data, size), [] {},
          GeneratorButtonOption(data, size));
    case 1:
      return Checkbox(GeneratorString(data, size), &g_bool);
    case 2:
      return Input(GeneratorString(data, size), GeneratorString(data, size));
    case 3:
      return Menu(&g_list, &g_int, GeneratorMenuOption(data, size));
    case 4:
      return Radiobox(&g_list, &g_int);
    case 5:
      return Toggle(&g_list, &g_int);
    case 6:
      return Slider(GeneratorString(data, size), &g_int,
                    GeneratorInt(data, size), GeneratorInt(data, size),
                    GeneratorInt(data, size));
    case 7:
      return ResizableSplitLeft(GeneratorComponent(data, size, depth - 1),
                                GeneratorComponent(data, size, depth - 1),
                                &g_int);
    case 8:
      return ResizableSplitRight(GeneratorComponent(data, size, depth - 1),
                                 GeneratorComponent(data, size, depth - 1),
                                 &g_int);
    case 9:
      return ResizableSplitTop(GeneratorComponent(data, size, depth - 1),
                               GeneratorComponent(data, size, depth - 1),
                               &g_int);
    case 10:
      return ResizableSplitBottom(GeneratorComponent(data, size, depth - 1),
                                  GeneratorComponent(data, size, depth - 1),
                                  &g_int);
    case 11:
      return Container::Vertical(GeneratorComponents(data, size, depth - 1));

    case 12:
      return Container::Vertical(GeneratorComponents(data, size, depth - 1),
                                 &g_int);

    case 13:
      return Container::Horizontal(GeneratorComponents(data, size, depth - 1));
    case 14:
      return Container::Horizontal(GeneratorComponents(data, size, depth - 1),
                                   &g_int);
    case 15:
      return Container::Tab(GeneratorComponents(data, size, depth - 1), &g_int);
    case 16:
      return Maybe(GeneratorComponent(data, size, depth - 1), &g_bool);
    case 17:
      return Dropdown(&g_list, &g_int);
    case 18:
      return Collapsible(GeneratorString(data, size),
                         GeneratorComponent(data, size, depth - 1),
                         GeneratorBool(data, size));
    default:
      assert(false);
  }
}

Components GeneratorComponents(const char*& data, size_t& size, int depth) {
  Components out;
  if (depth > 0) {
    while (size && GeneratorInt(data, size) % 2) {
      out.push_back(GeneratorComponent(data, size, depth - 1));
    }
  }
  return std::move(out);
}

}  // namespace
extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  g_bool = GeneratorBool(data, size);
  g_int = GeneratorInt(data, size);
  g_list = {
      "test_1", "test_2", "test_3", "test_4", "test_5",
  };

  int depth = 10;
  auto component = GeneratorComponent(data, size, depth);

  int width = GeneratorInt(data, size);
  int height = GeneratorInt(data, size);

  width %= 500;
  width += 500;

  height %= 500;
  height += 500;

  auto screen =
      Screen::Create(Dimension::Fixed(width), Dimension::Fixed(height));

  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    for (size_t i = 0; i < size; ++i)
      parser.Add(data[i]);
  }

  Task event;
  while (event_receiver->Receive(&event)) {
    component->OnEvent(std::get<Event>(event));
    auto document = component->Render();
    Render(screen, document);
  }
  return 0;  // Non-zero return values are reserved for future use.
}
