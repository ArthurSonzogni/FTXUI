#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  std::wstring p =
      LR"(In probability theory and statistics, Bayes' theorem (alternatively Bayes' law or Bayes' rule) describes the probability of an event, based on prior knowledge of conditions that might be related to the event. For example, if cancer is related to age, then, using Bayes' theorem, a person's age can be used to more accurately assess the probability that they have cancer, compared to the assessment of the probability of cancer made without knowledge of the person's age. One of the many applications of Bayes' theorem is Bayesian inference, a particular approach to statistical inference. When applied, the probabilities involved in Bayes' theorem may have different probability interpretations. With the Bayesian probability interpretation the theorem expresses how a subjective degree of belief should rationally change to account for availability of related evidence. Bayesian inference is fundamental to Bayesian statistics.)";

  auto document = vbox({
      hbox({
          hflow(paragraph(p)) | border,
          hflow(paragraph(p)) | border,
          hflow(paragraph(p)) | border,
      }) | flex,
      hbox({
          hflow(paragraph(p)) | border,
          hflow(paragraph(p)) | border,
      }) | flex,
      hbox({
          hflow(paragraph(p)) | border,
      }) | flex,
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
  Render(screen, document);
  std::cout << screen.ToString();
  getchar();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
