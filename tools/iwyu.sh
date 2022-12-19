#!/bin/bash
cd "$(dirname "$0")"
cd ..
mapping_dir=$(pwd)
mkdir -p iwyu
cd iwyu
rm * -rf
echo $CMAKE_CXX_INCLUDE_WHAT_YOU_USE
cmake .. -DFTXUI_BUILD_TESTS=ON -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="include-what-you-use;-Xiwyu;--cxx17ns;-Xiwyu;--mapping_file=${mapping_dir}/iwyu.imp;-Xiwyu;--verbose=3"
make -j 2>out
fix_include --comments < out

../tools/format.sh
