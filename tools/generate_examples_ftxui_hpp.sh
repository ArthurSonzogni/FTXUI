#!/bin/bash

# Generate ./examples_ftxui_hpp from ./examples to using the umbrella header 
# ftxui/ftxui.hpp instead of individual #includes for ftxui.
# This is useful for testing the umbrella header.

echo "Generating ./examples_ftxui_hpp"
rm -rf ./examples_ftxui_hpp
cp -r ./examples ./examples_ftxui_hpp

for file in $(find ./examples_ftxui_hpp -name "*.cpp"); do
  echo "Generating $file"

  # Find the position of the first ftxui include
  first_ftxui_include=$(grep -nE '#include [<"]ftxui/' "$file" | head -n 1 | cut -d: -f1)

  # Remove all ftxui includes
  sed -i '/#include "ftxui/d' "$file"
  sed -i '/#include <ftxui/d' "$file"

  if [ -n "$first_ftxui_include" ]; then
    # Put it where the first ftxui include was
    sed -i "${first_ftxui_include}i#include \"ftxui/ftxui.hpp\"" "$file"
  else
    # Find the position of the last remaining include
    last_include=$(grep -n '#include ' "$file" | tail -n 1 | cut -d: -f1)

    if [ -z "$last_include" ]; then
      # If no includes left, put it at the top
      sed -i '1i#include "ftxui/ftxui.hpp"' "$file"
    else
      # Put it after the last include
      sed -i "${last_include}a#include \"ftxui/ftxui.hpp\"" "$file"
    fi
  fi
done

# Update CMakeLists.txt to use the umbrella header ftxui target.
find ./examples_ftxui_hpp -name "CMakeLists.txt" -exec sed -i "s/set(DIRECTORY_LIB .*)/set(DIRECTORY_LIB ftxui)/g" {} +

# Update CMakeLists.txt to use the umbrella header examples name prefix
find ./examples_ftxui_hpp -name "CMakeLists.txt" -exec sed -i 's/ftxui_example_/ftxui_hpp_example_/g' {} +
