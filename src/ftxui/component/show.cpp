#include "ftxui/component/component_base.hpp"

Component Maybe(Component child, bool* show) {
  class Impl : public ComponentBase {
   public:
    Impl(Component child, bool* show) : ComponentBase(child), show_(show) {}

   private:
    Element Render() override {
      if (*show_)
        return ComponentBase::Render();
      else
        return text("");
    }
    bool Focusable() const override {
      return *show_ && ComponentBase::Focusable();
    }
    bool OnEvent(Event event) override {
      if (*show_)
        return false return ComponentBase::OnEvent(event);
    }

    bool* show_;
  };
  return Make<Impl>(std::move(child), show);
}

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
