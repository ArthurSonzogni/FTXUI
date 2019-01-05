#include <iostream>
#include <thread>

#include "ftxui/component/component_horizontal.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/screen_interactive.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui;
using namespace ftxui::component;
using namespace ftxui::dom;

class MyComponent : ComponentHorizontal {
  public:
   MyComponent(ftxui::component::Delegate* delegate)
       : ComponentHorizontal(delegate),
         menu_1(delegate->NewChild()),
         menu_2(delegate->NewChild()),
         menu_3(delegate->NewChild()),
         menu_4(delegate->NewChild()),
         menu_5(delegate->NewChild()),
         menu_6(delegate->NewChild()) {

     for(Menu* menu : {&menu_1, &menu_2, &menu_3, &menu_4, &menu_5, &menu_6}) {
       menu->entries = {
         L"Monkey",
         L"Dog",
         L"Cat",
         L"Bird",
         L"Elephant",
       };
       menu->on_enter = [this]() { on_enter(); };
     }

     menu_2.selected_style = color(Color::Blue);
     menu_2.active_style = bold | color(Color::Blue);

     menu_3.selected_style = color(Color::Blue);
     menu_3.active_style = bgcolor(Color::Blue);

     menu_4.selected_style = bgcolor(Color::Blue);
     menu_4.active_style = bgcolor(Color::BlueLight);

     menu_5.normal_style = bgcolor(Color::Blue);
     menu_5.selected_style = bgcolor(Color::Yellow);
     menu_5.active_style = bgcolor(Color::Red);

     menu_6.normal_style = dim | color(Color::Blue);
     menu_6.selected_style = color(Color::Blue);
     menu_6.active_style = bold | color(Color::Blue);

     Focus(&menu_1);
   }

   std::function<void()> on_enter = [](){};
  private:
   Menu menu_1;
   Menu menu_2;
   Menu menu_3;
   Menu menu_4;
   Menu menu_5;
   Menu menu_6;

   Element Render() override {
     return
        hbox(
          menu_1.Render() | flex, separator(),
          menu_2.Render() | flex, separator(),
          menu_3.Render() | flex, separator(),
          menu_4.Render() | flex, separator(),
          menu_5.Render() | flex, separator(),
          menu_6.Render() | flex
        ) | frame;
   }
};

int main(int argc, const char *argv[])
{
  //auto screen = ftxui::ScreenInteractive::TerminalOutput();
  auto screen = ftxui::ScreenInteractive::Fullscreen();
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
