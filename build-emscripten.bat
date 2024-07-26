@echo on

:: Ensure Emscripten tools are in the PATH
set PATH=C:\msys64\ucrt64\bin;%PATH%

:: Set the Emscripten environment variables
call C:\emsdk\emsdk_env.bat

:: Create a build directory
if not exist build-emscripten mkdir build-emscripten
cd build-emscripten

:: Configure the project with CMake and build
emcmake cmake -G "Ninja" -DEMSCRIPTEN=ON .. & emmake ninja -v

