/// @module ftxui.screen:Color
/// @brief Module file for the Color class of the Screen module

module;

#include <ftxui/screen/color.hpp>

export module ftxui.screen:Color;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::Color;

    /**
     * @namespace literals
     * @brief The FTXUI ftxui::literals:: namespace
     */
    inline namespace literals {
        using ftxui::literals::operator""_rgb;
    }
}
