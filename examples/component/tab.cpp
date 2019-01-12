#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
  public:
   MyComponent() {
     Add(&container);
     container.Add(&toggle);
     container.Add(&menu);

     toggle.options = {L" left ", L" middle ", L" end "};
     menu.entries = {L" top ", L" middle ", L" bottom "};
   }

   std::function<void()> on_enter = [](){};
  private:
   Container container = Container::Vertical();
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
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}
