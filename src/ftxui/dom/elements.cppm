/// @module ftxui.dom.elements
/// @brief Module file for the Element classes and functions of the Dom module

module;

#include <ftxui/dom/elements.hpp>

export module ftxui.dom.elements;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::Node;
    using ftxui::Element;
    using ftxui::Elements;
    using ftxui::Decorator;
    using ftxui::GraphFunction;

    using ftxui::BorderStyle;

    using ftxui::operator|;
    using ftxui::operator|=;

    using ftxui::text;
    using ftxui::vtext;
    using ftxui::separator;
    using ftxui::separatorLight;
    using ftxui::separatorDashed;
    using ftxui::separatorHeavy;
    using ftxui::separatorDouble;
    using ftxui::separatorEmpty;
    using ftxui::separatorStyled;
    using ftxui::separatorCharacter;
    using ftxui::separatorHSelector;
    using ftxui::separatorVSelector;
    using ftxui::gauge;
    using ftxui::gaugeLeft;
    using ftxui::gaugeRight;
    using ftxui::gaugeUp;
    using ftxui::gaugeDown;
    using ftxui::gaugeDirection;
    using ftxui::border;
    using ftxui::borderLight;
    using ftxui::borderDashed;
    using ftxui::borderHeavy;
    using ftxui::borderDouble;
    using ftxui::borderRounded;
    using ftxui::borderEmpty;
    using ftxui::borderStyled;
    using ftxui::borderWith;
    using ftxui::window;
    using ftxui::spinner;
    using ftxui::paragraph;
    using ftxui::paragraphAlignLeft;
    using ftxui::paragraphAlignRight;
    using ftxui::paragraphAlignCenter;
    using ftxui::paragraphAlignJustify;
    using ftxui::graph;
    using ftxui::emptyElement;
    using ftxui::canvas;

    using ftxui::bold;
    using ftxui::dim;
    using ftxui::italic;
    using ftxui::inverted;
    using ftxui::underlined;
    using ftxui::underlinedDouble;
    using ftxui::blink;
    using ftxui::strikethrough;
    using ftxui::color;
    using ftxui::bgcolor;
    using ftxui::focusPosition;
    using ftxui::focusPositionRelative;
    using ftxui::automerge;
    using ftxui::hyperlink;
    using ftxui::selectionStyleReset;
    using ftxui::selectionColor;
    using ftxui::selectionBackgroundColor;
    using ftxui::selectionForegroundColor;
    using ftxui::selectionStyle;

    using ftxui::hbox;
    using ftxui::vbox;
    using ftxui::dbox;
    using ftxui::flexbox;
    using ftxui::gridbox;
    using ftxui::hflow;
    using ftxui::vflow;

    using ftxui::flex;
    using ftxui::flex_grow;
    using ftxui::flex_shrink;
    using ftxui::xflex;
    using ftxui::xflex_grow;
    using ftxui::xflex_shrink;
    using ftxui::yflex;
    using ftxui::yflex_grow;
    using ftxui::yflex_shrink;
    using ftxui::notflex;
    using ftxui::filler;

    using ftxui::WidthOrHeight;
    using ftxui::Constraint;
    using ftxui::size;

    using ftxui::frame;
    using ftxui::xframe;
    using ftxui::yframe;
    using ftxui::focus;
    using ftxui::select;

    using ftxui::focusCursorBlock;
    using ftxui::focusCursorBlockBlinking;
    using ftxui::focusCursorBar;
    using ftxui::focusCursorBarBlinking;
    using ftxui::focusCursorUnderline;
    using ftxui::focusCursorUnderlineBlinking;

    using ftxui::vscroll_indicator;
    using ftxui::hscroll_indicator;
    using ftxui::reflect;
    using ftxui::clear_under;

    using ftxui::hcenter;
    using ftxui::vcenter;
    using ftxui::center;
    using ftxui::align_right;
    using ftxui::nothing;

    namespace Dimension {
        using ftxui::Dimension::Fit;
    }
}
