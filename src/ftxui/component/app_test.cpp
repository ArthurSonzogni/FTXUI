// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST, EXPECT_EQ, Message, TestPartResult
#include <csignal>  // for raise, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM
#include <ftxui/component/event.hpp>  // for Event, Event::Custom
#include <tuple>                      // for _Swallow_assign, ignore

#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"  // for Renderer
#include "ftxui/dom/elements.hpp"         // for text, Element

#if defined(__unix__)
#include <fcntl.h>
#include <unistd.h>
#include <array>
#include <cstdio>
#include <ftxui/component/loop.hpp>
#include <string>
#endif

namespace ftxui {

namespace {
#if defined(__unix__)

// Capture the standard output (stdout) to a string.
class StdCapture {
 public:
  explicit StdCapture(std::string* captured) : captured_(captured) {
    if (pipe(pipefd_) != 0) {
      return;
    }
    old_stdout_ = dup(fileno(stdout));
    fflush(stdout);
    dup2(pipefd_[1], fileno(stdout));
    close(pipefd_[1]);  // Close the write end in the parent
  }

  ~StdCapture() {
    fflush(stdout);
    dup2(old_stdout_, fileno(stdout));
    close(old_stdout_);

    char buffer[1024];
    ssize_t count;
    while ((count = read(pipefd_[0], buffer, sizeof(buffer))) > 0) {
      captured_->append(buffer, count);
    }

    close(pipefd_[0]);
  }

  StdCapture(const StdCapture&) = delete;
  StdCapture& operator=(const StdCapture&) = delete;

 private:
  int pipefd_[2]{-1, -1};
  int old_stdout_{-1};
  std::string* const captured_;
};

#endif

bool TestSignal(int signal) {
  int called = 0;
  // The tree of components. This defines how to navigate using the keyboard.
  auto component = Renderer([&] {
    called++;
    std::ignore = std::raise(signal);
    called++;
    return text("");
  });

  auto screen = App::FitComponent();
  screen.Loop(component);

  EXPECT_EQ(called, 2);
  return true;
}
}  // namespace

TEST(App, Signal_SIGTERM) {
  TestSignal(SIGTERM);
}
TEST(App, Signal_SIGSEGV) {
  TestSignal(SIGSEGV);
}
TEST(App, Signal_SIGINT) {
  TestSignal(SIGINT);
}
TEST(App, Signal_SIGILL) {
  TestSignal(SIGILL);
}
TEST(App, Signal_SIGABRT) {
  TestSignal(SIGABRT);
}
TEST(App, Signal_SIGFPE) {
  TestSignal(SIGFPE);
}

// Regression test for:
// https://github.com/ArthurSonzogni/FTXUI/issues/402
TEST(App, PostEventToNonActive) {
  auto screen = App::FitComponent();
  screen.Post(Event::Custom);
}

// Regression test for:
// https://github.com/ArthurSonzogni/FTXUI/issues/402
TEST(App, PostTaskToNonActive) {
  auto screen = App::FitComponent();
  screen.Post([] {});
}

TEST(App, CtrlC) {
  auto screen = App::FitComponent();
  bool called = false;
  auto component = Renderer([&] {
    if (!called) {
      called = true;
      screen.PostEvent(Event::CtrlC);
    }
    return text("");
  });
  screen.Loop(component);
}

TEST(App, CtrlC_Forced) {
  auto screen = App::FitComponent();
  screen.ForceHandleCtrlC(true);
  auto component = Renderer([&] {
    screen.PostEvent(Event::CtrlC);
    return text("");
  });

  int ctrl_c_count = 0;
  component |= CatchEvent([&](Event event) {
    if (event != Event::CtrlC) {
      return false;
    }

    ++ctrl_c_count;

    if (ctrl_c_count == 100) {
      return false;
    }

    return true;
  });
  screen.Loop(component);

  ASSERT_LE(ctrl_c_count, 50);
}

TEST(App, CtrlC_NotForced) {
  auto screen = App::FitComponent();
  screen.ForceHandleCtrlC(false);
  auto component = Renderer([&] {
    screen.PostEvent(Event::CtrlC);
    return text("");
  });

  int ctrl_c_count = 0;
  component |= CatchEvent([&](Event event) {
    if (event != Event::CtrlC) {
      return false;
    }

    ++ctrl_c_count;

    if (ctrl_c_count == 100) {
      return false;
    }

    return true;
  });
  screen.Loop(component);

  ASSERT_GE(ctrl_c_count, 50);
}

// Regression test for:
// https://github.com/ArthurSonzogni/FTXUI/pull/1064/files
TEST(App, FixedSizeInitialFrame) {
#if defined(__unix__)
  bool is_tty = isatty(STDIN_FILENO);
  if (!is_tty) {
    int fd = open("/dev/tty", O_RDWR | O_NOCTTY);
    if (fd >= 0) {
      is_tty = true;
      close(fd);
    }
  }

  std::string output;
  {
    auto capture = StdCapture(&output);

    auto screen = App::FixedSize(2, 2);
    auto component = Renderer([&] { return text("AB"); });

    Loop loop(&screen, component);
    loop.RunOnce();
  }
  using namespace std::string_literals;

  std::string expected;
  // Install the App.
  expected += "\0"s;              // Flush stdout.
  if (is_tty) {
    expected += "\x1BP$q q\x1B\\"s; // Query cursor shape.
  }
  expected += "\x1B[?7l"s;        // Disable line wrapping.
  expected += "\x1B[?1000h"s;     // Enable mouse tracking.
  expected += "\x1B[?1003h"s;     // Enable mouse motion tracking.
  expected += "\x1B[?1015h"s;     // Enable mouse wheel tracking.
  expected += "\x1B[?1006h"s;     // Enable SGR mouse tracking.
  expected += "\0"s;              // Flush stdout.

  // Reset the screen.
  expected += "\x1B[?25l"s;  // Hide cursor.
  if (is_tty) {
    expected += "\x1B[6n"s;    // Query cursor position.
  }

  // Print the document.
  expected += "AB\r\n"s;  // Print "AB" and move to the next line.
  expected += "  "s;      // Print two spaces to fill the line.

  // Set cursor position.
  expected += "\x1B[1D"s;  // Move cursor left one character.

  // Flush
  expected += "\0"s;  // Flush stdout.

  // Uninstall the App.
  expected += "\x1B[1C"s;      // Move cursor right one character.
  expected += "\x1B[?1006l"s;  // Disable SGR mouse tracking.
  expected += "\x1B[?1015l"s;  // Disable mouse wheel tracking.
  expected += "\x1B[?1003l"s;  // Disable mouse motion tracking.
  expected += "\x1B[?1000l"s;  // Disable mouse tracking.
  expected += "\x1B[?7h"s;     // Enable line wrapping.
  expected += "\x1B[?25h"s;    // Show cursor.
  expected += "\x1B[1 q"s;     // Set cursor shape to 1 (block).
  expected += "\0"s;           // Flush stdout.

  // Skip one line to avoid the prompt to be printed over the last drawing.
  expected += "\r\n"s;
  ASSERT_EQ(expected, output);
#endif
}

}  // namespace ftxui
