// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/util/ref.hpp"

#include <utility>

namespace ftxui {

namespace {

// StringRef, ConstStringRef, and ConstStringListRef are exported
// (FTXUI_EXPORT(SCREEN)), but all of their member functions -- including
// ones inherited from Ref<string>/ConstRef<string> -- are defined inline in
// the header and never odr-used from within libftxui-screen itself. On
// Windows, MSVC only emits (and therefore dllexports) an inline member
// function into the DLL that first odr-uses it; otherwise consumers linking
// against the dllimport declaration get an unresolved external symbol. This
// function exists solely to force that instantiation.
[[maybe_unused]] void ForceSymbolInstantiation() {
  StringRef s = "a";
  StringRef s2(s);
  StringRef s3(std::move(s2));
  s = s3;
  s = std::move(s3);
  (void)s();
  (void)*s;
  (void)s.operator->();

  ConstStringRef c = "a";
  ConstStringRef c2(c);
  ConstStringRef c3(std::move(c2));
  c = c3;
  c = std::move(c3);
  (void)c();
  (void)*c;

  ConstStringListRef l;
  ConstStringListRef l2(l);
  ConstStringListRef l3(std::move(l2));
  l = l3;
  l = std::move(l3);
  (void)l.size();
  (void)l[0];
}

}  // namespace

}  // namespace ftxui
