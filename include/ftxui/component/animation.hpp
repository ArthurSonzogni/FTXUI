#ifndef FTXUI_ANIMATION_HPP
#define FTXUI_ANIMATION_HPP

#include <chrono>      // for milliseconds, duration, steady_clock, time_point
#include <functional>  // for function

#include "ftxui/component/event.hpp"

namespace ftxui {

namespace animation {
// Components who haven't completed their animation can call this function to
// request a new frame to be drawn later.
//
// When there is no new events and no animations to complete, no new frame is
// drawn.
void RequestAnimationFrame();

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;

// Parameter of Component::OnAnimation(param).
class Params {
 public:
  Params(Duration duration) : duration_(duration) {}

  /// The duration this animation step represents.
  Duration duration() const { return duration_; }

 private:
  Duration duration_;
};

namespace easing {
using Function = std::function<float(float)>;
// Linear interpolation (no easing)
float Linear(float p);

// Quadratic easing; p^2
float QuadraticIn(float p);
float QuadraticOut(float p);
float QuadraticInOut(float p);

// Cubic easing; p^3
float CubicIn(float p);
float CubicOut(float p);
float CubicInOut(float p);

// Quartic easing; p^4
float QuarticIn(float p);
float QuarticOut(float p);
float QuarticInOut(float p);

// Quintic easing; p^5
float QuinticIn(float p);
float QuinticOut(float p);
float QuinticInOut(float p);

// Sine wave easing; sin(p * PI/2)
float SineIn(float p);
float SineOut(float p);
float SineInOut(float p);

// Circular easing; sqrt(1 - p^2)
float CircularIn(float p);
float CircularOut(float p);
float CircularInOut(float p);

// Exponential easing, base 2
float ExponentialIn(float p);
float ExponentialOut(float p);
float ExponentialInOut(float p);

// Exponentially-damped sine wave easing
float ElasticIn(float p);
float ElasticOut(float p);
float ElasticInOut(float p);

// Overshooting cubic easing;
float BackIn(float p);
float BackOut(float p);
float BackInOut(float p);

// Exponentially-decaying bounce easing
float BounceIn(float p);
float BounceOut(float p);
float BounceInOut(float p);
}  // namespace easing

class Animator {
 public:
  Animator(float* from,
           float to = 0.f,
           Duration duration = std::chrono::milliseconds(250),
           easing::Function easing_function = easing::Linear,
           Duration delay = std::chrono::milliseconds(0));

  void OnAnimation(Params&);

  float to() const { return to_; }

 private:
  float* value_;
  float from_;
  float to_;
  Duration duration_;
  easing::Function easing_function_;
  Duration current_;
};

}  // namespace animation
}  // namespace ftxui

#endif /* end of include guard: FTXUI_ANIMATION_HPP */

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
