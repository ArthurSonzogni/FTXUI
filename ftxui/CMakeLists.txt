cmake_minimum_required(VERSION 3.0)

####################
# ftxui::screen
####################
add_library(screen
  src/ftxui/screen/screen.cpp
  src/ftxui/terminal.cpp
  src/ftxui/util/string.cpp
)

target_include_directories(screen
  PUBLIC
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE src
)

target_compile_features(screen PUBLIC cxx_std_17)
target_compile_options(screen PRIVATE -Wall)

####################
# ftxui::dom
####################
add_library(dom
  src/ftxui/dom/blink.cpp
  src/ftxui/dom/bold.cpp
  src/ftxui/dom/color.cpp
  src/ftxui/dom/composite_decorator.cpp
  src/ftxui/dom/dbox.cpp
  src/ftxui/dom/dim.cpp
  src/ftxui/dom/flex.cpp
  src/ftxui/dom/frame.cpp
  src/ftxui/dom/gauge.cpp
  src/ftxui/dom/hbox.cpp
  src/ftxui/dom/inverted.cpp
  src/ftxui/dom/node.cpp
  src/ftxui/dom/node_decorator.cpp
  src/ftxui/dom/separator.cpp
  src/ftxui/dom/size.cpp
  src/ftxui/dom/text.cpp
  src/ftxui/dom/underlined.cpp
  src/ftxui/dom/util.cpp
  src/ftxui/dom/vbox.cpp
)

target_include_directories(dom
  PUBLIC
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE src
)

target_link_libraries(dom
  PUBLIC screen
)

target_compile_features(dom PUBLIC cxx_std_17)
target_compile_options(dom PRIVATE -Wall)

####################
# ftxui::component
####################
add_library(component
  src/ftxui/component/component.cpp
  src/ftxui/component/component_direction.cpp
  src/ftxui/component/component_horizontal.cpp
  src/ftxui/component/component_vertical.cpp
  src/ftxui/component/event.cpp
  src/ftxui/component/input.cpp
  src/ftxui/component/menu.cpp
  src/ftxui/component/screen_interactive.cpp
  src/ftxui/component/toggle.cpp
)

target_include_directories(component
  PUBLIC
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE src
)

target_link_libraries(component
  PUBLIC dom
)

target_compile_features(component PUBLIC cxx_std_17)
target_compile_options(component PRIVATE -Wall)

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
  FILE ftxui-targets.cmake
	NAMESPACE ftxui::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
)

# Note: For gtest, please follow:
# https://stackoverflow.com/questions/24295876/cmake-cannot-find-a-googletest-required-library
find_package(GTest)
find_package(Threads)
if (GTEST_FOUND AND THREADS_FOUND)
  add_executable(dom_tests
    src/ftxui/dom/gauge_test.cpp
    src/ftxui/dom/hbox_test.cpp
    src/ftxui/dom/text_test.cpp
    src/ftxui/dom/vbox_test.cpp
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
