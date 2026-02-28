// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "gtest/gtest.h"
#include "ftxui/screen/pixel.hpp"
#include "ftxui/screen/image.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

TEST(CompatibilityTest, PixelAlias) {
  Pixel pixel;
  pixel.character = "a";
  pixel.bold = true;
  
  Cell cell;
  cell.character = "a";
  cell.bold = true;

  EXPECT_EQ(pixel.character, cell.character);
  EXPECT_EQ(pixel.bold, cell.bold);
}

TEST(CompatibilityTest, ImageAlias) {
  Image image(10, 10);
  EXPECT_EQ(image.dimx(), 10);
  EXPECT_EQ(image.dimy(), 10);
  
  Surface surface(10, 10);
  EXPECT_EQ(image.dimx(), surface.dimx());
  EXPECT_EQ(image.dimy(), surface.dimy());
}

TEST(CompatibilityTest, PixelAtAlias) {
  Image image(10, 10);
  image.PixelAt(0, 0).character = "b";
  EXPECT_EQ(image.at(0, 0), "b");
  EXPECT_EQ(image.CellAt(0, 0).character, "b");
}

TEST(CompatibilityTest, ScreenInteractiveAlias) {
  // We can't easily run a full ScreenInteractive in a test without blocking,
  // but we can check the alias and static constructors.
  auto screen = ScreenInteractive::FixedSize(80, 24);
  EXPECT_EQ(screen.dimx(), 80);
  EXPECT_EQ(screen.dimy(), 24);
  
  auto app = App::FixedSize(80, 24);
  EXPECT_EQ(screen.dimx(), app.dimx());
  EXPECT_EQ(screen.dimy(), app.dimy());
}

TEST(CompatibilityTest, DomElements) {
  Pixel pixel;
  pixel.character = "X";
  auto element = separator(pixel);
  auto element2 = text("X") | borderWith(pixel);
  
  // Just check they compile and run
  Screen screen(10, 10);
  Render(screen, element);
  Render(screen, element2);
}

TEST(CompatibilityTest, CanvasCompatibility) {
  Canvas canvas(10, 10);
  Cell cell;
  cell.character = "a";
  
  canvas.DrawPixel(0, 0, cell);
  EXPECT_EQ(canvas.GetPixel(0, 0).character, "a");
  
  Surface surface(10, 10);
  canvas.DrawImage(0, 0, surface);
}

}  // namespace ftxui
