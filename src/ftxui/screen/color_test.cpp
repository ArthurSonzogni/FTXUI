// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/color.hpp"
#include <gtest/gtest.h>
#include <cstdlib>  // for std::getenv, setenv, unsetenv
#include <optional>
#include <string>
#include "ftxui/screen/terminal.hpp"

namespace ftxui {

#if !defined(_WIN32)
namespace {
// Clear an environment variable for the duration of a scope, restoring its
// original value on destruction. ComputeColorSupport reads NO_COLOR and
// WT_SESSION from the environment; tests must not depend on the machine
// running them.
class ScopedClearEnv {
 public:
  explicit ScopedClearEnv(const char* name) : name_(name) {
    const char* value = std::getenv(name);  // NOLINT
    if (value != nullptr) {
      saved_ = value;
    }
    unsetenv(name);
  }
  ~ScopedClearEnv() {
    if (saved_) {
      setenv(name_, saved_->c_str(), 1);
    } else {
      unsetenv(name_);
    }
  }
  ScopedClearEnv(const ScopedClearEnv&) = delete;
  ScopedClearEnv& operator=(const ScopedClearEnv&) = delete;

 private:
  const char* name_;
  std::optional<std::string> saved_;
};
}  // namespace
#endif

TEST(ColorTest, PrintTransparent) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color().Print(false), "39");
  EXPECT_EQ(Color().Print(true), "49");
}

TEST(ColorTest, PrintColor16) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color(Color::Red).Print(false), "31");
  EXPECT_EQ(Color(Color::Red).Print(true), "41");
}

TEST(ColorTest, PrintColor256) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color(Color::DarkRed).Print(false), "38;5;52");
  EXPECT_EQ(Color(Color::DarkRed).Print(true), "48;5;52");
}

TEST(ColorTest, PrintTrueCOlor) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "38;2;1;2;3");
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(true), "48;2;1;2;3");
}

TEST(ColorTest, FallbackTo256) {
  Terminal::SetColorSupport(Terminal::Color::Palette256);
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "38;5;16");
}

TEST(ColorTest, FallbackTo16) {
  Terminal::SetColorSupport(Terminal::Color::Palette16);
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "30");
}

TEST(ColorTest, Literals) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  using namespace ftxui::literals;
  EXPECT_EQ(Color(0xABCDEF_rgb).Print(false), "38;2;171;205;239");
}

TEST(ColorTest, Interpolate) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);

  EXPECT_EQ(Color::Interpolate(0.3f, Color(), Color()).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.3f, Color::Red, Color()).Print(false), "31");
  EXPECT_EQ(Color::Interpolate(0.7f, Color::Red, Color()).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.3f, Color(), Color::Red).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.7f, Color(), Color::Red).Print(false), "31");

  EXPECT_EQ(Color::Interpolate(0.3f,                       //
                               Color::RGB(1, 2, 3),        //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;141;141;71");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color::RGB(1, 2, 3),        //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;207;207;104");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color(Color::Red),          //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;216;207;104");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color::RGB(244, 244, 123),  //
                               Color(Color::Plum1))        //
                .Print(false),
            "38;2;251;198;225");
}

TEST(ColorTest, HSV) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color::HSV(0, 255, 255).Print(false), "38;2;255;0;0");
}

TEST(ColorTest, ComputeColorSupport) {
#if !defined(_WIN32)
  const ScopedClearEnv no_color("NO_COLOR");
  const ScopedClearEnv wt_session("WT_SESSION");

  EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "256", "", "", "", {}),
            Terminal::Color::Palette256);
  EXPECT_EQ(Terminal::ComputeColorSupport("xterm-256color", "", "", "", "", {}),
            Terminal::Color::Palette256);

  // An unidentified terminal without any hint only gets Palette16.
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "unknown", "unknown", {}),
            Terminal::Color::Palette16);
  // An empty terminal name is treated as unidentified.
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "", "", {}),
            Terminal::Color::Palette16);

  // Apple's Terminal.app supports 256 colors, but not 24bit ones.
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "Apple_Terminal", "unknown",
                                          "unknown", {}),
            Terminal::Color::Palette256);
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "iTerm.app", "unknown",
                                          "unknown", {}),
            Terminal::Color::TrueColor);
#endif
  EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "truecolor", "", "", "", {}),
            Terminal::Color::TrueColor);
  EXPECT_EQ(Terminal::ComputeColorSupport("kitty", "", "", "", "", {}),
            Terminal::Color::TrueColor);
}

#if !defined(_WIN32)
TEST(ColorTest, ComputeColorSupportWTSession) {
  const ScopedClearEnv no_color("NO_COLOR");
  const ScopedClearEnv wt_session("WT_SESSION");

  // Without WT_SESSION, an unidentified terminal only gets Palette16.
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "unknown", "unknown", {}),
            Terminal::Color::Palette16);

  // Windows Terminal sets WT_SESSION, including when running under WSL.
  setenv("WT_SESSION", "test_session_id", 1);
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "unknown", "unknown", {}),
            Terminal::Color::TrueColor);
}

TEST(ColorTest, ComputeColorSupportNoColor) {
  const ScopedClearEnv no_color("NO_COLOR");
  const ScopedClearEnv wt_session("WT_SESSION");

  // NO_COLOR (https://no-color.org) disables colors, even on a TrueColor
  // capable terminal.
  setenv("NO_COLOR", "1", 1);
  EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "truecolor", "", "unknown",
                                          "unknown", {}),
            Terminal::Color::Palette1);

  // NO_COLOR takes precedence over WT_SESSION.
  setenv("WT_SESSION", "test_session_id", 1);
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "unknown", "unknown", {}),
            Terminal::Color::Palette1);

  // An empty NO_COLOR is the same as an unset one.
  setenv("NO_COLOR", "", 1);
  unsetenv("WT_SESSION");
  EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "truecolor", "", "unknown",
                                          "unknown", {}),
            Terminal::Color::TrueColor);
}
#endif

TEST(ColorTest, FallbackToPalette1) {
  Terminal::SetColorSupport(Terminal::Color::Palette1);
  // Every color degrades to the terminal's default colors.
  EXPECT_EQ(Color(Color::Red).Print(false), "39");
  EXPECT_EQ(Color(Color::Red).Print(true), "49");
  EXPECT_EQ(Color(Color::DarkRed).Print(false), "39");
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "39");
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(true), "49");
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
}

}  // namespace ftxui
