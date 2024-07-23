#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>
#include <chrono> 

#include "imgui_impl_bgfx.h"
#include "file-ops.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN


struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cube_vertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cube_tri_list[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

static bgfx::ShaderHandle create_shader(
    const std::string& shader, const char* name)
{
    const bgfx::Memory* mem = bgfx::copy(shader.data(), shader.size());
    const bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, name);
    return handle;
}

struct context_t
{
    SDL_Window* window = nullptr;
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    float cam_pitch = 0.0f;
    float cam_yaw = 0.0f;
    float rot_scale = 0.01f;

    int prev_mouse_x = 0;
    int prev_mouse_y = 0;

    int width = 0;
    int height = 0;

    bool quit = false;

    int reset = BGFX_RESET_NONE;
};

 
// Variables for FPS calculation
auto startTime = std::chrono::high_resolution_clock::now();
int frameCount = 0;

void main_loop(void* data)
{
    auto context = static_cast<context_t*>(data);

    for (SDL_Event current_event; SDL_PollEvent(&current_event) != 0;) {
        ImGui_ImplSDL2_ProcessEvent(&current_event);
        if (current_event.type == SDL_QUIT) {
            context->quit = true;
            break;
        } else if (current_event.type == SDL_WINDOWEVENT) {
            if (current_event.window.event == SDL_WINDOWEVENT_RESIZED) {
                context->width = current_event.window.data1;
                context->height = current_event.window.data2;
                bgfx::reset(context->width, context->height, context->reset);
                bgfx::setViewRect(0, 0, 0, context->width, context->height);
            }
        }
    }

    ImGui_Implbgfx_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // your drawing here
    ImGui::Render();
    

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();


    ImGui_Implbgfx_RenderDrawLists(255, ImGui::GetDrawData());



    if (!ImGui::GetIO().WantCaptureMouse) {
        // simple input code for orbit camera
        int mouse_x, mouse_y;
        const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
            int delta_x = mouse_x - context->prev_mouse_x;
            int delta_y = mouse_y - context->prev_mouse_y;
            context->cam_yaw += float(-delta_x) * context->rot_scale;
            context->cam_pitch += float(-delta_y) * context->rot_scale;
        }
        context->prev_mouse_x = mouse_x;
        context->prev_mouse_y = mouse_y;
        
    }

    float cam_rotation[16];
    bx::mtxRotateXYZ(cam_rotation, context->cam_pitch, context->cam_yaw, 0.0f);

    float cam_translation[16];
    bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

    float cam_transform[16];
    bx::mtxMul(cam_transform, cam_translation, cam_rotation);

    float view[16];
    bx::mtxInverse(view, cam_transform);

    float proj[16];
    bx::mtxProj(
        proj, 60.0f, float(context->width) / float(context->height), 0.1f,
        100.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    float model[16];
    bx::mtxIdentity(model);
    bgfx::setTransform(model);

    bgfx::setVertexBuffer(0, context->vbh);
    bgfx::setIndexBuffer(context->ibh);

    bgfx::submit(0, context->program);

    //set title with renderer and fps

     // Frame timing
    frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - startTime;
    if (elapsedTime.count() >= 1.0f) {
        int fps = frameCount / elapsedTime.count();
        std::string title = "X-Engine: " + std::string(bgfx::getRendererName(bgfx::getRendererType())) + " FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(context->window, title.c_str());
        // Reset timing
        startTime = currentTime;
        frameCount = 0;
    }

    

    bgfx::frame();

#if BX_PLATFORM_EMSCRIPTEN
    if (context->quit) {
        emscripten_cancel_main_loop();
    }
#endif
}

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const int width = 800;
    const int height = 600;
    SDL_Window* window = SDL_CreateWindow(
        argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
        height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

#if !BX_PLATFORM_EMSCRIPTEN
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        printf(
            "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
            SDL_GetError());
        return 1;
    }
    bgfx::renderFrame(); // single threaded mode
#endif // !BX_PLATFORM_EMSCRIPTEN

    bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_OSX
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_LINUX
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_EMSCRIPTEN
    pd.nwh = (void*)"#canvas";
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?
       // BX_PLATFORM_EMSCRIPTEN

    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    bgfx_init.resolution.width = width;
    bgfx_init.resolution.height = height;
    bgfx_init.resolution.reset = BGFX_RESET_NONE;
    bgfx_init.platformData = pd;
    //bgfx_init.type = bgfx::RendererType::OpenGL;
    bgfx::init(bgfx_init);


    //bgfx::setDebug(BGFX_DEBUG_STATS);

    bgfx::setViewClear(
        0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;


    ImGui_Implbgfx_Init(255);
#if BX_PLATFORM_WINDOWS
    ImGui_ImplSDL2_InitForD3D(window);
#elif BX_PLATFORM_OSX
    ImGui_ImplSDL2_InitForMetal(window);
    
#elif BX_PLATFORM_LINUX || BX_PLATFORM_EMSCRIPTEN
    ImGui_ImplSDL2_InitForOpenGL(window, nullptr);
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?
       // BX_PLATFORM_EMSCRIPTEN

    bgfx::VertexLayout pos_col_vert_layout;
    pos_col_vert_layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
        pos_col_vert_layout);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

    std::string shader_root = "shaders/";

    switch(bgfx::getRendererType())
    {
        case bgfx::RendererType::Direct3D11: printf("Direct3D11\n"); 
        case bgfx::RendererType::Direct3D12: printf("Direct3D12\n"); shader_root = "shaders/dx11/"; break;
        case bgfx::RendererType::Metal: printf("Metal\n"); shader_root = "shaders/metal/";  break;
        case bgfx::RendererType::OpenGL: printf("OpenGL\n"); shader_root = "shaders/glsl/";  break;
        case bgfx::RendererType::OpenGLES: printf("OpenGLES\n"); shader_root = "shaders/essl/"; break;
        case bgfx::RendererType::Vulkan: printf("Vulkan\n"); shader_root = "shaders/spirv/"; break;
        case bgfx::RendererType::Noop: printf("Noop\n"); break;
        case bgfx::RendererType::Count: printf("Count\n"); break;
        default: printf("Unknown renderer type\n"); break;
    }


    std::string vshader;
    if (!fileops::read_file(shader_root + "cubes.vs.bin", vshader)) {
        printf("Could not find shader vertex shader (ensure shaders have been "
               "compiled).\n"
               "Run compile-shaders-<platform>.sh/bat\n");
        return 1;
    }

    std::string fshader;
    if (!fileops::read_file(shader_root + "cubes.fs.bin", fshader)) {
        printf("Could not find shader fragment shader (ensure shaders have "
               "been compiled).\n"
               "Run compile-shaders-<platform>.sh/bat\n");
        return 1;
    }

    bgfx::ShaderHandle vsh = create_shader(vshader, "vshader");
    bgfx::ShaderHandle fsh = create_shader(fshader, "fshader");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    context_t context;
    context.width = width;
    context.height = height;
    context.program = program;
    context.window = window;
    context.vbh = vbh;
    context.ibh = ibh;
    context.reset = bgfx_init.resolution.reset;



#if BX_PLATFORM_EMSCRIPTEN
    emscripten_set_main_loop_arg(main_loop, &context, -1, 1);
#else
    while (!context.quit) {
        main_loop(&context);
    }
#endif // BX_PLATFORM_EMSCRIPTEN

    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(program);

    ImGui_ImplSDL2_Shutdown();
    ImGui_Implbgfx_Shutdown();

    ImGui::DestroyContext();
    bgfx::shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}