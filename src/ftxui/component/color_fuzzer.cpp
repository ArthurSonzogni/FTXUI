// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cstddef>
#include <cstdint>
#include <string>
#include "ftxui/screen/color.hpp"
#include "ftxui/screen/terminal.hpp"

using namespace ftxui;

namespace {

uint8_t GeneratorByte(const char*& data, size_t& size) {
  if (size == 0) {
    return 0;
  }
  uint8_t out = static_cast<uint8_t>(data[0]);
  data++;
  size--;
  return out;
}

float GeneratorFloat(const char*& data, size_t& size) {
  if (size == 0) {
    return 0.0f;
  }
  float out = float(static_cast<uint8_t>(data[0])) / 255.0f;
  data++;
  size--;
  return out;
}

Color GeneratorColor(const char*& data, size_t& size) {
  if (size < 4) {
    return Color();
  }
  int type = GeneratorByte(data, size) % 5;
  switch (type) {
    case 0:
      return Color(
          static_cast<Color::Palette16>(GeneratorByte(data, size) % 16));
    case 1:
      return Color(static_cast<Color::Palette256>(GeneratorByte(data, size)));
    case 2: {
      uint8_t r = GeneratorByte(data, size);
      uint8_t g = GeneratorByte(data, size);
      uint8_t b = GeneratorByte(data, size);
      return Color::RGB(r, g, b);
    }
    case 3: {
      uint8_t h = GeneratorByte(data, size);
      uint8_t s = GeneratorByte(data, size);
      uint8_t v = GeneratorByte(data, size);
      return Color::HSV(h, s, v);
    }
    case 4: {
      uint8_t r = GeneratorByte(data, size);
      uint8_t g = GeneratorByte(data, size);
      uint8_t b = GeneratorByte(data, size);
      uint8_t a = GeneratorByte(data, size);
      return Color::RGBA(r, g, b, a);
    }
    default:
      return Color();
  }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  if (size < 5) {
    return 0;
  }

  // Read terminal color support
  int support = GeneratorByte(data, size) % 4;
  switch (support) {
    case 0:
      Terminal::SetColorSupport(Terminal::Color::Palette1);
      break;
    case 1:
      Terminal::SetColorSupport(Terminal::Color::Palette16);
      break;
    case 2:
      Terminal::SetColorSupport(Terminal::Color::Palette256);
      break;
    case 3:
      Terminal::SetColorSupport(Terminal::Color::TrueColor);
      break;
  }

  while (size > 1) {
    int op = GeneratorByte(data, size) % 4;
    switch (op) {
      case 0: {
        Color c = GeneratorColor(data, size);
        bool is_background = GeneratorByte(data, size) % 2;
        c.Print(is_background);
        break;
      }
      case 1: {
        Color c1 = GeneratorColor(data, size);
        Color c2 = GeneratorColor(data, size);
        float t = GeneratorFloat(data, size);
        Color result = Color::Interpolate(t, c1, c2);
        result.Print(true);
        result.Print(false);
        break;
      }
      case 2: {
        Color c1 = GeneratorColor(data, size);
        Color c2 = GeneratorColor(data, size);
        Color result = Color::Blend(c1, c2);
        result.Print(true);
        result.Print(false);
        break;
      }
      case 3: {
        uint8_t h = GeneratorByte(data, size);
        uint8_t s = GeneratorByte(data, size);
        uint8_t v = GeneratorByte(data, size);
        uint8_t a = GeneratorByte(data, size);
        Color result = Color::HSVA(h, s, v, a);
        result.Print(true);
        break;
      }
    }
  }

  return 0;
}
