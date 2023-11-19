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
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
  target_link_options(component PUBLIC "SHELL: -s ALLOW_MEMORY_GROWTH=1")

  get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)
  foreach(file
      "index.html"
      "index.mjs"
      "index.css"
      "sw.js"
      "run_webassembly.py")
    configure_file(${file} ${file})
  endforeach(file)
endif()
