/// @module ftxui.screen:Terminal
/// @brief Module file for the Terminal namespace of the Screen module

module;

#include <ftxui/screen/terminal.hpp>

export module ftxui.screen:Terminal;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::Dimensions;

    /**
     * @namespace Terminal
     * @brief The FTXUI ftxui::Terminal:: namespace
     */
    namespace Terminal {
        using ftxui::Terminal::Size;
        using ftxui::Terminal::SetFallbackSize;
        using ftxui::Terminal::Color;
        using ftxui::Terminal::ColorSupport;
        using ftxui::Terminal::SetColorSupport;
    }
}
