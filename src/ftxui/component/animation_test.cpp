#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult
#include <memory>  // for shared_ptr, __shared_ptr_access, allocator, make_shared

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "gtest/gtest_pred_impl.h"  // for EXPECT_EQ, Test, SuiteApiResolver, TEST, TestFactoryImpl

namespace ftxui {

TEST(AnimationTest, StartAndEnd) {
  std::vector<animation::easing::Function> functions = {
    animation::easing::Linear,
    animation::easing::QuadraticIn,
    animation::easing::QuadraticOut,
    animation::easing::QuadraticInOut,
    animation::easing::CubicIn,
    animation::easing::CubicOut,
    animation::easing::CubicInOut,
    animation::easing::QuarticIn,
    animation::easing::QuarticOut,
    animation::easing::QuarticInOut,
    animation::easing::QuinticIn,
    animation::easing::QuinticOut,
    animation::easing::QuinticInOut,
    animation::easing::SineIn,
    animation::easing::SineOut,
    animation::easing::SineInOut,
    animation::easing::CircularIn,
    animation::easing::CircularOut,
    animation::easing::CircularInOut,
    animation::easing::ExponentialIn,
    animation::easing::ExponentialOut,
    animation::easing::ExponentialInOut,
    animation::easing::ElasticIn,
    animation::easing::ElasticOut,
    animation::easing::ElasticInOut,
    animation::easing::BackIn,
    animation::easing::BackOut,
    animation::easing::BackInOut,
    animation::easing::BounceIn,
    animation::easing::BounceOut,
    animation::easing::BounceInOut,
  };
  for (auto& it : functions) {
    EXPECT_NEAR(0.f, it(0.f), 1.0e-4);
    EXPECT_NEAR(1.f, it(1.f), 1.0e-4);
  }
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
