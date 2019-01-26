#ifndef FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP
#define FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP

#include "ftxui/screen/screen.hpp"
#include <functional>
#include <memory>

namespace ftxui {
class Component;

class ScreenInteractive : public Screen {
  public:
    static ScreenInteractive FixedSize(int dimx, int dimy);
    static ScreenInteractive Fullscreen();
    static ScreenInteractive FitComponent();
    static ScreenInteractive TerminalOutput();

    ~ScreenInteractive();
    void Loop(Component*);
    std::function<void()> ExitLoopClosure();

  private:
   void Draw(Component* component);
   bool quit_ = false;

   enum class Dimension {
     FitComponent,
     Fixed,
     Fullscreen,
     TerminalOutput,
   };
   Dimension dimension_ = Dimension::Fixed;
   ScreenInteractive(int dimx, int dimy, Dimension dimension);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP */
