if (NOT FTXUI_BUILD_MODULES)
  return()
endif()

# CMake 3.28+ supports the new CMake C++ module system.
if(CMAKE_VERSION VERSION_LESS 3.28)
  message(FATAL_ERROR
    "FTXUI_BUILD_MODULES requires CMake 3.28 or higher. "
  )
  return()
endif()

# Not all compilers support the C++ module system yet.
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|MSVC")
  message(FATAL_ERROR
    "FTXUI_BUILD_MODULES requires a compiler that supports C++20 modules. "
    "Currently, only Clang, GCC, and MSVC support this feature."
  )
  return()
endif()

# The list of generators which support scanning sources for C++ modules include:
# - Ninja
# - Ninja Multi-Config
# - Visual Studio 17 2022
if (NOT (CMAKE_GENERATOR MATCHES "Ninja") AND
    NOT (CMAKE_GENERATOR MATCHES "Visual Studio" AND
      CMAKE_GENERATOR_VERSION VERSION_GREATER_EQUAL 17))
  message(FATAL_ERROR
    "FTXUI_BUILD_MODULES requires a generator that supports C++20 modules.
    Please use Ninja or Visual Studio 17 2022 or higher."
  )
  return()
endif()

cmake_minimum_required(VERSION 3.28)
add_library(ftxui-modules)

target_sources(ftxui-modules
  PUBLIC
  FILE_SET CXX_MODULES
  FILES
  src/ftxui/component.cppm
  src/ftxui/component/Animation.cppm
  src/ftxui/component/CapturedMouse.cppm
  src/ftxui/component/Component.cppm
  src/ftxui/component/ComponentBase.cppm
  src/ftxui/component/ComponentOptions.cppm
  src/ftxui/component/Event.cppm
  src/ftxui/component/Loop.cppm
  src/ftxui/component/Mouse.cppm
  src/ftxui/component/Receiver.cppm
  src/ftxui/component/ScreenInteractive.cppm
  src/ftxui/component/Task.cppm
  src/ftxui/dom.cppm
  src/ftxui/dom/Canvas.cppm
  src/ftxui/dom/Deprecated.cppm
  src/ftxui/dom/Direction.cppm
  src/ftxui/dom/Elements.cppm
  src/ftxui/dom/FlexboxConfig.cppm
  src/ftxui/dom/LinearGradient.cppm
  src/ftxui/dom/Node.cppm
  src/ftxui/dom/Requirement.cppm
  src/ftxui/dom/Selection.cppm
  src/ftxui/dom/Table.cppm
  src/ftxui/screen.cppm
  src/ftxui/screen/Box.cppm
  src/ftxui/screen/Color.cppm
  src/ftxui/screen/ColorInfo.cppm
  src/ftxui/screen/Deprecated.cppm
  src/ftxui/screen/Image.cppm
  src/ftxui/screen/Pixel.cppm
  src/ftxui/screen/Screen.cppm
  src/ftxui/screen/String.cppm
  src/ftxui/screen/Terminal.cppm
  src/ftxui/util.cppm
  src/ftxui/util/AutoReset.cppm
  src/ftxui/util/Ref.cppm
  )

target_link_libraries(ftxui-modules
  PUBLIC
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_compile_features(ftxui-modules PUBLIC cxx_std_20)

#set_target_properties(ftxui-modules PROPERTIES 
  #CXX_STANDARD 23 
  #CXX_STANDARD_REQUIRED ON
  #CXX_EXTENSIONS OFF 
  #CXX_MODULE_DYNDEP OFF
#)

add_library(ftxui::modules ALIAS ftxui-modules)

if(FTXUI_ENABLE_INSTALL)
  install(TARGETS ftxui-modules
    EXPORT ftxui-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui/modules
  )
endif()
