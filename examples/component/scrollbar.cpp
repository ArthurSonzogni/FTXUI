// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

Component DummyWindowContent() {
  class Impl : public ComponentBase {
   private:
    float scroll_x = 0.1;
    float scroll_y = 0.1;

   public:
    Impl() {
      auto content = Renderer([=] {
        const std::string lorem =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
            "do eiusmod tempor incididunt ut labore et dolore magna "
            "aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
            "ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis "
            "aute irure dolor in reprehenderit in voluptate velit esse "
            "cillum dolore eu fugiat nulla pariatur. Excepteur sint "
            "occaecat cupidatat non proident, sunt in culpa qui officia "
            "deserunt mollit anim id est laborum.";
        return vbox({
            text(lorem.substr(0, -1)),   text(lorem.substr(5, -1)),   text(""),
            text(lorem.substr(10, -1)),  text(lorem.substr(15, -1)),  text(""),
            text(lorem.substr(20, -1)),  text(lorem.substr(25, -1)),  text(""),
            text(lorem.substr(30, -1)),  text(lorem.substr(35, -1)),  text(""),
            text(lorem.substr(40, -1)),  text(lorem.substr(45, -1)),  text(""),
            text(lorem.substr(50, -1)),  text(lorem.substr(55, -1)),  text(""),
            text(lorem.substr(60, -1)),  text(lorem.substr(65, -1)),  text(""),
            text(lorem.substr(70, -1)),  text(lorem.substr(75, -1)),  text(""),
            text(lorem.substr(80, -1)),  text(lorem.substr(85, -1)),  text(""),
            text(lorem.substr(90, -1)),  text(lorem.substr(95, -1)),  text(""),
            text(lorem.substr(100, -1)), text(lorem.substr(105, -1)), text(""),
            text(lorem.substr(110, -1)), text(lorem.substr(115, -1)), text(""),
            text(lorem.substr(120, -1)), text(lorem.substr(125, -1)), text(""),
            text(lorem.substr(130, -1)), text(lorem.substr(135, -1)), text(""),
            text(lorem.substr(140, -1)),
        });
      });

      auto scrollable_content = Renderer(content, [&, content] {
        return content->Render() | focusPositionRelative(scroll_x, scroll_y) |
               frame | flex;
      });

      SliderOption<float> option_x;
      option_x.value = &scroll_x;
      option_x.min = 0.f;
      option_x.max = 1.f;
      option_x.increment = 0.1f;
      option_x.direction = Direction::Right;
      option_x.color_active = Color::Blue;
      option_x.color_inactive = Color::BlueLight;
      auto scrollbar_x = Slider(option_x);

      SliderOption<float> option_y;
      option_y.value = &scroll_y;
      option_y.min = 0.f;
      option_y.max = 1.f;
      option_y.increment = 0.1f;
      option_y.direction = Direction::Down;
      option_y.color_active = Color::Yellow;
      option_y.color_inactive = Color::YellowLight;
      auto scrollbar_y = Slider(option_y);

      Add(Container::Vertical({
          Container::Horizontal({
              scrollable_content,
              scrollbar_y,
          }) | flex,
          Container::Horizontal({
              scrollbar_x,
              Renderer([] { return text(L"x"); }),
          }),
      }));
    }
  };
  return Make<Impl>();
}

int main() {
  auto window_1 = Window({
      .inner = DummyWindowContent(),
      .title = "First window",
      .width = 80,
      .height = 30,
  });

  auto window_2 = Window({
      .inner = DummyWindowContent(),
      .title = "My window",
      .left = 40,
      .top = 20,
      .width = 80,
      .height = 30,
  });

  auto window_container = Container::Stacked({
      window_1,
      window_2,
  });

  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(window_container);

  return EXIT_SUCCESS;
}
