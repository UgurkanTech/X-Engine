# X-Engine
X-Engine is an work-in-progress game engine built in C++ with BGFX, ImGui, and SDL2, configured using CMake for seamless cross-platform development.

Project Status: [■■-----------------------------] 5% Complete


## Description
X-Engine aims to provide a robust foundation for developing modern cross-platform games. It leverages BGFX for efficient rendering, ImGui for immediate mode GUI, and SDL2 for window and input management.

## Build Support

| Platform       | Architecture              | Status              |
| -------------- | ------------------------- | ------------------  |
| Windows        | x64                       | ✅ Successful       |
| Linux          | arm64, x64                | ✅ Successful       |
| macOS  (13.0+) | arm64, x64, Universal     | ✅ Successful       |
| Wasm/Emscripten| -                         | ✅ Successful      |
| Android (14+)  | aarch64, x86_64           | ⏳ Planned Feature  |
| Raspberry Pi   | arm64                     | 📝 Not Tested       |


## Supported Renderers

| Renderer        | Status             |
| --------------- | ------------------ |
| DirectX 11      | ✅ Supported       |
| DirectX 12      | ✅ Supported       |
| Vulkan          | ✅ Supported       |
| OpenGL 2.1+     | ✅ Supported       |
| Metal           | ✅ Supported       |
| OpenGL ES 2.0+  | ✅ Supported       |

## Features
- **Cross-Platform:** Successfully tested on Windows (MSYS2 with ucrt64 toolchain supporting DirectX 11 and 12), macOS (native toolchain with Metal API), and Linux.
- **BGFX Integration:** Leverages BGFX for versatile rendering backend support, including DirectX, Vulkan, OpenGL, Metal, and OpenGL ES.
- **ImGui Integration:** Includes ImGui for creating intuitive and customizable graphical user interfaces.
- **SDL2 Support:** Uses SDL2 for cross-platform window creation, management, and input handling.
