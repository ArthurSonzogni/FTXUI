file(GLOB_RECURSE FTXUI_MODULES *.cppm)

add_library(ftxui_modules)

cmake_minimum_required(VERSION 3.28)

if(NOT COMMAND configure_cpp_module_target)
  function(configure_cpp_module_target target)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
      target_sources(${target} PUBLIC FILE_SET CXX_MODULES FILES ${FTXUI_MODULES})
    else()
      message(WARNING "C++ modules require CMake 3.28+. Using standard compilation.")
      target_sources(${target} PRIVATE ${FTXUI_MODULES})
    endif()
  endfunction()
endif()

configure_cpp_module_target(ftxui_modules)

target_link_libraries(ftxui_modules
  PUBLIC
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_include_directories(ftxui_modules
  PRIVATE
    ${ftxui_SOURCE_DIR}/include
)

set_target_properties(ftxui_modules PROPERTIES 
  CXX_STANDARD 23 
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF 
  CXX_MODULE_DYNDEP OFF
)

add_library(ftxui::modules ALIAS ftxui_modules)

if(FTXUI_ENABLE_INSTALL)
  install(TARGETS ftxui_modules
    EXPORT ftxui-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ftxui/modules
  )
endif()