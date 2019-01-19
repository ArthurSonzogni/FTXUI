#ifndef FTXUI_UTIL_AUTORESET_HPP
#define FTXUI_UTIL_AUTORESET_HPP

namespace ftxui {

template<typename T>
class AutoReset {
 public:
  AutoReset(T* variable, T new_value)
      : variable_(variable),
        previous_value_(std::move(*variable)) {
    *variable_ = std::move(new_value);
  }
  ~AutoReset() { *variable_ = std::move(previous_value_); }
 private:
  T* variable_;
  T previous_value_;
};

} // namespace ftxui

#endif /* end of include guard: FTXUI_UTIL_AUTORESET_HPP */
