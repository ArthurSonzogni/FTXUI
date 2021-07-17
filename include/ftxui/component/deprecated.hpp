#ifndef FTXUI_COMPONENT_DEPRECATED_HPP
#define FTXUI_COMPONENT_DEPRECATED_HPP

#include "ftxui/component/component.hpp"

namespace ftxui {

Component Input(WideStringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option = {});
}  // namespace ftxui

#endif /* FTXUI_COMPONENT_DEPRECATED_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
