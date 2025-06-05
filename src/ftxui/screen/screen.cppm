/// @module ftxui.screen.screen
/// @brief Module file for the Screen class of the Screen module

module;

#include <ftxui/screen/screen.hpp>

export module ftxui.screen.screen;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    namespace Dimension {
        using ftxui::Dimension::Fixed;
        using ftxui::Dimension::Full;
    }

    using ftxui::Image;
    using ftxui::Screen;
}
