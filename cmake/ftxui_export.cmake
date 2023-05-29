add_library(ftxui::screen ALIAS screen)
add_library(ftxui::dom ALIAS dom)
add_library(ftxui::component ALIAS component)
export(
  TARGETS screen dom component
  NAMESPACE ftxui::
  FILE "${PROJECT_BINARY_DIR}/ftxui-targets.cmake"
)
