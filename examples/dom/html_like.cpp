#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>
#include <thread>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  using namespace std::chrono_literals;

  auto img1 = []() { return text(L"img") | border; };
  auto img2 = []() { return vbox({text(L"big"), text(L"image")}) | border; };

  std::string reset_position;
  for (int i = 0;; ++i) {
    auto document =  //
        hflow(
            paragraph(L"Hello world! Here is an image:"), img1(),
            paragraph(L" Here is a text "), text(L"underlined ") | underlined,
            paragraph(L" Here is a text "), text(L"bold ") | bold,
            paragraph(L"Hello world! Here is an image:"), img2(),
            paragraph(
                L"Le Lorem Ipsum est simplement du faux texte employé dans la "
                L"composition et la mise en page avant impression. Le Lorem "
                L"Ipsum est le faux texte standard de l'imprimerie depuis les "
                L"années 1500, quand un imprimeur anonyme assembla ensemble "
                L"des morceaux de texte pour réaliser un livre spécimen de "
                L"polices de texte. Il n'a pas fait que survivre cinq siècles, "
                L"mais s'est aussi adapté à la bureautique informatique, sans "
                L"que son contenu n'en soit modifié. Il a été popularisé dans "
                L"les années 1960 grâce à la vente de feuilles Letraset "
                L"contenant des passages du Lorem Ipsum, et, plus récemment, "
                L"par son inclusion dans des applications de mise en page de "
                L"texte, comme Aldus PageMaker."),
            paragraph(L" Here is a text "), text(L"dim ") | dim,
            paragraph(L"Hello world! Here is an image:"), img1(),
            paragraph(L" Here is a text "), text(L"red ") | color(Color::Red),
            paragraph(L" A spinner "), spinner(6, i / 10)) |
        border;

    auto screen = Screen::Create(Dimension::Full());
    Render(screen, document);
    std::cout << reset_position << screen.ToString() << std::flush;

    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.01s);
  }

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
