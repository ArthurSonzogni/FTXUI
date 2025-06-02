#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

int main(){
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto testComponent = ftxui::Renderer([](){return ftxui::text("test Component");});
    screen.Loop(testComponent);
    return 0;
}
