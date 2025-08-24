// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)

namespace ftxui {

namespace {

// Test fixture for piped input functionality
class PipedInputTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Save original stdin for restoration
    original_stdin_ = dup(STDIN_FILENO);
  }

  void TearDown() override {
    // Restore original stdin
    if (original_stdin_ >= 0) {
      dup2(original_stdin_, STDIN_FILENO);
      close(original_stdin_);
    }
  }

  // Create a pipe and redirect stdin to read from it
  void SetupPipedStdin() {
    if (pipe(pipe_fds_) == 0) {
      dup2(pipe_fds_[0], STDIN_FILENO);
      close(pipe_fds_[0]);
      // Keep write end open for writing test data
      piped_stdin_setup_ = true;
    }
  }

  // Write test data to the piped stdin
  void WriteToPipedStdin(const std::string& data) {
    if (piped_stdin_setup_) {
      write(pipe_fds_[1], data.c_str(), data.length());
      close(pipe_fds_[1]);  // Close write end to signal EOF
    }
  }

  // Check if /dev/tty is available (not available in some CI environments)
  bool IsTtyAvailable() {
    struct stat st;
    return stat("/dev/tty", &st) == 0;
  }

 private:
  int original_stdin_ = -1;
  int pipe_fds_[2] = {-1, -1};
  bool piped_stdin_setup_ = false;
};

TEST_F(PipedInputTest, DefaultBehaviorEnabled) {
  // Test that HandlePipedInput is enabled by default
  if (!IsTtyAvailable()) {
    GTEST_SKIP() << "/dev/tty not available in this environment";
  }

  auto screen = ScreenInteractive::TerminalOutput();
  auto component = Renderer([] { return text("test"); });

  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  // Install should redirect stdin since HandlePipedInput is on by default
  screen.Install();

  // Stdin should be the tty
  EXPECT_TRUE(isatty(STDIN_FILENO));

  screen.Uninstall();
}

TEST_F(PipedInputTest, ExplicitlyDisabled) {
  // Test that explicitly disabling works
  auto screen = ScreenInteractive::TerminalOutput();
  screen.HandlePipedInput(false);
  auto component = Renderer([] { return text("test"); });

  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  screen.Install();

  // Stdin should still be the pipe since feature is disabled
  EXPECT_FALSE(isatty(STDIN_FILENO));

  screen.Uninstall();
}

TEST_F(PipedInputTest, ExplicitlyEnabled) {
  if (!IsTtyAvailable()) {
    GTEST_SKIP() << "/dev/tty not available in this environment";
  }

  auto screen = ScreenInteractive::TerminalOutput();
  screen.HandlePipedInput(true);  // Explicitly enable
  auto component = Renderer([] { return text("test"); });

  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  // Before install: stdin should be piped
  EXPECT_FALSE(isatty(STDIN_FILENO));

  screen.Install();

  // After install with piped input handling: stdin should be redirected to tty
  EXPECT_TRUE(isatty(STDIN_FILENO));

  screen.Uninstall();

  // After uninstall: stdin should be restored to original state
  // Note: This will be the pipe we set up, so it should be non-tty
  EXPECT_FALSE(isatty(STDIN_FILENO));
}

TEST_F(PipedInputTest, NormalStdinUnchanged) {
  // Test that normal stdin (not piped) is not affected
  auto screen = ScreenInteractive::TerminalOutput();
  auto component = Renderer([] { return text("test"); });

  // Don't setup piped stdin - use normal stdin
  bool original_isatty = isatty(STDIN_FILENO);

  screen.Install();

  // Stdin should remain unchanged
  EXPECT_EQ(original_isatty, isatty(STDIN_FILENO));

  screen.Uninstall();

  // Stdin should still be unchanged
  EXPECT_EQ(original_isatty, isatty(STDIN_FILENO));
}

TEST_F(PipedInputTest, MultipleInstallUninstallCycles) {
  if (!IsTtyAvailable()) {
    GTEST_SKIP() << "/dev/tty not available in this environment";
  }

  auto screen = ScreenInteractive::TerminalOutput();
  auto component = Renderer([] { return text("test"); });

  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  // First cycle
  screen.Install();
  EXPECT_TRUE(isatty(STDIN_FILENO));
  screen.Uninstall();
  EXPECT_FALSE(isatty(STDIN_FILENO));

  // Second cycle should work the same
  screen.Install();
  EXPECT_TRUE(isatty(STDIN_FILENO));
  screen.Uninstall();
  EXPECT_FALSE(isatty(STDIN_FILENO));
}

TEST_F(PipedInputTest, HandlePipedInputMethodBehavior) {
  auto screen = ScreenInteractive::TerminalOutput();

  // Test method can be called multiple times
  screen.HandlePipedInput(true);
  screen.HandlePipedInput(false);
  screen.HandlePipedInput(true);

  // Should be enabled after last call
  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  if (IsTtyAvailable()) {
    screen.Install();
    EXPECT_TRUE(isatty(STDIN_FILENO));
    screen.Uninstall();
  }
}

// Test the graceful fallback when /dev/tty is not available
// This test simulates environments like containers where /dev/tty might not
// exist
TEST_F(PipedInputTest, GracefulFallbackWhenTtyUnavailable) {
  auto screen = ScreenInteractive::TerminalOutput();
  auto component = Renderer([] { return text("test"); });

  SetupPipedStdin();
  WriteToPipedStdin("test data\n");

  // This test doesn't directly mock /dev/tty unavailability since that's hard
  // to do in a unit test environment, but the code path handles freopen()
  // failure gracefully
  screen.Install();

  // The behavior depends on whether /dev/tty is available
  // If available, stdin gets redirected; if not, it remains piped
  // Both behaviors are correct

  screen.Uninstall();

  // After uninstall, stdin should be restored
  EXPECT_FALSE(isatty(STDIN_FILENO));  // Should still be our test pipe
}

}  // namespace

}  // namespace ftxui

#endif  // !defined(_WIN32) && !defined(__EMSCRIPTEN__)
