#include "GLRenderer.h"

#include "Skybox.h"
#include "SkySphere.h"
#include "GLShaderProgram.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

// Probably a better way of doing this
std::array<bool, 1024> IRenderer::m_keys;
std::vector<Light> IRenderer::m_lights;
bool GLRenderer::m_firstMouse = true;
double GLRenderer::m_prevX = 1280.0 / 2.0;
double GLRenderer::m_prevY = 720.0 / 2.0;
Camera GLRenderer::m_camera;

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer(width, height) {
	
	std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;

	glewExperimental = GL_TRUE;
	auto err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "GLEW failed to initialize: " << glewGetErrorString(err) << std::endl;
		throw std::runtime_error("");
	}
	
	std::cout << "OpenGL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// Renderer controls blending at runtime.
	glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LESS);

	// depth clamping for l8ter
	
	glViewport(0, 0, width, height);

	// Start up GBuffer
	m_gBuffer = std::make_unique<GBuffer>(GBuffer(width, height));
	m_skybox = std::make_unique<Skybox>(Skybox("skybox/ocean/"));
	//m_skySphere = std::make_unique<SkySphere>();

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	glGenBuffers(1, &m_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate memory, but do not fill
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Define range of the buffer that links to a binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Projection matrix does not change at runtime (constant window size)
	const auto projection = m_camera.GetProjMatrix(width, height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	// Insert data into allocated memory block
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Setup Screenquad
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(m_screenQuadVertices), &m_screenQuadVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
}

/***********************************************************************************/
GLRenderer::~GLRenderer() {
}

/***********************************************************************************/
void GLRenderer::ClearColor(const float r, const float g, const float b, const float a) const {
	glClearColor(r, g, b, a);
}

/***********************************************************************************/
void GLRenderer::Resize(const size_t width, const size_t height) {
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);
}

/***********************************************************************************/
void GLRenderer::GetDepthBuffer() const {
	m_gBuffer->BlitDepthBuffer();
}

/***********************************************************************************/
void GLRenderer::EnableBlending() const {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha blending
}

/***********************************************************************************/
void GLRenderer::Render() const {

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void GLRenderer::RenderSkybox(GLShaderProgram& shader) const {
	
	GetDepthBuffer();
	m_skybox->Draw(shader, m_camera.GetViewMatrix(), m_camera.GetProjMatrix(m_width, m_height));
}

/***********************************************************************************/
void GLRenderer::UpdateMouse(const double xPos, const double yPos) {
	if (m_firstMouse) {
		m_prevX = xPos;
		m_prevY = yPos;
		m_firstMouse = false;
	}

	const auto xOffset = xPos - m_prevX;
	const auto yOffset = m_prevY - yPos; // Reversed since y-coordinates go from bottom to top

	m_prevX = xPos;
	m_prevY = yPos;

	// This can probably be extended into an if-check when using a GUI or something
	m_camera.ProcessMouseMovement(xOffset, yOffset);
}

/***********************************************************************************/
void GLRenderer::Update(const double glfwTimer) {

	// Update timer
	m_timer.Update(glfwTimer);
	const auto deltaTime = m_timer.GetDelta();

	// Update Keyboard
	if (m_keys.at(GLFW_KEY_W)) {
		m_camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	}
	if (m_keys.at(GLFW_KEY_S)) {
		m_camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	}
	if (m_keys.at(GLFW_KEY_A)) {
		m_camera.ProcessKeyboard(Camera::LEFT, deltaTime);
	}
	if (m_keys.at(GLFW_KEY_D)) {
		m_camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
	}
	if (m_keys.at(GLFW_KEY_SPACE)) {
		m_camera.ProcessKeyboard(Camera::UP, deltaTime);
	}
	if (m_keys.at(GLFW_KEY_LEFT_CONTROL)) {
		m_camera.ProcessKeyboard(Camera::DOWN, deltaTime);
	}


	// Update view matrix inside UBO
	const auto view = m_camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
}

/***********************************************************************************/
void GLRenderer::BeginGeometryPass() const {
	m_gBuffer->BindGBuffer();
}

/***********************************************************************************/
void GLRenderer::EndGeometryPass() const {
	m_gBuffer->UnBindGBuffer();
}

/***********************************************************************************/
void GLRenderer::BeginLightingPass() const {
}
