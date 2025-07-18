if (NOT FTXUI_BUILD_MODULES)
  return()
endif()

add_library(ftxui-modules)

target_sources(ftxui-modules
  PUBLIC FILE_SET CXX_MODULES FILES
  src/ftxui/ftxui.cppm
  src/ftxui/component.cppm
  src/ftxui/component/animation.cppm
  src/ftxui/component/captured_mouse.cppm
  src/ftxui/component/component.cppm
  src/ftxui/component/component_base.cppm
  src/ftxui/component/component_options.cppm
  src/ftxui/component/event.cppm
  src/ftxui/component/loop.cppm
  src/ftxui/component/mouse.cppm
  src/ftxui/component/receiver.cppm
  src/ftxui/component/screen_interactive.cppm
  src/ftxui/component/task.cppm
  src/ftxui/dom.cppm
  src/ftxui/dom/canvas.cppm
  src/ftxui/dom/deprecated.cppm
  src/ftxui/dom/direction.cppm
  src/ftxui/dom/elements.cppm
  src/ftxui/dom/flexbox_config.cppm
  src/ftxui/dom/linear_gradient.cppm
  src/ftxui/dom/node.cppm
  src/ftxui/dom/requirement.cppm
  src/ftxui/dom/selection.cppm
  src/ftxui/dom/table.cppm
  src/ftxui/screen.cppm
  src/ftxui/screen/box.cppm
  src/ftxui/screen/color.cppm
  src/ftxui/screen/color_info.cppm
  src/ftxui/screen/deprecated.cppm
  src/ftxui/screen/image.cppm
  src/ftxui/screen/pixel.cppm
  src/ftxui/screen/screen.cppm
  src/ftxui/screen/string.cppm
  src/ftxui/screen/terminal.cppm
  src/ftxui/util.cppm
  src/ftxui/util/autoreset.cppm
  src/ftxui/util/ref.cppm
  )

target_link_libraries(ftxui-modules
  PUBLIC
  ftxui::screen
  ftxui::dom
  ftxui::component
  )

target_compile_features(ftxui-modules PUBLIC cxx_std_20)
# TODO: Explain why this is needed.
if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(ftxui-modules PUBLIC -fmodules-ts)
endif ()

add_library(ftxui::modules ALIAS ftxui-modules)

if(FTXUI_ENABLE_INSTALL)

  include(GNUInstallDirs)

  install(TARGETS ftxui-modules
    EXPORT ftxui-targets
    FILE_SET CXX_MODULES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    FILE_SET HEADERS
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    )
  install(EXPORT ftxui-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
    CXX_MODULES_DIRECTORY ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
    )
  install(FILES my_package-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ftxui
    )
endif()
