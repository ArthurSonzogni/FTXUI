#ifndef FTXUI_SCREEN_INTERACTIVE
#define FTXUI_SCREEN_INTERACTIVE

#include "ftxui/screen.hpp"
#include <functional>
#include <memory>

namespace ftxui {

namespace component {
  class Delegate;
  class Component;
} // namespace component

class ScreenInteractive : public Screen {
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

	 void Clear();
	 void Draw();
	 bool quit_ = false;

   enum class Dimension {
     Fixed,
     TerminalOutput,
     Fullscreen,
   };
   Dimension dimension_ = Dimension::Fixed;

   ScreenInteractive(size_t dimx, size_t dimy, Dimension dimension);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_INTERACTIVE */
