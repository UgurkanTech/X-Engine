cmake_minimum_required(VERSION 3.15)
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the toolchain file before the project command
if(EMSCRIPTEN)
    cmake_policy(SET CMP0127 NEW)
    
elseif(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchains/windows-ucrt64.cmake" CACHE FILEPATH "Toolchain file")
elseif(APPLE)
    set(BUILD_ARM64 ON)
    set(BUILD_X64 OFF)
    set(BUILD_UNIVERSAL OFF)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchains/apple.cmake" CACHE FILEPATH "Toolchain file")
elseif(UNIX AND NOT APPLE)

endif()

# Set the project name and version
project(Application VERSION 0.1.0 LANGUAGES C CXX)


add_subdirectory(external)

file(GLOB SOURCES src/*.cpp src/*.hpp include/*.cpp shaders/*.sc)

add_executable(Application ${SOURCES})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
# Platform-specific configurations
if(EMSCRIPTEN)
    set(EMSCRIPTEN "C:/emsdk/upstream/emscripten" CACHE PATH "Path to Emscripten SDK" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin-web")

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set_target_properties(Application PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
    set(WASM_BUILD_FLAGS "\
        -s EXPORT_ES6=0 \
        -s NO_EXIT_RUNTIME=1 \
        -s ALLOW_MEMORY_GROWTH \
        -s ENVIRONMENT=web,worker \
        -s ASSERTIONS=1 \
        -s NO_DISABLE_EXCEPTION_CATCHING=1 \
        -s STACK_SIZE=655360 \
        -s INITIAL_MEMORY=52428800 \
        -s EXPORTED_RUNTIME_METHODS=ccall,cwrap  \
        -s EXPORTED_FUNCTIONS=['_main'] \
        -s MAX_WEBGL_VERSION=2 \
        -s FORCE_FILESYSTEM=1 \
        --preload-file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders@/shaders \
        -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WASM_BUILD_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WASM_BUILD_FLAGS}")
    set_target_properties(Application PROPERTIES LINK_FLAGS "${WASM_BUILD_FLAGS}")
    target_compile_definitions(Application PRIVATE EMSCRIPTEN)
elseif(WIN32)
    # Windows-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wformat -static-libgcc -static-libstdc++ -static")
    target_compile_definitions(Application PRIVATE WIN32)
elseif(APPLE)
    # macOS-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC -Wall -Wformat -framework Metal -framework Cocoa -framework QuartzCore -framework GameController -framework CoreHaptics")
    set(CMAKE_C_FLAGS "-x objective-c")
    target_compile_definitions(Application PRIVATE APPLE)
elseif(UNIX AND NOT APPLE)
    # Linux-specific configurations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC -Wall -Wformat")
    target_compile_definitions(Application PRIVATE LINUX)
endif()

# Add any additional libraries or include directories here
target_link_libraries(Application PRIVATE imgui bgfx bx bimg SDL2::SDL2)

target_include_directories(Application PRIVATE "./include")

include(shaders.cmake)