#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen.hpp"
#include "ftxui/dom/elements.hpp"

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
	auto document =
		hbox(
			window(hcenter(text(L" main frame ")),
				vbox(
					text(L"Line 1"),
					text(L"Line 2"),
					text(L"Line 3"),
					frame(
						vbox(
							text(L"Line 4"),
							text(L"Line 5"),
							text(L"Line 6")
						)
					),
					hbox(
						window(text(L"frame 2"),
							vbox(
								text(L"Line 4"),
								text(L"Line 5"),
								text(L"Line 6")
							)
						),
						window(text(L"frame 3"),
							vbox(
								text(L"Line 7"),
								text(L"Line 8"),
								text(L"Line 9")
							)
						)
					),
					text(L"footer footer footer footer footer")
				)
			),
			filler()
		);
  auto screen = ftxui::Screen::TerminalOutput(document);
  Render(screen, document.get());
  std::cout << screen.ToString() << std::endl;
}
