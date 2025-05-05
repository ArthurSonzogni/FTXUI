# This script tests the project with different versions of Bazel and compilers
# locally. This avoids waiting on the CI to run the tests.

# Version
# -------
# - Version >= 7 are supported
# - Version <= 6 fail with the error:
#   Error:
#     external/googletest~1.14.0.bcr.1/googletest/include/gtest/internal/gtest-port.h:279:2:
#     error: #error C++ versions less than C++14 are not supported.
for ver in \
  "7.0.0" \
  "8.0.0"
do
  for cc in \
    "gcc" \
    "clang"
    do
      echo "=== Testing with Bazel ${ver} with ${cc} ==="
      USE_BAZEL_VERSION=${ver} CC=${cc} bazel clean --expunge
      USE_BAZEL_VERSION=${ver} CC=${cc} bazel build //...
      USE_BAZEL_VERSION=${ver} CC=${cc} bazel test //...
    done
  done
