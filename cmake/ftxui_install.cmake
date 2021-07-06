include(GNUInstallDirs)
install(TARGETS screen dom component
    EXPORT ftxui-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
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
