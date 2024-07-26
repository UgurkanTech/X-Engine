#!/bin/bash

# Set the Emscripten environment variables
source C:/emsdk/emsdk_env.sh

# Create a build directory
mkdir -p build-emscripten
cd build-emscripten

# Configure the project with CMake and build
emcmake cmake -G "Ninja" -DEMSCRIPTEN=ON .. & emmake ninja -v


