# Some developers would be happier with the gtest version provided from their
# package manager. Use it if it is installed the package provide cmake support.
# https://github.com/ArthurSonzogni/FTXUI/issues/551
find_package(GTest QUIET)

if (GTest_FOUND)
  return()
endif()

option(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
option(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        23ef29555ef4789f555f1ba8c51b4c52975f0907
  GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(googletest)
if(googletest_POPULATED)
  return()
endif()

FetchContent_Populate(googletest)
set(BUILD_GMOCK OFF CACHE INTERNAL "")
set(INSTALL_GTEST OFF CACHE INTERNAL "")
set(gtest_force_shared_crt ON CACHE INTERNAL "")
add_subdirectory(
  "${googletest_SOURCE_DIR}"
  "${googletest_BINARY_DIR}"
  EXCLUDE_FROM_ALL
)
