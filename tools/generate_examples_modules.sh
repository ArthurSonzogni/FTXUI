#!/bin/bash

# Generate ./examples_modules from ./examples to using C++20 modules instead of 
# #includes for ftxui.
# This is useful for testing ftxui with modules. This ensures we don't forget
# to update the FTXUI modules when adding new features to FTXUI.

echo "Generating ./examples_modules"
rm -rf ./examples_modules
cp -r ./examples ./examples_modules

for file in ./examples_modules/**/*.cpp; do
  echo "Generating $file"

  sed -i '/#include "ftxui/d' "$file"
  sed -i '/#include <ftxui/d' "$file"
  last_include=$(grep -n '#include <' "$file" | tail -n 1 | cut -d: -f1)
  if [ -z "$last_include" ]; then
    sed -i '1i\import ftxui.component;\nimport ftxui.dom;\nimport ftxui.screen;\nimport ftxui.util;\n' "$file"
  else
    sed -i "${last_include}a\\\nimport ftxui.component;\nimport ftxui.dom;\nimport ftxui.screen;\nimport ftxui.util;" "$file"
  fi
done

# Modify the CMakeLists.txt file to link against ftxui-modules
sed -i 's/${DIRECTORY_LIB}/ftxui-modules/g' ./examples_modules/CMakeLists.txt
sed -i 's/ftxui_example_/ftxui_modules_example_/g' ./examples_modules/CMakeLists.txt



