/// @module ftxui.util
/// @brief Module file for FTXUI utility operations.

module;

#include <ftxui/util/autoreset.hpp>
#include <ftxui/util/ref.hpp>

export module ftxui.util;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::AutoReset;
    using ftxui::ConstRef;
    using ftxui::ConstStringListRef;
    using ftxui::ConstStringRef;
    using ftxui::Ref;
    using ftxui::StringRef;
}
