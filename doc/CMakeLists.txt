if(NOT FTXUI_BUILD_DOCS)
  return()
endif()

find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
  return()
endif()

include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG v2.3.4

)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# Generate example list for documentation
set(DOM_EXAMPLES "")
set(COMPONENT_EXAMPLES "")

get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)

foreach(example IN LISTS EXAMPLES)
  if(example MATCHES "^dom/.*")
    list(APPEND DOM_EXAMPLES "${example}")
  elseif(example MATCHES "^component/.*")
    list(APPEND COMPONENT_EXAMPLES "${example}")
  else()
    message(ERROR "Unknown example '${example}'")
  endif()
endforeach()

macro(write_example_list file title page examples)
  file(WRITE "${file}" "@page ${page} ${title}\n")
  file(APPEND "${file}" "@tableofcontents\n")

  foreach(example IN LISTS ${examples})
    get_filename_component(name "${example}" NAME_WE)
    file(APPEND "${file}" "# ${name}\n")

    # Add a markdown  to the demo. URL example:
    # https://arthursonzogni.github.io/FTXUI/examples/?file=component/canvas_animated
    file(APPEND "${file}" "[Demo](https://arthursonzogni.github.io/FTXUI/examples/?file=${example})\n")

    file(APPEND "${file}" "@include examples/${example}.cpp\n")
    file(APPEND "${file}" "\n")
  endforeach()

  # Reference to the examples
  foreach(example IN LISTS ${examples})
    get_filename_component(name "${example}" NAME_WE)
    file(APPEND "${file}" "@example examples/${example}.cpp\n")
  endforeach()
endmacro()

write_example_list("${CMAKE_CURRENT_BINARY_DIR}/dom_examples.md"
  "Example"
  module-dom-examples
  DOM_EXAMPLES)
write_example_list("${CMAKE_CURRENT_BINARY_DIR}/component_examples.md"
  "Example"
  module-component-examples
  COMPONENT_EXAMPLES)

configure_file(Doxyfile.in Doxyfile @ONLY)

# note the option ALL which allows to build the docs together with the application
add_custom_target(doc
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM
  )
