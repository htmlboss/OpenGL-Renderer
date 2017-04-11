#include "GLRenderer.h"

#include "../Skybox.h"
#include "GLShaderProgram.h"
#include "../Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

std::array<bool, 1024> IRenderer::m_keys;
bool GLRenderer::m_firstMouse = true;
double GLRenderer::m_prevX = 1280.0 / 2.0;
double GLRenderer::m_prevY = 720.0 / 2.0;
bool GLRenderer::m_shouldResize = false;
std::size_t GLRenderer::m_width = 0;
std::size_t GLRenderer::m_height = 0;
std::unique_ptr<Camera> GLRenderer::m_camera = std::make_unique<Camera>();

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer() {
	
	std::cout << "Using OpenGL Renderer.\n\n";

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		throw std::runtime_error("Failed to start GLAD.\n");
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n' 
			  << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	glViewport(0, 0, width, height);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LESS);

	m_gBuffer = std::make_unique<GBuffer>(GBuffer(width, height));
	m_skybox = std::make_unique<Skybox>(Skybox("skybox/ocean/"));

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	glGenBuffers(1, &m_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate memory, but do not fill
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));

	m_projMatrix = m_camera->GetProjMatrix(width, height);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));

	// Setup Screenquad
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_screenQuadVertices.size(), m_screenQuadVertices.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
	
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
	m_shouldResize = true;

	m_width = width;
	m_height = height;
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

/***********************************************************************************/
void GLRenderer::RenderSkybox(GLShaderProgram& shader) const {
	
	GetDepthBuffer();
	m_skybox->Draw(shader, m_camera->GetViewMatrix(), m_projMatrix);
}

/***********************************************************************************/
void GLRenderer::UpdateCameraView(double xPos, double yPos) {
	
	if (m_firstMouse) {
		m_prevX = xPos;
		m_prevY = yPos;
		m_firstMouse = false;
	}

	const auto xOffset = xPos - m_prevX;
	const auto yOffset = m_prevY - yPos; // Reversed since y-coordinates go from bottom to top

	m_prevX = xPos;
	m_prevY = yPos;
	m_camera->ProcessMouseMovement(xOffset, yOffset);
}

/***********************************************************************************/
void GLRenderer::Update(const double deltaTime) {

	// Window size changed.
	if (m_shouldResize) {
		m_projMatrix = m_camera->GetProjMatrix(m_width, m_height);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
		glViewport(0, 0, m_width, m_height);
		m_gBuffer->Resize(m_width, m_height);

		m_shouldResize = false;
	}

	// Update Keyboard
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_W)) {
		m_camera->ProcessKeyboard(Camera::FORWARD, deltaTime);
	}
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_S)) {
		m_camera->ProcessKeyboard(Camera::BACKWARD, deltaTime);
	}
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_A)) {
		m_camera->ProcessKeyboard(Camera::LEFT, deltaTime);
	}
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_D)) {
		m_camera->ProcessKeyboard(Camera::RIGHT, deltaTime);
	}
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_SPACE)) {
		m_camera->ProcessKeyboard(Camera::UP, deltaTime);
	}
	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
		m_camera->ProcessKeyboard(Camera::DOWN, deltaTime);
	}

	// Update view matrix inside UBO
	const auto view = m_camera->GetViewMatrix();
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
