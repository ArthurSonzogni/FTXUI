find_program( CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
endif()


function(ftxui_set_options library)
  set_target_properties(${library} PROPERTIES OUTPUT_NAME "ftxui-${library}")

  if(CLANG_TIDY_EXE AND FTXUI_CLANG_TIDY)
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

  # C++17 is used. We require fold expression at least.
  target_compile_features(${library} PUBLIC cxx_std_17)

  # Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
  # to the library and the library users.
  if (MSVC)
    target_compile_options(${library} PUBLIC "/utf-8")
  endif()

  # Add as many warning as possible:
  if (WIN32)
    if (MSVC)
      target_compile_options(${library} PRIVATE "/W3")
      target_compile_options(${library} PRIVATE "/WX")
      target_compile_options(${library} PRIVATE "/wd4244")
      target_compile_options(${library} PRIVATE "/wd4267")
      target_compile_options(${library} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
    endif()
    # Force Win32 to UNICODE
    target_compile_definitions(${library} PRIVATE UNICODE _UNICODE)
  else()
    target_compile_options(${library} PRIVATE "-Wall")
    target_compile_options(${library} PRIVATE "-Wextra")
    target_compile_options(${library} PRIVATE "-pedantic")
    target_compile_options(${library} PRIVATE "-Werror")
    target_compile_options(${library} PRIVATE "-Wmissing-declarations")
    target_compile_options(${library} PRIVATE "-Wdeprecated")
    target_compile_options(${library} PRIVATE "-Wshadow")
  endif()

  if (FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    target_compile_definitions(${library}
      PRIVATE "FTXUI_MICROSOFT_TERMINAL_FALLBACK")
  endif()
endfunction()

if (EMSCRIPTEN)
  #string(APPEND CMAKE_CXX_FLAGS " -s ASSERTIONS=1")
  string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD")
endif()

