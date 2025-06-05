/// @module ftxui.screen.string
/// @brief Module file for string functions of the Screen module

module;

#include <ftxui/screen/string.hpp>

export module ftxui.screen.string;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::to_string;
    using ftxui::to_wstring;
    using ftxui::string_width;
    using ftxui::Utf8ToGlyphs;
    using ftxui::CellToGlyphIndex;
}
