find_package(Doxygen)
if (DOXYGEN_FOUND)
  # Generate example list for documentation
  set(EXAMPLE_LIST "${CMAKE_CURRENT_BINARY_DIR}/example_list.md")
  file(WRITE ${EXAMPLE_LIST} "# Examples")
  get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)
  foreach(EXAMPLE IN LISTS EXAMPLES)
    file(APPEND ${EXAMPLE_LIST} "\n@example examples/${EXAMPLE}.cpp")
  endforeach(EXAMPLE IN LISTS EXAMPLES)

  configure_file(Doxyfile.in Doxyfile @ONLY)

  # note the option ALL which allows to build the docs together with the application
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
    )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
