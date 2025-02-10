find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )

function(ftxui_set_options library)
  set_target_properties(${library} PROPERTIES VERSION ${PROJECT_VERSION})
  if (NOT ${library} MATCHES "ftxui-*")
    set_target_properties(${library} PROPERTIES OUTPUT_NAME "ftxui-${library}")
  endif()

  if (FTXUI_CLANG_TIDY)
    if (NOT CLANG_TIDY_EXE)
      message(FATAL_ERROR "clang-tidy requested but executable not found")
    endif()

    set_target_properties(${library}
      PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*"
    )

    # By using "PUBLIC" as opposed to "SYSTEM INTERFACE", the compiler warning
    # are enforced on the headers. This is behind "FTXUI_CLANG_TIDY", so that it
    # applies only when developing FTXUI and on the CI. User's of the library
    # get only the SYSTEM INTERFACE instead.
    target_include_directories(${library}
      PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
  else()
    target_include_directories(${library} SYSTEM
      INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
  endif()

  target_include_directories(${library} SYSTEM
    INTERFACE
      $<INSTALL_INTERFACE:include>
  )

  target_include_directories(${library}
    PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )

  target_compile_features(${library} PUBLIC cxx_std_17)

  # Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
  # to the library and the library users.
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${library} PUBLIC "/utf-8")
  endif()

  # CMake does automatically add -fPIC when linking a shared library, but it
  # does not add it when linking a static library. This is a problem when the 
  # static library is later linked into a shared library. 
  # Doing it helps some users.
  set_property(TARGET ${library} PROPERTY POSITION_INDEPENDENT_CODE ON)

  # Add as many warning as possible:
  if (WIN32)
    if (MSVC)
      if(FTXUI_DEV_WARNINGS)
        target_compile_options(${library} PRIVATE "/W3")
        target_compile_options(${library} PRIVATE "/WX")
      endif()
      target_compile_options(${library} PRIVATE "/wd4244")
      target_compile_options(${library} PRIVATE "/wd4267")
      target_compile_options(${library} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
    endif()
    # Force Win32 to UNICODE
    target_compile_definitions(${library} PRIVATE UNICODE _UNICODE)
  else()
    if(FTXUI_DEV_WARNINGS)
      target_compile_options(${library} PRIVATE "-Wall")
      target_compile_options(${library} PRIVATE "-Werror")
      target_compile_options(${library} PRIVATE "-Wextra")
  
      target_compile_options(${library} PRIVATE "-Wcast-align")
      target_compile_options(${library} PRIVATE "-Wdeprecated")
      target_compile_options(${library} PRIVATE "-Wmissing-declarations")
      target_compile_options(${library} PRIVATE "-Wnon-virtual-dtor")
      target_compile_options(${library} PRIVATE "-Wnull-dereference")
      target_compile_options(${library} PRIVATE "-Woverloaded-virtual")
      target_compile_options(${library} PRIVATE "-Wpedantic")
      target_compile_options(${library} PRIVATE "-Wshadow")
      target_compile_options(${library} PRIVATE "-Wunused")
    endif()
  endif()

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(FTXUI_DEV_WARNINGS)
      target_compile_options(${library} PRIVATE "-Wdocumentation")
    endif()
  endif()


  if (FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    target_compile_definitions(${library}
      PRIVATE "FTXUI_MICROSOFT_TERMINAL_FALLBACK")
  endif()
endfunction()

if (EMSCRIPTEN)
  string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD")
endif()
