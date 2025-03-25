/**
 * @file Receiver.cppm
 * @brief Module file for the Receiver class of the Component module
 */

module;

#include <ftxui/component/receiver.hpp>

export module ftxui.component.Receiver;

/**
 * @namespace ftxui
 * @brief The FTXUI ftxui:: namespace
 */
export namespace ftxui {
    using ftxui::SenderImpl;
    using ftxui::ReceiverImpl;
    using ftxui::Sender;
    using ftxui::Receiver;
    using ftxui::MakeReceiver;
}