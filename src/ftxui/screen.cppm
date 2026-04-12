/// @module ftxui.screen
/// @brief Module file for FTXUI screen operations.

module;

#include <ftxui/screen/box.hpp>
#include <ftxui/screen/cell.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/color_info.hpp>
#include <ftxui/screen/deprecated.hpp>
#include <ftxui/screen/image.hpp>
#include <ftxui/screen/pixel.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <ftxui/screen/surface.hpp>
#include <ftxui/screen/terminal.hpp>

export module ftxui.screen;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::Box;
    using ftxui::Cell;
    using ftxui::Color;
    using ftxui::ColorInfo;
    using ftxui::Dimensions;
    using ftxui::Image;
    using ftxui::Pixel;
    using ftxui::Screen;
    using ftxui::Surface;

    using ftxui::CellToGlyphIndex;
    using ftxui::GetColorInfo;
    using ftxui::Utf8ToGlyphs;
    using ftxui::string_width;
    using ftxui::to_string;
    using ftxui::to_wstring;
    using ftxui::wchar_width;
    using ftxui::wstring_width;

    /**
     * @namespace Dimension
     * @brief The FTXUI ftxui::Dimension:: namespace
     */
    namespace Dimension {
        using ftxui::Dimension::Fixed;
        using ftxui::Dimension::Full;
    }

    /**
     * @namespace Terminal
     * @brief The FTXUI ftxui::Terminal:: namespace
     */
    namespace Terminal {
        using ftxui::Terminal::Color;
        using ftxui::Terminal::Size;
        using ftxui::Terminal::SetFallbackSize;
        using ftxui::Terminal::ColorSupport;
        using ftxui::Terminal::SetColorSupport;
    }

    /**
     * @namespace literals
     * @brief The FTXUI ftxui::literals:: namespace
     */
    inline namespace literals {
        using ftxui::literals::operator""_rgb;
    }
}
