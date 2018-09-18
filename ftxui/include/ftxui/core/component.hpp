#ifndef FTXUI_STATE_HPP
#define FTXUI_STATE_HPP

#include "ftxui/core/requirement.hpp"
#include "ftxui/core/document.hpp"

namespace ftxui {

class Component {
 public:
  virtual Document Render() = 0;

  // Requirement -------------------------------------------------------------
  virtual void ComputeRequirement();
  Requirement requirement() { return requirement_; }

 private:
  Requirement requirement_;
};

};  // namespace ftxui

#endif /* end of include guard: FTXUI_STATE_HPP */
