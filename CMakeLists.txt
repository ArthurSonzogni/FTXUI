cmake_minimum_required(VERSION 3.0)
project(ftxui
  LANGUAGES CXX
  VERSION 0.1
)

option(FTXUI_BUILD_EXAMPLES "Set to ON to build examples" ON)
option(FTXUI_BUILD_TESTS "Set to ON to build tests" ON)
option(FTXUI_ENABLE_INSTALL "Generate the install target" ON)

enable_testing()

find_package(Threads)

add_library(screen
  src/ftxui/screen/box.cpp
  src/ftxui/screen/screen.cpp
  src/ftxui/screen/string.cpp
  src/ftxui/screen/terminal.cpp
  src/ftxui/screen/wcwidth.cpp
  include/ftxui/screen/box.hpp
  include/ftxui/screen/color.hpp
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
  src/ftxui/component/checkbox.cpp
  src/ftxui/component/component.cpp
  src/ftxui/component/container.cpp
  src/ftxui/component/event.cpp
  src/ftxui/component/input.cpp
  src/ftxui/component/menu.cpp
  src/ftxui/component/radiobox.cpp
  src/ftxui/component/screen_interactive.cpp
  src/ftxui/component/toggle.cpp
  include/ftxui/component/checkbox.hpp
  include/ftxui/component/component.hpp
  include/ftxui/component/container.hpp
  include/ftxui/component/event.hpp
  include/ftxui/component/input.hpp
  include/ftxui/component/menu.hpp
  include/ftxui/component/radiobox.hpp
  include/ftxui/component/screen_interactive.hpp
  include/ftxui/component/producer_consumer.hpp
  include/ftxui/component/toggle.hpp
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
  if (MSVC)
    target_compile_options(${lib} PRIVATE "/W4")
    target_compile_options(${lib} PRIVATE "/WX")
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
  write_basic_package_version_file(
    ftxui-config.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
  )

  install(EXPORT ftxui-export
    FILE ftxui-config.cmake
	  NAMESPACE ftxui::
	  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
  )
endif()

export(TARGETS screen dom component NAMESPACE ftxui::
  FILE ${PROJECT_BINARY_DIR}/ftxui-targets.cmake)

# Note: For gtest, please follow:
# https://stackoverflow.com/questions/24295876/cmake-cannot-find-a-googletest-required-library
find_package(GTest)
if (FTXUI_BUILD_TESTS AND GTEST_FOUND AND THREADS_FOUND)
  add_executable(dom_tests
    tests/gauge_test.cpp
    tests/hbox_test.cpp
    tests/text_test.cpp
    tests/vbox_test.cpp
  )

  target_link_libraries(dom_tests
    PRIVATE dom
    PRIVATE Threads::Threads
    PRIVATE ${GTEST_BOTH_LIBRARIES}
  )

  target_include_directories(dom_tests
    PRIVATE ${GTest_INCLUDE_DIRS}
  )
  gtest_discover_tests(dom_tests)
  add_test(dom_tests dom_tests)
endif()

if(FTXUI_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
