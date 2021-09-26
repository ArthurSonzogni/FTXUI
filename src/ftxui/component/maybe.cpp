#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"

namespace ftxui {

Component Maybe(Component child, bool* show) {
  class Impl : public ComponentBase {
   public:
    Impl(bool* show): show_(show) {}

   private:
    Element Render() override {
      return *show_ ? ComponentBase::Render() : std::make_unique<Node>();
    }
    bool Focusable() const override {
      return *show_ && ComponentBase::Focusable();
    }
    bool OnEvent(Event event) override {
      return *show_ && ComponentBase::OnEvent(event);
    }

    bool* show_;
  };
  
  auto maybe = Make<Impl>(show);
  maybe->Add(std::move(child));
  return maybe;
}

}  // namespace ftxui
