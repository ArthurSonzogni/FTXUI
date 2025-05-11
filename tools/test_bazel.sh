# This script tests the project with different versions of Bazel and compilers
# locally. This avoids waiting on the CI to run the tests.

for ver in \
  "6.0.0" \
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
