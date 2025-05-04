# This script tests the project with different versions of Bazel and compilers
# locally. This avoids waiting on the CI to run the tests.

# Version
# -------
# - Version >= 7 are supported
# - Version <= 6 fail with the error:
#   ERROR: Analysis of target '//:component' failed; build aborted: no such
#          package '@rules_cc//cc/compiler': BUILD file not found in directory
#          'cc/compiler' of external repository @rules_cc. Add a BUILD file to a
#          directory to mark it as a package.
#

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
