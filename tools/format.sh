#!/bin/bash
cd "$(dirname "$0")"
cd ..

# Add the license file.
files=$(find ./src ./include ./examples  -name "*.hpp" -o -name "*.cpp")
for file in $files
do
  if ! grep -q Copyright $file
  then
    cat $file ./tools/license_headers.cpp > $file.new && mv $file.new $file
  fi
done

# Use clang-format.
for file in $files
do
  clang-format -i $file
done

exampleList="./doc/example_list.md"
echo "# Examples" > $exampleList
files=$(find ./examples/ -iname "*.cpp")
for f in $files
do
  echo "@example $f" >> $exampleList
done
