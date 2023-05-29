if(NOT FTXUI_ENABLE_INSTALL)
  return()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ------------------------------------------------------------------------------
# Install the library and its public headers into the standard subdirectories
# ------------------------------------------------------------------------------
install(
  TARGETS screen dom component
  EXPORT ftxui-targets
  )

install(
  DIRECTORY include/ftxui
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

# ------------------------------------------------------------------------------
# Install the exported cmake targets for use in other CMake projects.
# ------------------------------------------------------------------------------
install(
  EXPORT ftxui-targets
  NAMESPACE ftxui::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ftxui"
  )

# ------------------------------------------------------------------------------
# Create and install the ftuxi-config.cmake and ftuxi-config-version.cmake files
# needed to support users of find_package()
# ------------------------------------------------------------------------------
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ftxui-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/ftxui-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/ftxui/cmake"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  )
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/ftxui-config-version.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY SameMajorVersion
  )
install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/ftxui-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/ftxui-config-version.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ftxui"
  )

# ------------------------------------------------------------------------------
# Create and install pkg-config file for easy use of library in build systems
# other than CMake:
# ------------------------------------------------------------------------------
configure_file(ftxui.pc.in ${CMAKE_CURRENT_BINARY_DIR}/ftxui.pc @ONLY)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/ftxui.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
  )
