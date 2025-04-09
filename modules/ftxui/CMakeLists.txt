# CMake 3.28+ supports the new CMake C++ module system.
if(CMAKE_VERSION VERSION_LESS 3.28)
  return()
endif()

# Not all compilers support the C++ module system yet.
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|MSVC")
  return()
endif()

# The list of generators which support scanning sources for C++ modules include:
# - Ninja
# - Ninja Multi-Config
# - Visual Studio 17 2022
if (NOT (CMAKE_GENERATOR MATCHES "Ninja") AND
    NOT (CMAKE_GENERATOR MATCHES "Visual Studio" AND
      CMAKE_GENERATOR_VERSION VERSION_GREATER_EQUAL 17))
  return()
endif()

cmake_minimum_required(VERSION 3.28)
add_library(ftxui_modules)

target_sources(ftxui_modules
  PUBLIC
  FILE_SET CXX_MODULES
  FILES
  component.cppm
  component/Animation.cppm
  component/CapturedMouse.cppm
  component/Component.cppm
  component/ComponentBase.cppm
  component/ComponentOptions.cppm
  component/Event.cppm
  component/Loop.cppm
  component/Mouse.cppm
  component/Receiver.cppm
  component/ScreenInteractive.cppm
  component/Task.cppm
  dom.cppm
  dom/Canvas.cppm
  dom/Deprecated.cppm
  dom/Direction.cppm
  dom/Elements.cppm
  dom/FlexboxConfig.cppm
  dom/LinearGradient.cppm
  dom/Node.cppm
  dom/Requirement.cppm
  dom/Selection.cppm
  dom/Table.cppm
  screen.cppm
  screen/Box.cppm
  screen/Color.cppm
  screen/ColorInfo.cppm
  screen/Deprecated.cppm
  screen/Image.cppm
  screen/Pixel.cppm
  screen/Screen.cppm
  screen/String.cppm
  screen/Terminal.cppm
  util.cppm
  util/AutoReset.cppm
  util/Ref.cppm
  )

target_link_libraries(ftxui_modules
  PUBLIC
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_compile_features(ftxui_modules PUBLIC cxx_std_20)

#set_target_properties(ftxui_modules PROPERTIES 
  #CXX_STANDARD 23 
  #CXX_STANDARD_REQUIRED ON
  #CXX_EXTENSIONS OFF 
  #CXX_MODULE_DYNDEP OFF
#)

add_library(ftxui::modules ALIAS ftxui_modules)

if(FTXUI_ENABLE_INSTALL)
  install(TARGETS ftxui_modules
    EXPORT ftxui-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui/modules
  )
endif()
