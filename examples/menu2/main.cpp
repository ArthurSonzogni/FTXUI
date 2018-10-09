#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen_interactive.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/component_horizontal.hpp"
#include "ftxui/component/component_vertical.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui::component;
using namespace ftxui::dom;

class MyComponent : ComponentHorizontal {
  public:
   MyComponent(ftxui::component::Delegate* delegate)
       : ComponentHorizontal(delegate),
         left_menu(delegate->NewChild()),
         right_menu(delegate->NewChild()) {
     left_menu.entries = {L"0%",  L"10%", L"20%", L"30%", L"40%", L"50%",
                          L"60%", L"70%", L"80%", L"90%"};
     right_menu.entries = {L"0%",  L"1%", L"2%", L"3%", L"4%", L"5%",
                           L"6%", L"7%", L"8%", L"9%", L"10%"};

     left_menu.on_enter = [this]() { on_enter(); };
     right_menu.on_enter = [this]() { on_enter(); };
     Focus(&left_menu);
   }

   std::function<void()> on_enter = [](){};
  private:
   Menu left_menu;
   Menu right_menu;

   Element Render() override {
     int sum = left_menu.selected * 10 + right_menu.selected;
     return
       frame(
         vbox(
           // -------- Top panel --------------
           hbox(
             // -------- Left Menu --------------
             flex(
               vbox(
                 hcenter(bold(text(L"Percentage by 10%"))),
                 left_menu.Render()
               )
             ),
             // -------- Right Menu --------------
             flex(
               vbox(
                 hcenter(bold(text(L"Percentage by 1%"))),
                 right_menu.Render()
               )
             ),
             flex()
           ),
           separator(),
           // -------- Bottom panel --------------
           flex(vbox(
             hbox(text(L" gauge : "), gauge(sum/100.0)),
             hbox(text(L"  text : "), text(to_wstring(std::to_string(sum) + " %")))
           ))
         )
       );
   }
};

int main(int argc, const char *argv[])
{
  ftxui::ScreenInteractive screen(60,17);
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
