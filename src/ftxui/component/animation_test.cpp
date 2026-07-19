// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <gtest/gtest.h>
#include <chrono>      // for milliseconds
#include <functional>  // for function
#include <thread>      // for sleep_for
#include <vector>      // for allocator, vector

#include "ftxui/component/animation.hpp"  // for Function, BackIn, BackInOut, BackOut, BounceIn, BounceInOut, BounceOut, CircularIn, CircularInOut, CircularOut, CubicIn, CubicInOut, CubicOut, ElasticIn, ElasticInOut, ElasticOut, ExponentialIn, ExponentialInOut, ExponentialOut, Linear, QuadraticIn, QuadraticInOut, QuadraticOut, QuarticIn, QuarticInOut, QuarticOut, QuinticIn, QuinticInOut, QuinticOut, SineIn, SineInOut, SineOut
#include "ftxui/component/app.hpp"        // for App
#include "ftxui/component/component.hpp"  // for Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/loop.hpp"            // for Loop
#include "ftxui/dom/elements.hpp"              // for text

namespace ftxui {

TEST(AnimationTest, StartAndEnd) {
  const std::vector<animation::easing::Function> functions = {
      animation::easing::Linear,         animation::easing::QuadraticIn,
      animation::easing::QuadraticOut,   animation::easing::QuadraticInOut,
      animation::easing::CubicIn,        animation::easing::CubicOut,
      animation::easing::CubicInOut,     animation::easing::QuarticIn,
      animation::easing::QuarticOut,     animation::easing::QuarticInOut,
      animation::easing::QuinticIn,      animation::easing::QuinticOut,
      animation::easing::QuinticInOut,   animation::easing::SineIn,
      animation::easing::SineOut,        animation::easing::SineInOut,
      animation::easing::CircularIn,     animation::easing::CircularOut,
      animation::easing::CircularInOut,  animation::easing::ExponentialIn,
      animation::easing::ExponentialOut, animation::easing::ExponentialInOut,
      animation::easing::ElasticIn,      animation::easing::ElasticOut,
      animation::easing::ElasticInOut,   animation::easing::BackIn,
      animation::easing::BackOut,        animation::easing::BackInOut,
      animation::easing::BounceIn,       animation::easing::BounceOut,
      animation::easing::BounceInOut,
  };
  for (const auto& it : functions) {
    EXPECT_NEAR(0.F, it(0.F), 1.0e-4);
    EXPECT_NEAR(1.F, it(1.F), 1.0e-4);
  }
}

namespace {

class AnimationCounter : public ComponentBase {
 public:
  int count = 0;

 private:
  Element OnRender() override { return text(""); }
  void OnAnimation(animation::Params& /*params*/) override { count++; }
};

// Give the recurring AnimationTask (reposted every 15ms) time to become due,
// then run the loop to execute it.
void RunAnimationTask(Loop& loop) {
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  loop.RunOnce();
}

}  // namespace

// OnAnimation must tick once per handled event, and not tick for the
// terminal's replies to FTXUI's own internal requests. See #1302.
TEST(AnimationTest, EventRequestsAnimationFrame) {
  auto component = Make<AnimationCounter>();
  auto screen = App::FixedSize(10, 1);
  Loop loop(&screen, component);
  loop.RunOnce();

  // No event: the recurring AnimationTask is a no-op.
  RunAnimationTask(loop);
  EXPECT_EQ(component->count, 0);

  // A handled event grants exactly one animation tick.
  screen.PostEvent(Event::Custom);
  loop.RunOnce();
  RunAnimationTask(loop);
  EXPECT_EQ(component->count, 1);
  RunAnimationTask(loop);
  EXPECT_EQ(component->count, 1);

  // An internal terminal reply must not grant one.
  screen.PostEvent(Event::CursorPosition("", 1, 1));
  loop.RunOnce();
  RunAnimationTask(loop);
  EXPECT_EQ(component->count, 1);
}

}  // namespace ftxui
