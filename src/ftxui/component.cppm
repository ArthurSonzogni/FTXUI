/// @module ftxui.component
/// @brief Module file for FTXUI component operations.

module;

#include <ftxui/component/animation.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/task.hpp>

export module ftxui.component;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::AnimatedColorOption;
    using ftxui::AnimatedColorsOption;
    using ftxui::AnimationTask;
    using ftxui::App;
    using ftxui::ButtonOption;
    using ftxui::CapturedMouseInterface;
    using ftxui::CheckboxOption;
    using ftxui::Closure;
    using ftxui::Component;
    using ftxui::ComponentBase;
    using ftxui::Components;
    using ftxui::Delegate;
    using ftxui::DropdownOption;
    using ftxui::EntryState;
    using ftxui::Event;
    using ftxui::Focus;
    using ftxui::InputOption;
    using ftxui::InputState;
    using ftxui::Loop;
    using ftxui::MenuEntryOption;
    using ftxui::MenuOption;
    using ftxui::Mouse;
    using ftxui::RadioboxOption;
    using ftxui::ResizableSplitOption;
    using ftxui::Screen;
    using ftxui::SliderOption;
    using ftxui::Task;
    using ftxui::UnderlineOption;
    using ftxui::WindowOptions;
    using ftxui::WindowRenderState;

    using ftxui::Make;

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

    // Deprecated
    using ftxui::ReceiverImpl;
    using ftxui::ScreenInteractive;
    using ftxui::SenderImpl;
    using ftxui::Receiver;
    using ftxui::Sender;
    using ftxui::MakeReceiver;

    using ftxui::operator|;
    using ftxui::operator|=;

    /**
     * @namespace Container
     * @brief The FTXUI ftxui::Container:: namespace
     */
    namespace Container {
        using ftxui::Container::Vertical;
        using ftxui::Container::Horizontal;
        using ftxui::Container::Tab;
        using ftxui::Container::Stacked;
    }

    /**
     * @namespace animation
     * @brief The FTXUI ftxui::animation:: namespace
     */
    namespace animation {
        using ftxui::animation::Animator;
        using ftxui::animation::Clock;
        using ftxui::animation::Duration;
        using ftxui::animation::Params;
        using ftxui::animation::TimePoint;

        using ftxui::animation::RequestAnimationFrame;

        /**
         * @namespace easing
         * @brief The FTXUI ftxui::animation::easing:: namespace
         */
        namespace easing {
            using ftxui::animation::easing::Function;

            using ftxui::animation::easing::Linear;
            using ftxui::animation::easing::QuadraticIn;
            using ftxui::animation::easing::QuadraticOut;
            using ftxui::animation::easing::QuadraticInOut;
            using ftxui::animation::easing::CubicIn;
            using ftxui::animation::easing::CubicOut;
            using ftxui::animation::easing::CubicInOut;
            using ftxui::animation::easing::QuarticIn;
            using ftxui::animation::easing::QuarticOut;
            using ftxui::animation::easing::QuarticInOut;
            using ftxui::animation::easing::QuinticIn;
            using ftxui::animation::easing::QuinticOut;
            using ftxui::animation::easing::QuinticInOut;
            using ftxui::animation::easing::SineIn;
            using ftxui::animation::easing::SineOut;
            using ftxui::animation::easing::SineInOut;
            using ftxui::animation::easing::CircularIn;
            using ftxui::animation::easing::CircularOut;
            using ftxui::animation::easing::CircularInOut;
            using ftxui::animation::easing::ExponentialIn;
            using ftxui::animation::easing::ExponentialOut;
            using ftxui::animation::easing::ExponentialInOut;
            using ftxui::animation::easing::ElasticIn;
            using ftxui::animation::easing::ElasticOut;
            using ftxui::animation::easing::ElasticInOut;
            using ftxui::animation::easing::BackIn;
            using ftxui::animation::easing::BackOut;
            using ftxui::animation::easing::BackInOut;
            using ftxui::animation::easing::BounceIn;
            using ftxui::animation::easing::BounceOut;
            using ftxui::animation::easing::BounceInOut;
        }
    }
}
