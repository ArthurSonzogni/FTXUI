// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_ANIMATION_HPP
#define FTXUI_ANIMATION_HPP

#include <chrono>      // for milliseconds, duration, steady_clock, time_point
#include <functional>  // for function
#include "ftxui/util/export.hpp"

namespace ftxui::animation {
/// @brief RequestAnimationFrame is a function that requests a new frame to be
/// drawn in the next animation cycle.
///
/// @note This function is typically called by components that need to
/// update their state or appearance over time, such as animations or
/// transitions. This is useful when the change doesn't depend depend on the
/// events seen by the terminal, but rather on the passage of time.
///
/// Components who haven't completed their animation can call this function to
/// request a new frame to be drawn later.
///
/// When there is no new events and no animations to complete, no new frame is
/// drawn.
///
/// @ingroup component
FTXUI_EXPORT(COMPONENT) void RequestAnimationFrame();

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<float>;

/// Parameters passed to ComponentBase::OnAnimation(Params&).
class FTXUI_EXPORT(COMPONENT) Params {
 public:
  explicit Params(Duration duration) : duration_(duration) {}

  /// The duration this animation step represents.
  Duration duration() const { return duration_; }

 private:
  Duration duration_;
};

namespace easing {
using Function = std::function<float(float)>;
/// Linear interpolation (no easing).
FTXUI_EXPORT(COMPONENT) float Linear(float p);

/// Quadratic ease-in (`p^2`).
FTXUI_EXPORT(COMPONENT) float QuadraticIn(float p);
/// Quadratic ease-out.
FTXUI_EXPORT(COMPONENT) float QuadraticOut(float p);
/// Quadratic ease-in-out.
FTXUI_EXPORT(COMPONENT) float QuadraticInOut(float p);

/// Cubic ease-in (`p^3`).
FTXUI_EXPORT(COMPONENT) float CubicIn(float p);
/// Cubic ease-out.
FTXUI_EXPORT(COMPONENT) float CubicOut(float p);
/// Cubic ease-in-out.
FTXUI_EXPORT(COMPONENT) float CubicInOut(float p);

/// Quartic ease-in (`p^4`).
FTXUI_EXPORT(COMPONENT) float QuarticIn(float p);
/// Quartic ease-out.
FTXUI_EXPORT(COMPONENT) float QuarticOut(float p);
/// Quartic ease-in-out.
FTXUI_EXPORT(COMPONENT) float QuarticInOut(float p);

/// Quintic ease-in (`p^5`).
FTXUI_EXPORT(COMPONENT) float QuinticIn(float p);
/// Quintic ease-out.
FTXUI_EXPORT(COMPONENT) float QuinticOut(float p);
/// Quintic ease-in-out.
FTXUI_EXPORT(COMPONENT) float QuinticInOut(float p);

/// Sine wave ease-in (`sin(p * pi/2)`).
FTXUI_EXPORT(COMPONENT) float SineIn(float p);
/// Sine wave ease-out.
FTXUI_EXPORT(COMPONENT) float SineOut(float p);
/// Sine wave ease-in-out.
FTXUI_EXPORT(COMPONENT) float SineInOut(float p);

/// Circular ease-in (`sqrt(1 - p^2)`).
FTXUI_EXPORT(COMPONENT) float CircularIn(float p);
/// Circular ease-out.
FTXUI_EXPORT(COMPONENT) float CircularOut(float p);
/// Circular ease-in-out.
FTXUI_EXPORT(COMPONENT) float CircularInOut(float p);

/// Exponential ease-in (base 2).
FTXUI_EXPORT(COMPONENT) float ExponentialIn(float p);
/// Exponential ease-out (base 2).
FTXUI_EXPORT(COMPONENT) float ExponentialOut(float p);
/// Exponential ease-in-out (base 2).
FTXUI_EXPORT(COMPONENT) float ExponentialInOut(float p);

/// Exponentially damped sine-wave ease-in.
FTXUI_EXPORT(COMPONENT) float ElasticIn(float p);
/// Exponentially damped sine-wave ease-out.
FTXUI_EXPORT(COMPONENT) float ElasticOut(float p);
/// Exponentially damped sine-wave ease-in-out.
FTXUI_EXPORT(COMPONENT) float ElasticInOut(float p);

/// Overshooting cubic ease-in.
FTXUI_EXPORT(COMPONENT) float BackIn(float p);
/// Overshooting cubic ease-out.
FTXUI_EXPORT(COMPONENT) float BackOut(float p);
/// Overshooting cubic ease-in-out.
FTXUI_EXPORT(COMPONENT) float BackInOut(float p);

/// Exponentially decaying bounce ease-in.
FTXUI_EXPORT(COMPONENT) float BounceIn(float p);
/// Exponentially decaying bounce ease-out.
FTXUI_EXPORT(COMPONENT) float BounceOut(float p);
/// Exponentially decaying bounce ease-in-out.
FTXUI_EXPORT(COMPONENT) float BounceInOut(float p);
}  // namespace easing

/// Animates a float value toward a target using an easing function.
class FTXUI_EXPORT(COMPONENT) Animator {
 public:
  /// Animate `*from` toward `to` over `duration` after an optional `delay`.
  explicit Animator(float* from,
                    float to = 0.f,
                    Duration duration = std::chrono::milliseconds(250),
                    easing::Function easing_function = easing::Linear,
                    Duration delay = std::chrono::milliseconds(0));

  /// Advance the animation by `params.duration()` and update the target float.
  void OnAnimation(Params&);

  /// Return the target value of this animation.
  float to() const { return to_; }

 private:
  float* value_;
  float from_;
  float to_;
  Duration duration_;
  easing::Function easing_function_;
  Duration current_;
};

}  // namespace ftxui::animation

#endif /* end of include guard: FTXUI_ANIMATION_HPP */
