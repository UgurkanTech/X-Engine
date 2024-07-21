# apple.cmake

# Architecture options
option(BUILD_ARM64 "Build for ARM64 architecture" ON)
option(BUILD_X64 "Build for x64 architecture" OFF)
option(BUILD_UNIVERSAL "Build universal binary (both ARM64 and x64)" OFF)

# Static build option
option(BUILD_STATIC "Build static libraries" OFF)

set(CMAKE_SYSTEM_NAME Darwin)

if(BUILD_UNIVERSAL)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    set(CMAKE_SYSTEM_PROCESSOR "universal")
elseif(BUILD_X64)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR "x86_64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_SYSTEM_PROCESSOR "arm64")
endif()

# Use default compilers for macOS
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_OBJC_COMPILER "/usr/bin/clang")

# Set up pkg-config
find_program(PKG_CONFIG_EXECUTABLE NAMES pkg-config PATHS /usr/local/bin /opt/homebrew/bin)

# Where to look for libraries and headers
set(CMAKE_FIND_ROOT_PATH "/usr/local;/opt/homebrew")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Adjust PATH
list(APPEND CMAKE_PROGRAM_PATH "/usr/local/bin" "/opt/homebrew/bin")

# Libraries and headers
include_directories("/usr/local/include" "/opt/homebrew/include" "/opt/local/include")
link_directories("/usr/local/lib" "/opt/homebrew/lib" "/opt/local/lib")

set(COCOA_LIBRARY "-framework Cocoa")
set(COREFOUNDATION_LIBRARY "-framework CoreFoundation")
set(FOUNDATION_LIBRARY "-framework Foundation")
set(IOKIT_LIBRARY "-framework IOKit")
set(METAL_LIBRARY "-framework Metal")
set(QUARTZCORE_LIBRARY "-framework QuartzCore")
set(GAMECONTROLLER_LIBRARY "-framework GameController")
set(COREHAPTICS_LIBRARY "-framework CoreHaptics")

# Static build settings
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

