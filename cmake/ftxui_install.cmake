if(NOT FTXUI_ENABLE_INSTALL)
  return()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ------------------------------------------------------------------------------
# Install the library and its public headers into the standard subdirectories.
# On multi-config generators (e.g. Visual Studio) both Debug and Release are
# installed into separate lib/<config>/ subdirectories so they can coexist in
# the same package without overwriting each other.  Single-config generators
# (Ninja, Make) keep the traditional flat lib/ layout.
# Generator expressions in DESTINATION require CMake 3.20.
# ------------------------------------------------------------------------------
get_property(_ftxui_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_ftxui_isMultiConfig)
  if(CMAKE_VERSION VERSION_LESS "3.20")
    message(WARNING
      "FTXUI: CMake >= 3.20 is required to install Debug and Release "
      "libraries into separate subdirectories on multi-config generators. "
      "Falling back to a flat lib/ layout (the two configurations will "
      "overwrite each other). Please upgrade CMake.")
    set(_ftxui_isMultiConfig FALSE)
  endif()
endif()

if(_ftxui_isMultiConfig)
  install(
    TARGETS screen dom component ftxui
    EXPORT ftxui-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
  )
else()
  install(
    TARGETS screen dom component ftxui
    EXPORT ftxui-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )
endif()

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
