#ifndef FTXUI_CORE_TERMINAL_HPP
#define FTXUI_CORE_TERMINAL_HPP

namespace ftxui {

class Terminal {
 public:
  struct Dimensions {
    int dimx;
    int dimy;
  };

  static Dimensions Size();
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_CORE_TERMINAL_HPP */
