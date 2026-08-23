if (NOT FTXUI_BUILD_MODULES)
  return()
endif()

add_library(modules)

target_sources(modules
  PUBLIC FILE_SET CXX_MODULES FILES
  src/ftxui/ftxui.cppm
  src/ftxui/component.cppm
  src/ftxui/dom.cppm
  src/ftxui/screen.cppm
  src/ftxui/util.cppm
  )

target_link_libraries(modules
  PUBLIC
  ftxui::screen
  ftxui::dom
  ftxui::component
  )

target_compile_features(modules PUBLIC cxx_std_20)
# TODO: Explain why this is needed.
if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(modules PUBLIC -fmodules-ts)
endif ()

add_library(ftxui::modules ALIAS modules)

if(FTXUI_ENABLE_INSTALL)

  include(GNUInstallDirs)

  install(TARGETS modules
    EXPORT ftxui-targets
    FILE_SET CXX_MODULES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    FILE_SET HEADERS
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui
    )
endif()
