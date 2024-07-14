cmake_minimum_required(VERSION 3.15)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the toolchain file before the project command
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchains/windows-ucrt64.cmake" CACHE FILEPATH "Toolchain file")
elseif(APPLE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchains/apple.cmake" CACHE FILEPATH "Toolchain file")
elseif(UNIX AND NOT APPLE)

endif()

# Set the project name and version
project(Application VERSION 0.1.0 LANGUAGES C CXX)
# Set Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Add your source files
file(GLOB SOURCES "./src/*.cpp")

add_executable(Application ${SOURCES})

# Platform-specific configurations
if(WIN32)
    # Windows-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static-libgcc -static-libstdc++ -static")
    target_compile_definitions(Application PRIVATE WIN32)
elseif(APPLE)
    # macOS-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC")
    target_compile_definitions(Application PRIVATE APPLE)
elseif(UNIX AND NOT APPLE)
    # Linux-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC")
    target_compile_definitions(Application PRIVATE LINUX)
endif()

# Add any additional libraries or include directories here
#target_link_libraries(Application PRIVATE )
target_include_directories(Application PRIVATE "./include")