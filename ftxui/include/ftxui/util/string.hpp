#include <string>

std::string to_string(const std::wstring& s);
std::wstring to_wstring(const std::string& s);

template<typename T>
std::wstring to_wstring(T s) {
  return to_wstring(std::to_string(s));
}
