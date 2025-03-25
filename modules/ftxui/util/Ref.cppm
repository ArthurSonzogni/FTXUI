/**
 * @file Ref.cppm
 * @brief Module file for the Ref classes of the Util module
 */

module;

#include <ftxui/util/ref.hpp>

export module ftxui.util.Ref;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::ConstRef;
    using ftxui::Ref;
    using ftxui::StringRef;
    using ftxui::ConstStringRef;
    using ftxui::ConstStringListRef;
}
