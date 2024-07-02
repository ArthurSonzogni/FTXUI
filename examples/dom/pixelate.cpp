#include "ftxui/dom/canvas.hpp"
#include <stdio.h>                 // for getchar
#include <cmath>                   // for cos
#include <ftxui/dom/elements.hpp>  // for Fit, canvas, operator|, border, Element
#include <ftxui/screen/screen.hpp> // for Pixel, Screen
#include <vector>                  // for vector, allocator

#include "ftxui/dom/canvas.hpp"    // for Canvas
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::Red, Color::Blue, Color::Green, ftxui

using namespace ftxui;
using namespace cv;

int main(int argc, char* argv[]) {
  if (argc != 2 && argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <image_path> [width] [height]" << std::endl;
    return 1;
  }

  std::string image_path = argv[1];
  int width, height;

  if (argc == 4) {
    width = std::stoi(argv[2]);
    height = std::stoi(argv[3]);
  } else {
    // default-values
    width = 100;
    height = 50;
  }

  // call static method
  Canvas::DrawImageOnTerminal(image_path, width, height);

  return 0;
}
