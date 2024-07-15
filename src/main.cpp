#include <stdio.h>
#include <iostream>
#include <bx/bx.h>
#include <bx/spscqueue.h>
#include <bx/thread.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif
#include <GLFW/glfw3native.h>
#include "imgui_bgfx.h"

static bx::DefaultAllocator s_allocator;
static bx::SpScUnboundedQueue s_apiThreadEvents(&s_allocator);

enum class EventType
{
    Exit,
    Key,
    Resize,
    ImGuiInit
};

struct ExitEvent
{
    EventType type = EventType::Exit;
};

struct KeyEvent
{
    EventType type = EventType::Key;
    int key;
    int action;
};

struct ResizeEvent
{
    EventType type = EventType::Resize;
    uint32_t width;
    uint32_t height;
};

struct ImGuiInitEvent
{
    EventType type = EventType::ImGuiInit;
    GLFWwindow* window;
};

static void glfw_errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto keyEvent = new KeyEvent;
    keyEvent->key = key;
    keyEvent->action = action;
    s_apiThreadEvents.push(keyEvent);
}

struct ApiThreadArgs
{
    bgfx::PlatformData platformData;
    uint32_t width;
    uint32_t height;
};

static int32_t runApiThread(bx::Thread *self, void *userData)
{
    auto args = (ApiThreadArgs *)userData;
    bgfx::Init init;
    init.platformData = args->platformData;
    init.resolution.width = args->width;
    init.resolution.height = args->height;
    init.resolution.reset = BGFX_RESET_VSYNC;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	init.type = bgfx::RendererType::Vulkan;
#elif BX_PLATFORM_OSX
	init.type = bgfx::RendererType::Metal;
#elif BX_PLATFORM_WINDOWS
	init.type = bgfx::RendererType::Direct3D12;
#endif

    if (!bgfx::init(init))
        return 1;
    
	
    const bgfx::ViewId kClearView = 0;
    bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
    uint32_t width = args->width;
    uint32_t height = args->height;
    bool showStats = true;
    bool exit = false;
    
    float lastTime = 0;
    float dt;
    float time;

    bool imguiInitialized = false;

    while (!exit) {
        time = (float)glfwGetTime();
        dt = time - lastTime;
        lastTime = time;

        while (auto ev = (EventType *)s_apiThreadEvents.pop()) {
            if (*ev == EventType::Key) {
                auto keyEvent = (KeyEvent *)ev;
                if (keyEvent->key == GLFW_KEY_F1 && keyEvent->action == GLFW_RELEASE)
                    showStats = !showStats;
            }
            else if (*ev == EventType::Resize) {
                auto resizeEvent = (ResizeEvent *)ev;
                bgfx::reset(resizeEvent->width, resizeEvent->height, init.resolution.reset);
                bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
                width = resizeEvent->width;
                height = resizeEvent->height;
                if (imguiInitialized) {
                    imguiReset(uint16_t(width), uint16_t(height));
                }
            }
            else if (*ev == EventType::ImGuiInit) {
                auto imguiInitEvent = (ImGuiInitEvent *)ev;
                imguiInit(imguiInitEvent->window);
                imguiInitialized = true;
            }
            else if (*ev == EventType::Exit) {
                exit = true;
            }
            delete ev;
        }

        bgfx::touch(kClearView);
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
        bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
        bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
        bgfx::setDebug(showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

		if (imguiInitialized) {
			imguiEvents(dt);
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
			ImGui::EndFrame();
            ImGui::Render();
            imguiRender(ImGui::GetDrawData());
        }

        bgfx::frame();
    }

    if (imguiInitialized) {
        imguiShutdown();
    }
    bgfx::shutdown();
    return 0;
}

int main(int argc, char **argv)
{
    glfwSetErrorCallback(glfw_errorCallback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1024, 768, "helloworld multithreaded", nullptr, nullptr);
    if (!window)
        return 1;
    glfwSetKeyCallback(window, glfw_keyCallback);
    
    #if BX_PLATFORM_OSX
    bgfx::renderFrame();
    #endif
    
    ApiThreadArgs apiThreadArgs;
    #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    apiThreadArgs.platformData.ndt = glfwGetX11Display();
    apiThreadArgs.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
    #elif BX_PLATFORM_OSX
    apiThreadArgs.platformData.nwh = glfwGetCocoaWindow(window);
    #elif BX_PLATFORM_WINDOWS
    apiThreadArgs.platformData.nwh = glfwGetWin32Window(window);
    #endif
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    apiThreadArgs.width = (uint32_t)width;
    apiThreadArgs.height = (uint32_t)height;

    bx::Thread apiThread;
    apiThread.init(runApiThread, &apiThreadArgs);

    // Send ImGui initialization event to the API thread
    auto imguiInitEvent = new ImGuiInitEvent;
    imguiInitEvent->window = window;
    s_apiThreadEvents.push(imguiInitEvent);

    bool exit = false;
    while (!exit) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            s_apiThreadEvents.push(new ExitEvent);
            exit = true;
        }
        int oldWidth = width, oldHeight = height;
        glfwGetWindowSize(window, &width, &height);
        if (width != oldWidth || height != oldHeight) {
            auto resize = new ResizeEvent;
            resize->width = (uint32_t)width;
            resize->height = (uint32_t)height;
            s_apiThreadEvents.push(resize);
        }
        bgfx::renderFrame();
    }
    while (bgfx::RenderFrame::NoContext != bgfx::renderFrame()) {}
    apiThread.shutdown();
    glfwTerminate();
    return apiThread.getExitCode();
}