// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cstddef>
#include <string>
#include <vector>
#include "ftxui/dom/canvas.hpp"

using namespace ftxui;

namespace {

int GeneratorInt(const char*& data, size_t& size) {
  if (size == 0) {
    return 0;
  }
  auto out = int(data[0]);
  data++;
  size--;
  return out;
}

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
  return out;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  if (size < 5) {
    return 0;
  }

  // Read canvas width and height
  int width = GeneratorInt(data, size);
  int height = GeneratorInt(data, size);

  // Keep dimensions bounded but allow some small, zero, negative or moderately
  // large values
  width = (width % 100) - 10;
  height = (height % 100) - 10;

  Canvas canvas(width, height);

  // Perform multiple drawing actions while data is available
  while (size > 2) {
    int action = GeneratorInt(data, size) % 15;
    switch (action) {
      case 0: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        canvas.DrawPointOn(x, y);
        break;
      }
      case 1: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        canvas.DrawPointOff(x, y);
        break;
      }
      case 2: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        canvas.DrawPointToggle(x, y);
        break;
      }
      case 3: {
        int x1 = GeneratorInt(data, size);
        int y1 = GeneratorInt(data, size);
        int x2 = GeneratorInt(data, size);
        int y2 = GeneratorInt(data, size);
        canvas.DrawPointLine(x1, y1, x2, y2);
        break;
      }
      case 4: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r = GeneratorInt(data, size) % 30;
        canvas.DrawPointCircle(x, y, r);
        break;
      }
      case 5: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r = GeneratorInt(data, size) % 30;
        canvas.DrawPointCircleFilled(x, y, r);
        break;
      }
      case 6: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r1 = GeneratorInt(data, size) % 30;
        int r2 = GeneratorInt(data, size) % 30;
        canvas.DrawPointEllipse(x, y, r1, r2);
        break;
      }
      case 7: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r1 = GeneratorInt(data, size) % 30;
        int r2 = GeneratorInt(data, size) % 30;
        canvas.DrawPointEllipseFilled(x, y, r1, r2);
        break;
      }
      case 8: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        canvas.DrawBlockOn(x, y);
        break;
      }
      case 9: {
        int x1 = GeneratorInt(data, size);
        int y1 = GeneratorInt(data, size);
        int x2 = GeneratorInt(data, size);
        int y2 = GeneratorInt(data, size);
        canvas.DrawBlockLine(x1, y1, x2, y2);
        break;
      }
      case 10: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r = GeneratorInt(data, size) % 30;
        canvas.DrawBlockCircle(x, y, r);
        break;
      }
      case 11: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        int r = GeneratorInt(data, size) % 30;
        canvas.DrawBlockCircleFilled(x, y, r);
        break;
      }
      case 12: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        std::string s = GeneratorString(data, size);
        canvas.DrawText(x, y, s);
        break;
      }
      case 13: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        canvas.GetCell(x, y);
        break;
      }
      case 14: {
        int x = GeneratorInt(data, size);
        int y = GeneratorInt(data, size);
        bool val = GeneratorBool(data, size);
        canvas.DrawPoint(x, y, val);
        break;
      }
    }
  }

  return 0;
}
