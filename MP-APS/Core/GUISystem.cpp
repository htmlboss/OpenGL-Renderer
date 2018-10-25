#include "GUISystem.h"

#include <cstddef>

#include <glad/glad.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

//https://github.com/vurtun/nuklear/blob/master/demo/glfw_opengl3/main.c

const constexpr std::size_t MaxVertexBuffer{ 512 * 1024 };
const constexpr std::size_t MaxElementBuffer{ 128 * 1024 };

/***********************************************************************************/
void GUISystem::Init(GLFWwindow* windowPtr) {
	m_nuklearContext = nk_glfw3_init(windowPtr, NK_GLFW3_INSTALL_CALLBACKS);

	/* Load Fonts: if none of these are loaded a default font will be used  */
	/* Load Cursor: if you uncomment cursor loading please hide the cursor */
	{
		struct nk_font_atlas* atlas;
		nk_glfw3_font_stash_begin(&atlas);
		nk_glfw3_font_stash_end();
	}
}

/***********************************************************************************/
void GUISystem::Update() {
}

/***********************************************************************************/
void GUISystem::Render() {
	
	struct nk_colorf bg;
	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

	nk_glfw3_new_frame();

	if (nk_begin(m_nuklearContext, "Demo", nk_rect(50, 50, 230, 250),
			NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
			NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			enum {EASY, HARD};
			static int op = EASY;
			static int property = 20;
			nk_layout_row_static(m_nuklearContext, 30, 80, 1);
			if (nk_button_label(m_nuklearContext, "button")) {
				fprintf(stdout, "button pressed\n");
			}

			nk_layout_row_dynamic(m_nuklearContext, 30, 2);
			if (nk_option_label(m_nuklearContext, "easy", op == EASY)) op = EASY;
			if (nk_option_label(m_nuklearContext, "hard", op == HARD)) op = HARD;

			nk_layout_row_dynamic(m_nuklearContext, 25, 1);
			nk_property_int(m_nuklearContext, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(m_nuklearContext, 20, 1);
			nk_label(m_nuklearContext, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(m_nuklearContext, 25, 1);
			if (nk_combo_begin_color(m_nuklearContext, nk_rgb_cf(bg), nk_vec2(nk_widget_width(m_nuklearContext),400))) {
				nk_layout_row_dynamic(m_nuklearContext, 120, 1);
				bg = nk_color_picker(m_nuklearContext, bg, NK_RGBA);
				nk_layout_row_dynamic(m_nuklearContext, 25, 1);
				bg.r = nk_propertyf(m_nuklearContext, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
				bg.g = nk_propertyf(m_nuklearContext, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
				bg.b = nk_propertyf(m_nuklearContext, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
				bg.a = nk_propertyf(m_nuklearContext, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
				nk_combo_end(m_nuklearContext);
			}
		}

	nk_end(m_nuklearContext);


	nk_glfw3_render(NK_ANTI_ALIASING_ON, MaxVertexBuffer, MaxElementBuffer);
}

/***********************************************************************************/
void GUISystem::Shutdown() const {
	nk_glfw3_shutdown();
}
