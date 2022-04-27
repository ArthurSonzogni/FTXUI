#include <cmath>
#include <ratio>    // for ratio
#include <utility>  // for move

#include "ftxui/component/animation.hpp"

namespace ftxui::animation {

namespace easing {

namespace {
constexpr float kPi = 3.14159265358979323846F;
constexpr float kPi2 = kPi / 2.F;
}  // namespace

// Easing function have been taken out of:
// https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c
//
// Corresponding license:
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.

// Modeled after the line y = x
float Linear(float p) {
  return p;
}

// Modeled after the parabola y = x^2
float QuadraticIn(float p) {
  return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
float QuadraticOut(float p) {
  return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
float QuadraticInOut(float p) {
  if (p < 0.5F) {  // NOLINT
    return 2 * p * p;
  } else {
    return (-2 * p * p) + (4 * p) - 1;
  }
}

// Modeled after the cubic y = x^3
float CubicIn(float p) {
  return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
float CubicOut(float p) {
  float f = (p - 1);
  return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
float CubicInOut(float p) {
  if (p < 0.5F) {  // NOLINT
    return 4 * p * p * p;
  } else {
    float f = ((2 * p) - 2);
    return 0.5F * f * f * f + 1;  // NOLINT
  }
}

// Modeled after the quartic x^4
float QuarticIn(float p) {
  return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
float QuarticOut(float p) {
  float f = (p - 1);
  return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
float QuarticInOut(float p) {
  if (p < 0.5F) {              // NOLINT
    return 8 * p * p * p * p;  // NOLINT
  } else {
    float f = (p - 1);
    return -8 * f * f * f * f + 1;  // NOLINT
  }
}

// Modeled after the quintic y = x^5
float QuinticIn(float p) {
  return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
float QuinticOut(float p) {
  float f = (p - 1);
  return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
float QuinticInOut(float p) {
  if (p < 0.5F) {                        // NOLINT
    return 16 * p * p * p * p * p;       // NOLINT
  } else {                               // NOLINT
    float f = ((2 * p) - 2);             // NOLINT
    return 0.5 * f * f * f * f * f + 1;  // NOLINT
  }
}

// Modeled after quarter-cycle of sine wave
float SineIn(float p) {
  return std::sin((p - 1) * kPi2) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
float SineOut(float p) {
  return std::sin(p * kPi2);
}

// Modeled after half sine wave
float SineInOut(float p) {
  return 0.5F * (1 - std::cos(p * kPi));  // NOLINT
}

// Modeled after shifted quadrant IV of unit circle
float CircularIn(float p) {
  return 1 - std::sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
float CircularOut(float p) {
  return std::sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
float CircularInOut(float p) {
  if (p < 0.5F) {                                    // NOLINT
    return 0.5F * (1 - std::sqrt(1 - 4 * (p * p)));  // NOLINT
  } else {
    return 0.5F * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);  // NOLINT
  }
}

// Modeled after the exponential function y = 2^(10(x - 1))
float ExponentialIn(float p) {
  return (p == 0.0) ? p : std::pow(2, 10 * (p - 1));  // NOLINT
}

// Modeled after the exponential function y = -2^(-10x) + 1
float ExponentialOut(float p) {
  return (p == 1.0) ? p : 1 - std::pow(2, -10 * p);  // NOLINT
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
float ExponentialInOut(float p) {
  if (p == 0.0 || p == 1.F) {
    return p;
  }

  if (p < 0.5F) {                                   // NOLINT
    return 0.5 * std::pow(2, (20 * p) - 10);        // NOLINT
  } else {                                          // NOLINT
    return -0.5 * std::pow(2, (-20 * p) + 10) + 1;  // NOLINT
  }
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
float ElasticIn(float p) {
  return std::sin(13.F * kPi2 * p) * std::pow(2.F, 10.F * (p - 1));  // NOLINT
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) +
// 1
float ElasticOut(float p) {
  // NOLINTNEXTLINE
  return std::sin(-13.F * kPi2 * (p + 1)) * std::pow(2.F, -10.F * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
float ElasticInOut(float p) {
  if (p < 0.5F) {                                               // NOLINT
    return 0.5 * std::sin(13.F * kPi2 * (2 * p)) *              // NOLINT
           std::pow(2, 10 * ((2 * p) - 1));                     // NOLINT
  } else {                                                      // NOLINT
    return 0.5 * (std::sin(-13.F * kPi2 * ((2 * p - 1) + 1)) *  // NOLINT
                      std::pow(2, -10 * (2 * p - 1)) +          // NOLINT
                  2);                                           // NOLINT
  }
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
float BackIn(float p) {
  return p * p * p - p * std::sin(p * kPi);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
float BackOut(float p) {
  float f = (1 - p);
  return 1 - (f * f * f - f * std::sin(f * kPi));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
float BackInOut(float p) {
  if (p < 0.5F) {  // NOLINT
    float f = 2 * p;
    return 0.5F * (f * f * f - f * std::sin(f * kPi));  // NOLINT
  } else {
    float f = (1 - (2 * p - 1));                                    // NOLINT
    return 0.5F * (1 - (f * f * f - f * std::sin(f * kPi))) + 0.5;  // NOLINT
  }
}

float BounceIn(float p) {
  return 1 - BounceOut(1 - p);
}

float BounceOut(float p) {
  if (p < 4 / 11.0) {                                           // NOLINT
    return (121 * p * p) / 16.0;                                // NOLINT
  } else if (p < 8 / 11.0) {                                    // NOLINT
    return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;   // NOLINT
  } else if (p < 9 / 10.0) {                                    // NOLINT
    return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) +      // NOLINT
           16061 / 1805.0;                                      // NOLINT
  } else {                                                      // NOLINT
    return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;  // NOLINT
  }
}

float BounceInOut(float p) {                    // NOLINT
  if (p < 0.5F) {                               // NOLINT
    return 0.5F * BounceIn(p * 2);              // NOLINT
  } else {                                      // NOLINT
    return 0.5F * BounceOut(p * 2 - 1) + 0.5F;  // NOLINT
  }
}

}  // namespace easing

Animator::Animator(float* from,
                   float to,
                   Duration duration,
                   easing::Function easing_function,
                   Duration delay)
    : value_(from),
      from_(*from),
      to_(to),
      duration_(duration),
      easing_function_(std::move(easing_function)),
      current_(-delay) {
  RequestAnimationFrame();
}

void Animator::OnAnimation(Params& params) {
  current_ += params.duration();

  if (current_ >= duration_) {
    *value_ = to_;
    return;
  }

  if (current_ <= Duration()) {
    *value_ = from_;
  } else {
    *value_ = from_ +
              (to_ - from_) * easing_function_(current_ / duration_);  // NOLINT
  }

  RequestAnimationFrame();
}

}  // namespace ftxui::animation
