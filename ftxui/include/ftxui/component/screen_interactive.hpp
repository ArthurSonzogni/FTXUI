#ifndef FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP
#define FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP

#include "ftxui/screen/screen.hpp"
#include <functional>
#include <memory>

namespace ftxui::component {

class Delegate;
class Component;

class ScreenInteractive : public ftxui::screen::Screen {
  public:
    static ScreenInteractive FixedSize(size_t dimx, size_t dimy);
    static ScreenInteractive Fullscreen();
    static ScreenInteractive TerminalOutput();

    ~ScreenInteractive();
    component::Delegate* delegate();
    void Loop();
    std::function<void()> ExitLoopClosure();

  private:
   class Delegate;
   std::unique_ptr<Delegate> delegate_;

   void PrepareDraw();
   bool quit_ = false;

   enum class Dimension {
     Fixed,
     TerminalOutput,
     Fullscreen,
   };
   Dimension dimension_ = Dimension::Fixed;

   ScreenInteractive(size_t dimx, size_t dimy, Dimension dimension);
};

}  // namespace ftxui::component

#endif /* end of include guard: FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP */
