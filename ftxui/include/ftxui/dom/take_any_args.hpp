#include <type_traits>

// Turn a set of arguments into a vector.
template <class... Args>
Elements unpack(Args... args) {
  using T = std::common_type_t<Args...>;
  std::vector<T> vec;
  (vec.push_back(std::forward<Args>(args)), ...);
  return vec;
}

// Make |container| able to take any number of argments.
#define TAKE_ANY_ARGS(container) \
  template <class... Args> \
  Element container(Args... children) { \
    return container(unpack(std::forward<Args>(children)...)); \
  } \

