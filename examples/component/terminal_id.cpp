// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/component.hpp"       // for Renderer
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border

int main() {
	using namespace ftxui;

	std::string terminal_id = "UNKNOWN";

	auto screen =
		ScreenInteractive::TerminalOutput();

	screen.OnTerminalIDUpdate([&terminal_id] (
		TerminalID const& terminal_id_update)
	{
		std::stringstream stream;
		stream << terminal_id_update;
		terminal_id = stream.str();
	});

	auto renderer = Renderer([&]
	{
		return vbox({
			text("Terminal id " + terminal_id),
		}) | border;
	});

	screen.Loop(renderer);
}
