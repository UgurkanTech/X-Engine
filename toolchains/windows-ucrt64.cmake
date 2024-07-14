# windows-ucrt64.cmake

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Use environment variable for MSYS2 path if set, otherwise use default
if(DEFINED ENV{MSYS2_PATH})
    set(MSYS2_PATH $ENV{MSYS2_PATH})
else()
    set(MSYS2_PATH "C:/msys64")
endif()

set(TOOLCHAIN_PATH "${MSYS2_PATH}/ucrt64")

# Specify the locations of the tools
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/bin/g++.exe")
set(CMAKE_RC_COMPILER "${TOOLCHAIN_PATH}/bin/windres.exe")

# Set up pkg-config
set(ENV{PKG_CONFIG_PATH} "${TOOLCHAIN_PATH}/lib/pkgconfig")
set(PKG_CONFIG_EXECUTABLE "${TOOLCHAIN_PATH}/bin/pkg-config.exe")

# Where to look for libraries and headers
set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_PATH}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Adjust PATH
list(APPEND CMAKE_PROGRAM_PATH "${TOOLCHAIN_PATH}/bin")

# Libraries and headers
include_directories("${TOOLCHAIN_PATH}/include")
link_directories("${TOOLCHAIN_PATH}/lib")