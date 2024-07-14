# apple.cmake

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# Use default compilers for macOS
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

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
include_directories("/usr/local/include" "/opt/homebrew/include")
link_directories("/usr/local/lib" "/opt/homebrew/lib")