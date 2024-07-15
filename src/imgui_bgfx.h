#ifndef IMGUI_BGFX_H_HEADER_GUARD
#define IMGUI_BGFX_H_HEADER_GUARD

#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <bx/bx.h>
#include "imgui_shaders.h"

static bgfx::VertexLayout  imguiVertexLayout;
static bgfx::TextureHandle imguiFontTexture;
static bgfx::UniformHandle imguiFontUniform;
static bgfx::ProgramHandle imguiProgram;
static GLFWwindow* gWindow = NULL;
static GLFWcursor* gMouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

void imguiInit(GLFWwindow* window);
void imguiReset(uint16_t width, uint16_t height);
void imguiEvents(float dt);
void imguiRender(ImDrawData* drawData);
void imguiShutdown();

// GLFW callbacks
static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void glfw_char_callback(GLFWwindow* window, unsigned int c);
static void glfw_window_size_callback(GLFWwindow* window, int width, int height);


void imguiInit(GLFWwindow* window)
{
    gWindow = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup vertex declaration
    imguiVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    // Create font
    unsigned char* data;
    int width, height;
    io.Fonts->AddFontDefault();
    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);
    imguiFontTexture = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8, 0, bgfx::copy(data, width*height * 4));
    imguiFontUniform = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    // Create shader program
    bgfx::ShaderHandle vs = bgfx::createShader(bgfx::makeRef(vs_getImguiShader(), vs_getImguiShaderLen()));
    bgfx::ShaderHandle fs = bgfx::createShader(bgfx::makeRef(fs_getImguiShader(), fs_getImguiShaderLen()));
    imguiProgram = bgfx::createProgram(vs, fs, true);

    // Setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    // Set initial display size
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)display_w, (float)display_h);

    // Setup clipboard callbacks
    io.SetClipboardTextFn = [](void* user_data, const char* text) {
        glfwSetClipboardString((GLFWwindow*)user_data, text);
    };
    io.GetClipboardTextFn = [](void* user_data) {
        return glfwGetClipboardString((GLFWwindow*)user_data);
    };
    io.ClipboardUserData = gWindow;

    // Create mouse cursors
    gMouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    gMouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    gMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    gMouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    gMouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    gMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    gMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    gMouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    // Set GLFW callbacks
    // Set GLFW callbacks
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetCharCallback(window, glfw_char_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    // Set key mapping
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeypadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    imguiReset((uint16_t)display_w, (uint16_t)display_h);
}

void imguiReset(uint16_t width, uint16_t height)
{
    bgfx::setViewRect(200, 0, 0, width, height);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
}

void imguiEvents(float dt)
{
    ImGuiIO& io = ImGui::GetIO();

    // Update display size
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(gWindow, &w, &h);
    glfwGetFramebufferSize(gWindow, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Update delta time
    io.DeltaTime = dt;

    // Update mouse position
    double mouse_x, mouse_y;
    glfwGetCursorPos(gWindow, &mouse_x, &mouse_y);
    io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);

    // Update mouse cursor
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    glfwSetCursor(gWindow, gMouseCursors[imgui_cursor] ? gMouseCursors[imgui_cursor] : gMouseCursors[ImGuiMouseCursor_Arrow]);
}

void imguiRender( ImDrawData* drawData )
{
	for ( int ii = 0, num = drawData->CmdListsCount; ii < num; ++ii )
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = drawData->CmdLists[ ii ];
		uint32_t numVertices = ( uint32_t )drawList->VtxBuffer.size();
		uint32_t numIndices  = ( uint32_t )drawList->IdxBuffer.size();

		if ( !bgfx::getAvailTransientVertexBuffer( numVertices, imguiVertexLayout ) || !bgfx::getAvailTransientIndexBuffer( numIndices ) )
		{
			break;
		}

		bgfx::allocTransientVertexBuffer( &tvb, numVertices, imguiVertexLayout );
		bgfx::allocTransientIndexBuffer( &tib, numIndices );

		ImDrawVert* verts = ( ImDrawVert* )tvb.data;
		memcpy( verts, drawList->VtxBuffer.begin(), numVertices * sizeof( ImDrawVert ) );

		ImDrawIdx* indices = ( ImDrawIdx* )tib.data;
		memcpy( indices, drawList->IdxBuffer.begin(), numIndices * sizeof( ImDrawIdx ) );

		uint32_t offset = 0;
		for ( const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd )
		{
			if ( cmd->UserCallback )
			{
				cmd->UserCallback( drawList, cmd );
			}
			else if ( 0 != cmd->ElemCount )
			{
				uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA;
				bgfx::TextureHandle th = imguiFontTexture;
				if ( cmd->TextureId != NULL )
				{
					th.idx = uint16_t( uintptr_t( cmd->TextureId ) );
				}
				state |= BGFX_STATE_BLEND_FUNC( BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA );
				const uint16_t xx = uint16_t( bx::max( cmd->ClipRect.x, 0.0f ) );
				const uint16_t yy = uint16_t( bx::max( cmd->ClipRect.y, 0.0f ) );
				bgfx::setScissor( xx, yy, uint16_t( bx::min( cmd->ClipRect.z, 65535.0f ) - xx ), uint16_t( bx::min( cmd->ClipRect.w, 65535.0f ) - yy ) );
				bgfx::setState( state );
				bgfx::setTexture( 0, imguiFontUniform, th );
				bgfx::setVertexBuffer( 0, &tvb, 0, numVertices );
				bgfx::setIndexBuffer( &tib, offset, cmd->ElemCount );
				bgfx::submit( 200, imguiProgram );
			}

			offset += cmd->ElemCount;
		}
	}
}

void imguiShutdown()
{
	for ( ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++ )
	{
		glfwDestroyCursor( gMouseCursors[ cursor_n ] );
		gMouseCursors[ cursor_n ] = NULL;
	}

	bgfx::destroy( imguiFontUniform );
	bgfx::destroy( imguiFontTexture );
	bgfx::destroy( imguiProgram );
	ImGui::DestroyContext();
}

static const char* imguiGetClipboardText( void* userData )
{
	return glfwGetClipboardString( ( GLFWwindow* )userData );
}

static void imguiSetClipboardText( void* userData, const char* text )
{
	glfwSetClipboardString( ( GLFWwindow* )userData, text );
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < ImGuiMouseButton_COUNT)
        io.MouseDown[button] = (action == GLFW_PRESS);
}

static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (key >= 0 && key < 512)
    {
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;
    }

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

static void glfw_char_callback(GLFWwindow* window, unsigned int c)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    imguiReset((uint16_t)width, (uint16_t)height);
}

#endif // IMGUI_BGFX_H_HEADER_GUARD