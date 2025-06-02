if (NOT FTXUI_BUILD_MODULES)
  return()
endif()

add_library(ftxui-modules)

target_sources(ftxui-modules
  PUBLIC FILE_SET CXX_MODULES FILES
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
