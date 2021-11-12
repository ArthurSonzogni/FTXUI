#include <stddef.h>  // for size_t
#include <memory>    // for allocator, allocator_traits<>::value_type
#include <string>    // for basic_string, string
#include <utility>   // for move
#include <vector>    // for vector, __alloc_traits<>::value_type

#include "ftxui/dom/elements.hpp"  // for Element, gauge, text, vbox, spinner

namespace ftxui {

static const std::vector<std::vector<std::vector<std::string>>> elements = {
    {
        {"Replaced by the gauge"},
    },
    {
        {".  "},
        {".. "},
        {"..."},
    },
    {
        {"|"},
        {"/"},
        {"-"},
        {"\\"},
    },
    {
        {"+"},
        {"x"},
    },
    {
        {"|  "},
        {"|| "},
        {"|||"},
    },
    {
        {"←"},
        {"↖"},
        {"↑"},
        {"↗"},
        {"→"},
        {"↘"},
        {"↓"},
        {"↙"},
    },
    {
        {"▁"},
        {"▂"},
        {"▃"},
        {"▄"},
        {"▅"},
        {"▆"},
        {"▇"},
        {"█"},
        {"▇"},
        {"▆"},
        {"▅"},
        {"▄"},
        {"▃"},
        {"▁"},
    },
    {
        {"▉"},
        {"▊"},
        {"▋"},
        {"▌"},
        {"▍"},
        {"▎"},
        {"▏"},
        {"▎"},
        {"▍"},
        {"▌"},
        {"▋"},
        {"▊"},
    },
    {
        {"▖"},
        {"▘"},
        {"▝"},
        {"▗"},
    },
    {
        {"◢"},
        {"◣"},
        {"◤"},
        {"◥"},
    },
    {
        {"◰"},
        {"◳"},
        {"◲"},
        {"◱"},
    },
    {
        {"◴"},
        {"◷"},
        {"◶"},
        {"◵"},
    },
    {
        {"◐"},
        {"◓"},
        {"◑"},
        {"◒"},
    },
    {
        {"◡"},
        {"⊙"},
        {"◠"},
    },
    {
        {"⠁"},
        {"⠂"},
        {"⠄"},
        {"⡀"},
        {"⢀"},
        {"⠠"},
        {"⠐"},
        {"⠈"},
    },
    {
        {"⠋"},
        {"⠙"},
        {"⠹"},
        {"⠸"},
        {"⠼"},
        {"⠴"},
        {"⠦"},
        {"⠧"},
        {"⠇"},
        {"⠏"},
    },
    {
        {"(*----------)"}, {"(-*---------)"}, {"(--*--------)"},
        {"(---*-------)"}, {"(----*------)"}, {"(-----*-----)"},
        {"(------*----)"}, {"(-------*---)"}, {"(--------*--)"},
        {"(---------*-)"}, {"(----------*)"}, {"(---------*-)"},
        {"(--------*--)"}, {"(-------*---)"}, {"(------*----)"},
        {"(-----*-----)"}, {"(----*------)"}, {"(---*-------)"},
        {"(--*--------)"}, {"(-*---------)"},
    },
    {
        {"[      ]"},
        {"[=     ]"},
        {"[==    ]"},
        {"[===   ]"},
        {"[====  ]"},
        {"[===== ]"},
        {"[======]"},
        {"[===== ]"},
        {"[====  ]"},
        {"[===   ]"},
        {"[==    ]"},
        {"[=     ]"},
    },
    {
        {"[      ]"},
        {"[=     ]"},
        {"[==    ]"},
        {"[===   ]"},
        {"[====  ]"},
        {"[===== ]"},
        {"[======]"},
        {"[ =====]"},
        {"[  ====]"},
        {"[   ===]"},
        {"[    ==]"},
        {"[     =]"},
    },
    {
        {"[==    ]"},
        {"[==    ]"},
        {"[==    ]"},
        {"[==    ]"},
        {"[==    ]"},
        {" [==   ]"},
        {"[  ==  ]"},
        {"[   == ]"},
        {"[    ==]"},
        {"[    ==]"},
        {"[    ==]"},
        {"[    ==]"},
        {"[    ==]"},
        {"[   ==] "},
        {"[  ==  ]"},
        {"[ ==   ]"},
    },
    {{
         " ─╮",
         "  │",
         "   ",
     },
     {
         "  ╮",
         "  │",
         "  ╯",
     },
     {
         "   ",
         "  │",
         " ─╯",
     },
     {
         "   ",
         "   ",
         "╰─╯",
     },
     {
         "   ",
         "│  ",
         "╰─ ",
     },
     {
         "╭  ",
         "│  ",
         "╰  ",
     },
     {
         "╭─ ",
         "│  ",
         "   ",
     },
     {
         "╭─╮",
         "   ",
         "   ",
     }},
    {{
         "   /\\O ",
         "    /\\/",
         "   /\\  ",
         "  /  \\ ",
         "LOL  LOL",
     },
     {
         "    _O  ",
         "   //|_ ",
         "    |   ",
         "   /|   ",
         "   LLOL ",
     },
     {
         "     O  ",
         "    /_  ",
         "    |\\  ",
         "   / |  ",
         " LOLLOL ",
     }}};

/// @brief Useful to represent the effect of time and/or events. This display an
/// ASCII art "video".
/// @param charset_index The type of "video".
/// @param image_index The "frame" of the video. You need to increase this for
/// every "step".
/// @ingroup dom
Element spinner(int charset_index, size_t image_index) {
  if (charset_index == 0) {
    image_index %= 40;
    if (image_index > 20)
      image_index = 40 - image_index;
    return gauge(image_index * 0.05);
  }
  charset_index %= elements.size();
  image_index %= elements[charset_index].size();
  std::vector<Element> lines;
  for (const auto& it : elements[charset_index][image_index])
    lines.push_back(text(it));
  return vbox(std::move(lines));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
