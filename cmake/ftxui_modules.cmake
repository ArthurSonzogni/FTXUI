if (NOT FTXUI_BUILD_MODULES)
  return()
endif()

add_library(ftxui-modules)

target_sources(ftxui-modules
  PUBLIC FILE_SET CXX_MODULES FILES
  src/ftxui/ftxui.cppm
  src/ftxui/component.cppm
  src/ftxui/dom.cppm
  src/ftxui/screen.cppm
  src/ftxui/util.cppm
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
endif()
