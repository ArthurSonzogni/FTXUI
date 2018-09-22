#include "ftxui/core/dom/node.hpp"
#include "ftxui/core/dom/elements.hpp"

namespace ftxui {
namespace dom {

static wchar_t charset[] = L"┌┐└┘─│";

class Frame : public Node {
 public:
  Frame(Child child) : Node(unpack(std::move(child))) {}
  ~Frame() override {}

  void ComputeRequirement() override {
    children[0]->ComputeRequirement();
    requirement_ = children[0]->requirement();
    requirement_.min.x += 2;
    requirement_.min.y += 2;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    box.left++;
    box.right--;
    box.top++;
    box.bottom--;
    children[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    if (box_.left >= box_.right || box_.top >= box_.bottom)
      return;

    screen.at(box_.left, box_.top) = charset[0];
    screen.at(box_.right, box_.top) = charset[1];
    screen.at(box_.left, box_.bottom) = charset[2];
    screen.at(box_.right, box_.bottom) = charset[3];
    for(float x = box_.left + 1; x<box_.right; ++x) {
      screen.at(x, box_.top) = charset[4];
      screen.at(x, box_.bottom) = charset[4];
    }
    for(float y = box_.top + 1; y<box_.bottom; ++y) {
      screen.at(box_.left, y) = charset[5];
      screen.at(box_.right,y) = charset[5];
    }
    children[0]->Render(screen);
  }
 private:
  float progress_;
};

std::unique_ptr<Node> frame(Child child) {
  return std::make_unique<Frame>(std::move(child));
}

};  // namespace dom
};  // namespace ftxui
