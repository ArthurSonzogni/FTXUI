#include <iostream>
#include <thread>

#include "ftxui/component/component_vertical.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui::component;
using namespace ftxui::dom;

class MyComponent : ComponentVertical {
  public:
   MyComponent(Delegate* delegate)
       : ComponentVertical(delegate),
         toggle(delegate->NewChild()),
         menu(delegate->NewChild()) {

     toggle.options = {L" left ", L" middle ", L" end "};
     menu.entries = {L" top ", L" middle ", L" bottom "};
     Focus(&toggle);
   }

   std::function<void()> on_enter = [](){};
  private:
   Toggle toggle;
   Menu menu;

   Element Render() override {
     return
       vbox(
        hbox(frame(toggle.Render()), filler()),
        frame(menu.Render()));
   }
};

 
int main(int argc, const char *argv[])
{
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
