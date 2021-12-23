set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
function(example name)
  add_executable(ftxui_example_${name} ${name}.cpp)
  target_link_libraries(ftxui_example_${name} PUBLIC ${DIRECTORY_LIB})
  file(RELATIVE_PATH dir ${EXAMPLES_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(GLOBAL APPEND PROPERTY FTXUI::EXAMPLES ${dir}/${name})
endfunction(example)

add_subdirectory(component)
add_subdirectory(dom)

if (EMSCRIPTEN)
  get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)
  foreach(file
      "index.html"
      "run_webassembly.py")
    configure_file(${file} ${file})
  endforeach(file)
endif()
