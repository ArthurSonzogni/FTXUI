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
     Add(&container_);
     container_.Add(&toggle_);

     toggle_.entries = {
       L"menu_1",
       L"menu_2",
       L"menu_3",
     };

     container_.Add(&tab_container_);
    
     menu_1_.entries = {
       L"Forest",
       L"Water",
       L"I don't know"
     };
     tab_container_.Add(&menu_1_);

     menu_2_.entries = {
       L"Hello",
       L"Hi",
       L"Hay",
     };
     tab_container_.Add(&menu_2_);

     menu_3_.entries = {
       L"Table",
       L"Nothing",
       L"Is",
       L"Empty",
     };
     tab_container_.Add(&menu_3_);
   }

   std::function<void()> on_enter = [](){};
  private:
   Toggle toggle_;
   Container container_ = Container::Vertical();
   Container tab_container_ = Container::Tab(&(toggle_.selected));
   Menu menu_1_;
   Menu menu_2_;
   Menu menu_3_;
};

 
int main(int argc, const char *argv[])
{
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}
