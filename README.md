# X-Engine
X-Engine is an work-in-progress game engine built in C++ with BGFX, ImGui, and GLFW3, configured using CMake for seamless cross-platform development.

Project Status: [■■-----------------------------] 5% Complete


## Description
X-Engine aims to provide a robust foundation for developing modern cross-platform games. It leverages BGFX for efficient rendering, ImGui for immediate mode GUI, and GLFW3 for window and input management.

## Build Support

| Platform       | Architecture              | Status              |
| -------------- | ------------------------- | ------------------  |
| Windows        | x64                       | ✅ Successful       |
| Linux          | arm64, x64                | ✅ Successful       |
| macOS  (13.0+) | arm64, x64, Universal     | ✅ Successful       |
| Android (14+)  | aarch64, x86_64           | ⏳ Planned Feature  |
| Raspberry Pi   | arm64                     | 📝 Not Tested       |
| Wasm/Emscripten| -                         | ❌ Not Supported    |

## Supported Renderers

| Renderer        | Status             |
| --------------- | ------------------ |
| DirectX 11      | ✅ Supported       |
| DirectX 12      | ✅ Supported       |
| Vulkan          | ✅ Supported       |
| OpenGL 2.1+     | ✅ Supported       |
| Metal           | ✅ Supported       |
| OpenGL ES 2.0+  | ✅ Supported       |
| Wasm/Emscripten | ❌ Not Supported   |

## Features
- **Cross-Platform:** Successfully tested on Windows (MSYS2 with ucrt64 toolchain supporting DirectX 11 and 12), macOS (native toolchain with Metal API), and Linux.
- **BGFX Integration:** Utilizes BGFX's capabilities, supporting multiple rendering backends.
- **ImGui Integration:** Includes ImGui for intuitive GUI development.
- **GLFW3 Support:** Utilizes GLFW3 for unified window management and input handling.
- **Work-in-Progress:** Actively under development to add more features and optimizations.
