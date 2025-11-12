/// @module ftxui.component:ComponentBase
/// @brief Module file for the ComponentBase class of the Component module

module;

#include <ftxui/component/component_base.hpp>

export module ftxui.component:ComponentBase;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::Delegate;
    using ftxui::Focus;
    using ftxui::Event;

    /**
     * @namespace animation
     * @brief The FTXUI ftxui::animation:: namespace
     */
    namespace animation {
        using ftxui::animation::Params;
    }

    using ftxui::ComponentBase;
    using ftxui::Component;
    using ftxui::Components;
}
