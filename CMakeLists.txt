cmake_minimum_required(VERSION 3.0)

find_package(Git QUIET)
if (Git_FOUND)
  message("git found")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE git_version
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(git_version 0)
endif()

project(ftxui
  LANGUAGES CXX
  VERSION 0.3.${git_version}
)

option(FTXUI_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(FTXUI_ENABLE_INSTALL "Generate the install target" ON)
option(FTXUI_BUILD_TESTS "Set to ON to build tests" OFF)
option(FTXUI_BUILD_DOCS "Set to ON to build tests" ON)

enable_testing()

find_package(Threads)

add_library(screen
  src/ftxui/screen/box.cpp
  src/ftxui/screen/color.cpp
  src/ftxui/screen/color_info.cpp
  src/ftxui/screen/screen.cpp
  src/ftxui/screen/string.cpp
  src/ftxui/screen/terminal.cpp
  src/ftxui/screen/wcwidth.cpp
  include/ftxui/screen/box.hpp
  include/ftxui/screen/color.hpp
  include/ftxui/screen/color_info.hpp
  include/ftxui/screen/screen.hpp
  include/ftxui/screen/string.hpp
)

add_library(dom
  include/ftxui/dom/elements.hpp
  include/ftxui/dom/node.hpp
  include/ftxui/dom/requirement.hpp
  include/ftxui/dom/take_any_args.hpp
  src/ftxui/dom/blink.cpp
  src/ftxui/dom/bold.cpp
  src/ftxui/dom/border.cpp
  src/ftxui/dom/clear_under.cpp
  src/ftxui/dom/color.cpp
  src/ftxui/dom/composite_decorator.cpp
  src/ftxui/dom/dbox.cpp
  src/ftxui/dom/dim.cpp
  src/ftxui/dom/flex.cpp
  src/ftxui/dom/frame.cpp
  src/ftxui/dom/gauge.cpp
  src/ftxui/dom/graph.cpp
  src/ftxui/dom/hbox.cpp
  src/ftxui/dom/hflow.cpp
  src/ftxui/dom/inverted.cpp
  src/ftxui/dom/node.cpp
  src/ftxui/dom/node_decorator.cpp
  src/ftxui/dom/paragraph.cpp
  src/ftxui/dom/separator.cpp
  src/ftxui/dom/size.cpp
  src/ftxui/dom/spinner.cpp
  src/ftxui/dom/text.cpp
  src/ftxui/dom/underlined.cpp
  src/ftxui/dom/util.cpp
  src/ftxui/dom/vbox.cpp
)

add_library(component
  include/ftxui/component/button.hpp
  include/ftxui/component/checkbox.hpp
  include/ftxui/component/component.hpp
  include/ftxui/component/container.hpp
  include/ftxui/component/event.hpp
  include/ftxui/component/input.hpp
  include/ftxui/component/menu.hpp
  include/ftxui/component/radiobox.hpp
  include/ftxui/component/receiver.hpp
  include/ftxui/component/screen_interactive.hpp
  include/ftxui/component/toggle.hpp
  src/ftxui/component/button.cpp
  src/ftxui/component/checkbox.cpp
  src/ftxui/component/component.cpp
  src/ftxui/component/container.cpp
  src/ftxui/component/event.cpp
  src/ftxui/component/input.cpp
  src/ftxui/component/menu.cpp
  src/ftxui/component/radiobox.cpp
  src/ftxui/component/radiobox.cpp
  src/ftxui/component/screen_interactive.cpp
  src/ftxui/component/toggle.cpp
  src/ftxui/component/terminal_input_parser.cpp
  src/ftxui/component/terminal_input_parser.hpp
)

add_library(ftxui::screen ALIAS screen)
add_library(ftxui::dom ALIAS dom)
add_library(ftxui::component ALIAS component)

target_link_libraries(dom PUBLIC screen)
target_link_libraries(component PUBLIC dom Threads::Threads)

foreach(lib screen dom component)
  target_include_directories(${lib}
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      src
  )

  # C++17 is used. We requires fold expression at least.
  set_property(TARGET ${lib} PROPERTY CXX_STANDARD 17)

  # Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
  # to the library and the library users.
  if (MSVC)
    target_compile_options(${lib} PUBLIC "/utf-8")
  endif()

  # Add as many warning as possible:
  if (WIN32)
    if (MSVC)
      target_compile_options(${lib} PRIVATE "/W4")
      target_compile_options(${lib} PRIVATE "/WX")
      target_compile_options(${lib} PRIVATE "/wd4244")
      target_compile_options(${lib} PRIVATE "/wd4267")
      target_compile_options(${lib} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
    endif()
    # Force Win32 to UNICODE
    target_compile_definitions(${lib} PRIVATE UNICODE _UNICODE)
  else()
    target_compile_options(${lib} PRIVATE "-Wall")
    target_compile_options(${lib} PRIVATE "-Wextra")
    target_compile_options(${lib} PRIVATE "-pedantic")
    target_compile_options(${lib} PRIVATE "-Werror")
    target_compile_options(${lib} PRIVATE "-Wno-sign-compare")
  endif()

endforeach()

if(FTXUI_ENABLE_INSTALL)
  include(GNUInstallDirs)
  install(TARGETS screen dom component
      EXPORT ftxui-export
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ftxui/
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/ftxui/
  )

  install(DIRECTORY include/ftxui DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  include(CMakePackageConfigHelpers)
  configure_package_config_file(ftxui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ftxui-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/ftxui/cmake
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  )
  write_basic_package_version_file(
    ftxui-config-version.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
  )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ftxui-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
  )
  install(EXPORT ftxui-export
    FILE ftxui-config-version.cmake
    NAMESPACE ftxui::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
  )
endif()

export(TARGETS screen dom component NAMESPACE ftxui::
  FILE ${PROJECT_BINARY_DIR}/ftxui-targets.cmake)


if (FTXUI_BUILD_TESTS AND ${CMAKE_VERSION} VERSION_GREATER "3.11.4")
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
  include(FetchContent)

  FetchContent_Declare( googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG        release-1.10.0
  )

  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    message(STATUS "Fetching googletest...")
    FetchContent_Populate(googletest)
    message(STATUS "... done")
    add_subdirectory(
      ${googletest_SOURCE_DIR}
      ${googletest_BINARY_DIR}
      EXCLUDE_FROM_ALL)
  endif()

  add_executable(tests
    src/ftxui/component/container_test.cpp
    src/ftxui/component/terminal_input_parser_test.cpp
    src/ftxui/component/radiobox_test.cpp
    src/ftxui/component/receiver_test.cpp
    src/ftxui/component/toggle_test.cpp
    src/ftxui/dom/gauge_test.cpp
    src/ftxui/dom/hbox_test.cpp
    src/ftxui/dom/text_test.cpp
    src/ftxui/dom/vbox_test.cpp
  )

  target_link_libraries(tests
    PRIVATE component
    PRIVATE gtest
    PRIVATE gmock
    PRIVATE gtest_main
  )
  target_include_directories(tests
    PRIVATE src
  )

  set_property(TARGET tests PROPERTY CXX_STANDARD 17)
endif()

if(FTXUI_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(FTXUI_BUILD_DOCS)
  add_subdirectory(doc)
endif()
