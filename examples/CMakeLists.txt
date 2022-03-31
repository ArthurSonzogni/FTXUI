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
  # 32MB should be enough to run all the examples, in debug mode.
  target_link_options(component PUBLIC "SHELL: -s TOTAL_MEMORY=33554432")
  target_link_options(component PUBLIC "SHELL: -s ASSERTIONS=1")
  #string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
  #target_link_options(component PUBLIC "SHELL: -s ALLOW_MEMORY_GROWTH=1")

  get_property(EXAMPLES GLOBAL PROPERTY FTXUI::EXAMPLES)
  foreach(file
      "index.html"
      "run_webassembly.py")
    configure_file(${file} ${file})
  endforeach(file)
endif()
