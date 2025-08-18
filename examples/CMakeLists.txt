if(NOT FTXUI_BUILD_EXAMPLES)
  return()
endif()

set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
function(example name)
  add_executable(ftxui_example_${name} ${name}.cpp)
  target_link_libraries(ftxui_example_${name} PUBLIC ${DIRECTORY_LIB})
  file(RELATIVE_PATH dir ${EXAMPLES_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(GLOBAL APPEND PROPERTY FTXUI::EXAMPLES ${dir}/${name})
  target_compile_features(ftxui_example_${name} PRIVATE cxx_std_20)
endfunction(example)

add_subdirectory(component)
add_subdirectory(dom)

if (EMSCRIPTEN)
  get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)
  foreach(file
      "index.css"
      "index.html"
      "index.mjs"
      "run_webassembly.py"
      "sw.js"
  )
    configure_file(${file} ${file})
  endforeach(file)
endif()
