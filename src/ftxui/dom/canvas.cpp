#include "ftxui/dom/canvas.hpp"

#include <map>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

namespace ftxui {

namespace {

// Base UTF8 pattern:
// 11100010 10100000 10000000 // empty

// Pattern for the individuel dots:
// ┌──────┬───────┐
// │dot1  │ dot4  │
// ├──────┼───────┤
// │dot2  │ dot5  │
// ├──────┼───────┤
// │dot3  │ dot6  │
// ├──────┼───────┤
// │dot0-1│ dot0-2│
// └──────┴───────┘
// 11100010 10100000 10000001 // dot1
// 11100010 10100000 10000010 // dot2
// 11100010 10100000 10000100 // dot3
// 11100010 10100001 10000000 // dot0-1
// 11100010 10100000 10001000 // dot4
// 11100010 10100000 10010000 // dot5
// 11100010 10100000 10100000 // dot6
// 11100010 10100010 10000000 // dot0-2

uint8_t g_map_braille[2][4][2] = {
    {
        {0b00000000, 0b00000001},  // dot1
        {0b00000000, 0b00000010},  // dot2
        {0b00000000, 0b00000100},  // dot3
        {0b00000001, 0b00000000},  // dot0-1
    },
    {
        {0b00000000, 0b00001000},  // dot4
        {0b00000000, 0b00010000},  // dot5
        {0b00000000, 0b00100000},  // dot6
        {0b00000010, 0b00000000},  // dot0-2
    },
};

std::vector<std::string> g_map_block = {
    " ", "▘", "▖", "▌", "▝", "▀", "▞", "▛",
    "▗", "▚", "▄", "▙", "▐", "▜", "▟", "█",
};

const std::map<std::string, uint8_t> g_map_block_inversed = {
    {" ", 0b0000}, {"▘", 0b0001}, {"▖", 0b0010}, {"▌", 0b0011},
    {"▝", 0b0100}, {"▀", 0b0101}, {"▞", 0b0110}, {"▛", 0b0111},
    {"▗", 0b1000}, {"▚", 0b1001}, {"▄", 0b1010}, {"▙", 0b1011},
    {"▐", 0b1100}, {"▜", 0b1101}, {"▟", 0b1110}, {"█", 0b1111},
};

}  // namespace

Canvas::Canvas(int width, int height)
    : width_(width), height_(height), storage_(width_ * height_ / 8) {}

Pixel Canvas::GetPixel(int x, int y) const {
  auto it = storage_.find(XY{x / 2, y / 4});
  return (it == storage_.end()) ? Pixel{} : it->second.content;
}

void Canvas::DrawPoint(int x, int y, bool value) {
  DrawPoint(x, y, value, [](Pixel&) {});
}

void Canvas::DrawPoint(int x, int y, bool value, const Color& color) {
  DrawPoint(x, y, value, [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPoint(int x, int y, bool value, const Stylizer& style) {
  Style(x, y, style);
  if (value)
    DrawPointOn(x, y);
  else
    DrawPointOff(x, y);
}

void Canvas::DrawPointOn(int x, int y) {
  if (!IsIn(x,y))
    return;
  Cell& cell = storage_[XY{x / 2, y / 4}];
  if (cell.type != CellType::kBraille) {
    cell.content.character = "⠀";  // 3 bytes.
    cell.type = CellType::kBraille;
  }

  cell.content.character[1] |= g_map_braille[x % 2][y % 4][0];
  cell.content.character[2] |= g_map_braille[x % 2][y % 4][1];
}

void Canvas::DrawPointOff(int x, int y) {
  if (!IsIn(x,y))
    return;
  Cell& cell = storage_[XY{x / 2, y / 4}];
  if (cell.type != CellType::kBraille) {
    cell.content.character = "⠀";  // 3 byt
    cell.type = CellType::kBraille;
  }

  cell.content.character[1] &= ~(g_map_braille[x % 2][y % 4][0]);
  cell.content.character[2] &= ~(g_map_braille[x % 2][y % 4][1]);
}

void Canvas::DrawPointToggle(int x, int y) {
  if (!IsIn(x,y))
    return;
  Cell& cell = storage_[XY{x / 2, y / 4}];
  if (cell.type != CellType::kBraille) {
    cell.content.character = "⠀";  // 3 byt
    cell.type = CellType::kBraille;
  }

  cell.content.character[1] ^= g_map_braille[x % 2][y % 4][0];
  cell.content.character[2] ^= g_map_braille[x % 2][y % 4][1];
}

void Canvas::DrawPointLine(int x1, int y1, int x2, int y2) {
  DrawPointLine(x1, y1, x2, y2, [](Pixel&) {});
}

void Canvas::DrawPointLine(int x1, int y1, int x2, int y2, const Color& color) {
  DrawPointLine(x1, y1, x2, y2,
                [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPointLine(int x1,
                           int y1,
                           int x2,
                           int y2,
                           const Stylizer& style) {
  const int dx = std::abs(x2 - x1);
  const int dy = std::abs(y2 - y1);
  const int sx = x1 < x2 ? 1 : -1;
  const int sy = y1 < y2 ? 1 : -1;
  const int length = std::max(dx, dy);

  if (!IsIn(x1, y1) && !IsIn(x2, y2))
    return;
  if (dx + dx > width_ * height_)
    return;

  int error = dx - dy;
  for (int i = 0; i < length; ++i) {
    DrawPoint(x1, y1, true, style);
    if (2 * error >= -dy) {
      error -= dy;
      x1 += sx;
    }
    if (2 * error <= dx) {
      error += dx;
      y1 += sy;
    }
  }
  DrawPoint(x2, y2, true, style);
}

void Canvas::DrawPointCircle(int x1, int y1, int radius) {
  DrawPointCircle(x1, y1, radius, [](Pixel&) {});
}

void Canvas::DrawPointCircle(int x1, int y1, int radius, const Color& color) {
  DrawPointCircle(x1, y1, radius,
                  [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPointCircle(int x1,
                             int y1,
                             int radius,
                             const Stylizer& style) {
  DrawPointEllipse(x1, y1, radius, radius, style);
}

void Canvas::DrawPointCircleFilled(int x1, int y1, int radius) {
  DrawPointCircleFilled(x1, y1, radius, [](Pixel&) {});
}

void Canvas::DrawPointCircleFilled(int x1,
                                   int y1,
                                   int radius,
                                   const Color& color) {
  DrawPointCircleFilled(x1, y1, radius,
                        [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPointCircleFilled(int x1,
                                   int y1,
                                   int radius,
                                   const Stylizer& style) {
  DrawPointEllipseFilled(x1, y1, radius, radius, style);
}

void Canvas::DrawPointEllipse(int x1, int y1, int r1, int r2) {
  DrawPointEllipse(x1, y1, r1, r2, [](Pixel&) {});
}

void Canvas::DrawPointEllipse(int x1,
                               int y1,
                               int r1,
                               int r2,
                               const Color& color) {
  DrawPointEllipse(x1, y1, r1, r2,
                    [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPointEllipse(int x1,
                               int y1,
                               int r1,
                               int r2,
                               const Stylizer& s) {
  int x = -r1;
  int y = 0;
  int e2 = r2;
  int dx = (1 + 2 * x) * e2 * e2;
  int dy = x * x;
  int err = dx + dy;

  do {
    DrawPoint(x1 - x, y1 + y, true, s);
    DrawPoint(x1 + x, y1 + y, true, s);
    DrawPoint(x1 + x, y1 - y, true, s);
    DrawPoint(x1 - x, y1 - y, true, s);
    e2 = 2 * err;
    if (e2 >= dx) {
      x++;
      err += dx += 2 * r2 * r2;
    }
    if (e2 <= dy) {
      y++;
      err += dy += 2 * r1 * r1;
    }
  } while (x <= 0);

  while (y++ < r2) {
    DrawPoint(x1, y1 + y, true, s);
    DrawPoint(x1, y1 - y, true, s);
  }
}

void Canvas::DrawPointEllipseFilled(int x1, int y1, int r1, int r2) {
  DrawPointEllipseFilled(x1, y1, r1, r2, [](Pixel&) {});
}

void Canvas::DrawPointEllipseFilled(int x1,
                                     int y1,
                                     int r1,
                                     int r2,
                                     const Color& color) {
  DrawPointEllipseFilled(x1, y1, r1, r2,
                          [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawPointEllipseFilled(int x1,
                                     int y1,
                                     int r1,
                                     int r2,
                                     const Stylizer& s) {
  int x = -r1;
  int y = 0;
  int e2 = r2;
  int dx = (1 + 2 * x) * e2 * e2;
  int dy = x * x;
  int err = dx + dy;

  do {
    for (int xx = x1 + x; xx <= x1 - x; ++xx) {
      DrawPoint(xx, y1 + y, true, s);
      DrawPoint(xx, y1 - y, true, s);
    }
    e2 = 2 * err;
    if (e2 >= dx) {
      x++;
      err += dx += 2 * (long)r2 * r2;
    }
    if (e2 <= dy) {
      y++;
      err += dy += 2 * (long)r1 * r1;
    }
  } while (x <= 0);

  while (y++ < r2) {
    for (int yy = y1 - y; yy <= y1 + y; ++yy) {
      DrawPoint(x1, yy, true, s);
    }
  }
}

void Canvas::DrawBlock(int x, int y, bool value) {
  DrawBlock(x, y, value, [](Pixel&) {});
}

void Canvas::DrawBlock(int x, int y, bool value, const Color& color) {
  DrawBlock(x, y, value, [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlock(int x, int y, bool value, const Stylizer& style) {
  Style(x, y, style);
  if (value)
    DrawBlockOn(x, y);
  else
    DrawBlockOff(x, y);
}

void Canvas::DrawBlockOn(int x, int y) {
  if (!IsIn(x,y))
    return;
  y /= 2;
  Cell& cell = storage_[XY{x / 2, y / 2}];
  if (cell.type != CellType::kBlock) {
    cell.content.character = " ";
    cell.type = CellType::kBlock;
  }

  int bit = (x % 2) * 2 + y % 2;
  uint8_t value = g_map_block_inversed.at(cell.content.character);
  value |= 1 << bit;
  cell.content.character = g_map_block[value];
}

void Canvas::DrawBlockOff(int x, int y) {
  if (!IsIn(x,y))
    return;
  Cell& cell = storage_[XY{x / 2, y / 4}];
  if (cell.type != CellType::kBlock) {
    cell.content.character = " ";
    cell.type = CellType::kBlock;
  }
  y /= 2;

  int bit = (y % 2) * 2 + x % 2;
  uint8_t value = g_map_block_inversed.at(cell.content.character);
  value &= ~(1 << bit);
  cell.content.character = g_map_block[value];
}

void Canvas::DrawBlockToggle(int x, int y) {
  if (!IsIn(x,y))
    return;
  Cell& cell = storage_[XY{x / 2, y / 4}];
  if (cell.type != CellType::kBlock) {
    cell.content.character = " ";
    cell.type = CellType::kBlock;
  }
  y /= 2;

  int bit = (y % 2) * 2 + x % 2;
  uint8_t value = g_map_block_inversed.at(cell.content.character);
  value ^= 1 << bit;
  cell.content.character = g_map_block[value];
}

void Canvas::DrawBlockLine(int x1, int y1, int x2, int y2) {
  DrawBlockLine(x1, y1, x2, y2, [](Pixel&) {});
}

void Canvas::DrawBlockLine(int x1, int y1, int x2, int y2, const Color& color) {
  DrawBlockLine(x1, y1, x2, y2,
                [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlockLine(int x1,
                           int y1,
                           int x2,
                           int y2,
                           const Stylizer& style) {
  y1 /= 2;
  y2 /= 2;

  const int dx = std::abs(x2 - x1);
  const int dy = std::abs(y2 - y1);
  const int sx = x1 < x2 ? 1 : -1;
  const int sy = y1 < y2 ? 1 : -1;
  const int length = std::max(dx, dy);

  if (!IsIn(x1, y1) && !IsIn(x2, y2))
    return;
  if (dx + dx > width_ * height_)
    return;

  int error = dx - dy;
  for (int i = 0; i < length; ++i) {
    DrawBlock(x1, y1 * 2, true, style);
    if (2 * error >= -dy) {
      error -= dy;
      x1 += sx;
    }
    if (2 * error <= dx) {
      error += dx;
      y1 += sy;
    }
  }
  DrawBlock(x2, y2 * 2, true, style);
}

void Canvas::DrawBlockCircle(int x1, int y1, int radius) {
  DrawBlockCircle(x1, y1, radius, [](Pixel&) {});
}

void Canvas::DrawBlockCircle(int x1, int y1, int radius, const Color& color) {
  DrawBlockCircle(x1, y1, radius,
                  [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlockCircle(int x1,
                             int y1,
                             int radius,
                             const Stylizer& style) {
  DrawBlockEllipse(x1, y1, radius, radius, style);
}

void Canvas::DrawBlockCircleFilled(int x1, int y1, int radius) {
  DrawBlockCircleFilled(x1, y1, radius, [](Pixel&) {});
}

void Canvas::DrawBlockCircleFilled(int x1,
                                   int y1,
                                   int radius,
                                   const Color& color) {
  DrawBlockCircleFilled(x1, y1, radius,
                        [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlockCircleFilled(int x1,
                                   int y1,
                                   int radius,
                                   const Stylizer& s) {
  DrawBlockEllipseFilled(x1, y1, radius, radius, s);
}

void Canvas::DrawBlockEllipse(int x1, int y1, int r1, int r2) {
  DrawBlockEllipse(x1, y1, r1, r2, [](Pixel&) {});
}

void Canvas::DrawBlockEllipse(int x1,
                               int y1,
                               int r1,
                               int r2,
                               const Color& color) {
  DrawBlockEllipse(x1, y1, r1, r2,
                    [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlockEllipse(int x1,
                               int y1,
                               int r1,
                               int r2,
                               const Stylizer& s) {
  y1 /= 2;
  r2 /= 2;
  int x = -r1;
  int y = 0;
  int e2 = r2;
  int dx = (1 + 2 * x) * e2 * e2;
  int dy = x * x;
  int err = dx + dy;

  do {
    DrawBlock(x1 - x, 2 * (y1 + y), true, s);
    DrawBlock(x1 + x, 2 * (y1 + y), true, s);
    DrawBlock(x1 + x, 2 * (y1 - y), true, s);
    DrawBlock(x1 - x, 2 * (y1 - y), true, s);
    e2 = 2 * err;
    if (e2 >= dx) {
      x++;
      err += dx += 2 * r2 * r2;
    }
    if (e2 <= dy) {
      y++;
      err += dy += 2 * r1 * r1;
    }
  } while (x <= 0);

  while (y++ < r2) {
    DrawBlock(x1, 2 * (y1 + y), true, s);
    DrawBlock(x1, 2 * (y1 - y), true, s);
  }
}

void Canvas::DrawBlockEllipseFilled(int x1, int y1, int r1, int r2) {
  DrawBlockEllipseFilled(x1, y1, r1, r2, [](Pixel&) {});
}

void Canvas::DrawBlockEllipseFilled(int x1,
                                     int y1,
                                     int r1,
                                     int r2,
                                     const Color& color) {
  DrawBlockEllipseFilled(x1, y1, r1, r2,
                          [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawBlockEllipseFilled(int x1,
                                     int y1,
                                     int r1,
                                     int r2,
                                     const Stylizer& s) {
  y1 /= 2;
  r2 /= 2;
  int x = -r1;
  int y = 0;
  int e2 = r2;
  int dx = (1 + 2 * x) * e2 * e2;
  int dy = x * x;
  int err = dx + dy;

  do {
    for(int xx = x1+x; xx <= x1-x; ++xx) {
      DrawBlock(xx, 2 * (y1 + y), true, s);
      DrawBlock(xx, 2 * (y1 - y), true, s);
    }
    e2 = 2 * err;
    if (e2 >= dx) {
      x++;
      err += dx += 2 * r2 * r2;
    }
    if (e2 <= dy) {
      y++;
      err += dy += 2 * r1 * r1;
    }
  } while (x <= 0);

  while (y++ < r2) {
    for(int yy = y1+y; yy <= y1-y; ++yy) {
      DrawBlock(x1, 2 * yy, true, s);
    }
  }
}

void Canvas::DrawText(int x, int y, const std::string& value) {
  DrawText(x, y, value, [](Pixel&) {});
}

void Canvas::DrawText(int x,
                      int y,
                      const std::string& value,
                      const Color& color) {
  DrawText(x, y, value, [color](Pixel& p) { p.foreground_color = color; });
}

void Canvas::DrawText(int x,
                      int y,
                      const std::string& value,
                      const Stylizer& style) {
  x /= 2;
  y /= 4;
  for (const auto& it : Utf8ToGlyphs(value)) {
    if (!IsIn(x, y))
      continue;
    Cell& cell = storage_[XY{x, y}];
    cell.type = CellType::kText;
    cell.content.character = it;
    style(cell.content);
    x++;
  }
}

void Canvas::Style(int x, int y, const Stylizer& style) {
  if (IsIn(x, y))
    style(storage_[XY{x / 2, y / 4}].content);
}

Element ElementFrom(ConstRef<Canvas> canvas) {
  class Impl : public Node {
   public:
    Impl(ConstRef<Canvas> canvas) : canvas_(canvas) {
      requirement_.min_x = (canvas_->width() + 1) / 2;
      requirement_.min_y = (canvas_->height() + 3) / 4;
    }

    void Render(Screen& screen) override {
      int y_max = std::min(requirement_.min_y, box_.y_max - box_.y_min + 1);
      int x_max = std::min(requirement_.min_x, box_.x_max - box_.x_min + 1);
      for (int y = 0; y < y_max; ++y) {
        for (int x = 0; x < x_max; ++x) {
          screen.PixelAt(box_.x_min + x, box_.y_min + y) =
              canvas_->GetPixel(x * 2, y * 4);
        }
      }
    }

   private:
    ConstRef<Canvas> canvas_;
  };
  return std::make_shared<Impl>(std::move(canvas));
}

}  // namespace ftxui
