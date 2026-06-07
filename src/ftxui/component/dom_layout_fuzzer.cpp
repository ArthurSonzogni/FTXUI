// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cassert>
#include <vector>
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

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
  while (index < size && data[index]) {
    ++index;
  }
  auto out = std::string(data, data + index);
  data += index;
  size -= index;
  // Make sure it doesn't contain invalid UTF8
  try {
    to_wstring(out);
  } catch (...) {
    return "0";
  }
  return out;
}

int GeneratorInt(const char*& data, size_t& size) {
  if (size == 0) {
    return 0;
  }
  auto out = int(data[0]);
  data++;
  size--;
  return out;
}

float GeneratorFloat(const char*& data, size_t& size) {
  if (size == 0) {
    return 0.0f;
  }
  auto out = float(data[0]) / 255.0f;
  data++;
  size--;
  return out;
}

Elements GeneratorElements(const char*& data, size_t& size, int depth);

Element GeneratorElement(const char*& data, size_t& size, int depth) {
  depth--;
  int value = GeneratorInt(data, size);
  if (depth <= 0) {
    int base_choice = value % 5;
    if (base_choice == 0) {
      return text(GeneratorString(data, size));
    }
    if (base_choice == 1) {
      return paragraph(GeneratorString(data, size));
    }
    if (base_choice == 2) {
      return separator();
    }
    if (base_choice == 3) {
      return gauge(GeneratorFloat(data, size));
    }
    return emptyElement();
  }

  constexpr int value_max = 20;
  value = (value % value_max + value_max) % value_max;
  switch (value) {
    case 0:
      return text(GeneratorString(data, size));
    case 1:
      return paragraph(GeneratorString(data, size));
    case 2:
      return separator();
    case 3:
      return gauge(GeneratorFloat(data, size));
    case 4:
      return border(GeneratorElement(data, size, depth));
    case 5:
      return borderRounded(GeneratorElement(data, size, depth));
    case 6:
      return window(text(GeneratorString(data, size)),
                    GeneratorElement(data, size, depth));
    case 7:
      return vbox(GeneratorElements(data, size, depth));
    case 8:
      return hbox(GeneratorElements(data, size, depth));
    case 9:
      return dbox(GeneratorElements(data, size, depth));
    case 10:
      return flexbox(GeneratorElements(data, size, depth));
    case 11:
      return bold(GeneratorElement(data, size, depth));
    case 12:
      return dim(GeneratorElement(data, size, depth));
    case 13:
      return italic(GeneratorElement(data, size, depth));
    case 14:
      return inverted(GeneratorElement(data, size, depth));
    case 15:
      return underlined(GeneratorElement(data, size, depth));
    case 16:
      return blink(GeneratorElement(data, size, depth));
    case 17:
      return strikethrough(GeneratorElement(data, size, depth));
    case 18:
      return flex(GeneratorElement(data, size, depth));
    case 19:
      return filler();
    default:
      return emptyElement();
  }
}

Elements GeneratorElements(const char*& data, size_t& size, int depth) {
  Elements out;
  if (depth > 0) {
    while (size && (GeneratorInt(data, size) % 2)) {
      out.push_back(GeneratorElement(data, size, depth - 1));
    }
  }
  return out;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  if (size < 5) {
    return 0;
  }

  int width = GeneratorInt(data, size);
  int height = GeneratorInt(data, size);

  width = (width % 200) + 1;
  height = (height % 200) + 1;

  int depth = 5;
  auto element = GeneratorElement(data, size, depth);

  auto screen =
      Screen::Create(Dimension::Fixed(width), Dimension::Fixed(height));
  Render(screen, element);
  screen.ToString();

  return 0;
}
