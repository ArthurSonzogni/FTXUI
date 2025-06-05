/// @module ftxui.component.component
/// @brief Module file for the Component classes of the Component module

module;

#include <ftxui/component/component.hpp>

export module ftxui.component.component;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::ButtonOption;
    using ftxui::CheckboxOption;
    using ftxui::Event;
    using ftxui::InputOption;
    using ftxui::MenuOption;
    using ftxui::RadioboxOption;
    using ftxui::MenuEntryOption;

    using ftxui::Make;

    using ftxui::ComponentDecorator;
    using ftxui::ElementDecorator;

    using ftxui::operator|;
    using ftxui::operator|=;

    namespace Container {
        using ftxui::Container::Vertical;
        using ftxui::Container::Horizontal;
        using ftxui::Container::Tab;
        using ftxui::Container::Stacked;
    }
    
    using ftxui::Button;
    using ftxui::Checkbox;
    using ftxui::Input;
    using ftxui::Menu;
    using ftxui::MenuEntry;
    using ftxui::Radiobox;
    using ftxui::Dropdown;
    using ftxui::Toggle;
    using ftxui::Slider;
    using ftxui::ResizableSplit;
    using ftxui::ResizableSplitLeft;
    using ftxui::ResizableSplitRight;
    using ftxui::ResizableSplitTop;
    using ftxui::ResizableSplitBottom;
    using ftxui::Renderer;
    using ftxui::CatchEvent;
    using ftxui::Maybe;
    using ftxui::Modal;
    using ftxui::Collapsible;
    using ftxui::Hoverable;
    using ftxui::Window;
}
