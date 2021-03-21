add_subdirectory(component)
add_subdirectory(dom)
add_subdirectory(util)

if (EMSCRIPTEN)
  foreach(file
      "index.html"
      "run_webassembly.sh")
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${file}
      ${CMAKE_CURRENT_BINARY_DIR}/${file}
    )
  endforeach(file)
endif()
