// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_UTIL_WARN_WINDOWS_MACRO_H_
#define FTXUI_UTIL_WARN_WINDOWS_MACRO_H_

#ifdef min
#error \
    "The macro 'min' is defined, which conflicts with the standard C++ library and FTXUI. This is often caused by including <windows.h>. To fix this, add '#define NOMINMAX' before including <windows.h>, or pass '/DNOMINMAX' as a compiler flag."
#endif

#ifdef max
#error \
    "The macro 'max' is defined, which conflicts with the standard C++ library and FTXUI. This is often caused by including <windows.h>. To fix this, add '#define NOMINMAX' before including <windows.h>, or pass '/DNOMINMAX' as a compiler flag."
#endif

#endif  // FTXUI_UTIL_WARN_WINDOWS_MACRO_H_
