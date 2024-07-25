#!/bin/sh
echo "Compiling shaders..."
cmake --build build --target compile_shaders
echo "Done compiling shaders."