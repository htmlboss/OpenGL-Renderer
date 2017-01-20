#ifdef _DEBUG
	//#include "vld.h"
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <log.h>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Skybox.h"
#include "Timer.h"
#include "RenderUtils.h"
#include "SkySphere.h"
#include "Renderer.h"

#include <iostream>

// Function prototypes
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void window_resize_callback(GLFWwindow* window, int w, int h);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
void RenderQuad();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera
Camera  camera({0.0f, 2.0f, 0.0f});
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

Time timer;
/***********************************************************************************/
int main() {
	// Setup the logger
	FILELog::ReportingLevel() = logDEBUG;
	FILE* log_fd = fopen("log.txt", "w");
	Output2FILE::Stream() = log_fd;

	// Init GLFW
	if (!glfwInit()) {
		std::cerr << "Error: glfwInit() failed." << std::endl;
		throw std::runtime_error("");
	}

	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	if (!window) {
		std::cerr << "Error: glfwCreateWindow() failed." << std::endl;
		throw std::runtime_error("");
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, window_resize_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowPos(window, 300, 200);

	// Start up OpenGL
	Renderer renderer(WIDTH, HEIGHT);
	
	// For wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate memory, but do not fill
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Define range of the buffer that links to a binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Projection matrix does not change at runtime (constant window size)
	const auto projection = camera.GetProjMatrix(WIDTH, HEIGHT);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	// Insert data into allocated memory block
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Shaders
	Shader lightShader("Lamp Shader");
	lightShader.AddShader("shaders/lampvs.glsl", Shader::VertexShader);
	lightShader.AddShader("shaders/lampps.glsl", Shader::PixelShader);
	lightShader.Link();
	lightShader.AddUniforms({ "model", "lightColor" });

	Shader sphereShader("Sphere Shader");
	sphereShader.AddShader("shaders/spherevs.glsl", Shader::VertexShader);
	sphereShader.AddShader("shaders/sphereps.glsl", Shader::PixelShader);
	sphereShader.Link();
	sphereShader.AddUniforms({ "projection", "view", "model"});
	
	Shader skyboxShader("Skybox Shader");
	skyboxShader.AddShader("shaders/skyboxvs.glsl", Shader::VertexShader);
	skyboxShader.AddShader("shaders/skyboxps.glsl", Shader::PixelShader);
	skyboxShader.Link();
	skyboxShader.AddUniforms({ "projection", "view", "skybox" });

	Shader geometryPassShader("Deferred Geometry Pass Shader");
	geometryPassShader.AddShader("shaders/geometrypassvs.glsl", Shader::VertexShader);
	geometryPassShader.AddShader("shaders/geometrypassps.glsl", Shader::PixelShader);
	geometryPassShader.Link();
	geometryPassShader.AddUniforms({ "model", "texture_diffuse1", "texture_specular1"});

	Shader lightingPassShader("Deferred Lighting Pass Shader");
	lightingPassShader.AddShader("shaders/lightingpassvs.glsl", Shader::VertexShader);
	lightingPassShader.AddShader("shaders/lightingpassps.glsl", Shader::PixelShader);
	lightingPassShader.Link();
	lightingPassShader.AddUniforms({"viewPos", "gPosition", "gNormal", "gAlbedoSpec"});
	
	// Set samplers
	lightingPassShader.Bind();
	lightingPassShader.SetUniformi("gPosition", 0);
	lightingPassShader.SetUniformi("gNormal", 1);
	lightingPassShader.SetUniformi("gAlbedoSpec", 2);

	// - Colors
	const GLuint NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; ++i) {
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// Models
	Skybox skybox("skybox/ocean/");
	Light light(glm::vec3(2.3f, 2.0f, -3.0f), glm::vec3(1.0f), Light::POINTLIGHT);
	Model nanosuit("models/nanosuit/nanosuit.obj", "Nanosuit");
	SkySphere sphere;
	nanosuit.SetInstancing({ glm::vec3(0.0f), glm::vec3(-14.575f, 0.0f, 0.0f), glm::vec3(14.575f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 14.575f) });

	renderer.ClearColor();
	// Game loop
	while (!glfwWindowShouldClose(window)) {

		timer.Update(glfwGetTime());
		glfwPollEvents();
		do_movement();

		// Geometry pass
		renderer.m_gBuffer->BindGBuffer();

		const auto view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

		geometryPassShader.Bind();
		
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.25f));
		geometryPassShader.SetUniform("model", model);
		nanosuit.DrawInstanced(geometryPassShader);
		
		renderer.m_gBuffer->UnBindGBuffer();

		// Lighting Pass

		//glUniform1i(lightingPassShader.GetUniformLoc("skybox"), 3);
		//skybox.BindTexture();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightingPassShader.Bind();
		renderer.m_gBuffer->BindTextures();

		// Also send light relevant uniforms
		for (GLuint i = 0; i < lightPositions.size(); ++i)
		{
			glUniform3fv(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Position"), 1, &lightPositions[i][0]);
			glUniform3fv(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Color"), 1, &lightColors[i][0]);
			// Update attenuation parameters and calculate radius
			// Assume constant = 1.0
			static const GLfloat linear = 0.7f;
			static const GLfloat quadratic = 1.8f;
			glUniform1f(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Linear"), linear);
			glUniform1f(lightingPassShader.GetUniformLoc("lights[" + std::to_string(i) + "].Quadratic"), quadratic);
		}
		lightingPassShader.SetUniform("viewPos", camera.GetPosition());

		//skybox.Draw(skyboxShader, camera.GetViewMatrix(), projection);

		/*
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sphereShader.Bind();
		model = glm::mat4x4();
		model = glm::translate(model, { 0.0f, 10.75f, 0.0f });
		sphereShader.SetUniform("model", model);
		sphere.Draw(sphereShader, camera.GetViewMatrix(), projection, camera.GetPosition());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		*/

		RenderQuad();
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}

/***********************************************************************************/
// GLFW Callbacks
void error_callback(int error, const char* description) {
	std::cerr << "Error: " << description;
}

void window_resize_callback(GLFWwindow* window, int w, int h) {
	int fb_w, fb_h;
	glfwGetFramebufferSize(window, &fb_w, &fb_h);

	/// TODO: Rebuild projection matrix

	glViewport(0, 0, fb_w, fb_h);

	//std::cout << "Framebuffer resized to: " << fb_w << " x " << fb_h << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}

void do_movement() {
	// Camera controls
	if (keys[GLFW_KEY_W]) {
		camera.ProcessKeyboard(Camera::FORWARD, timer.GetDelta());
	}
	if (keys[GLFW_KEY_S]) {
		camera.ProcessKeyboard(Camera::BACKWARD, timer.GetDelta());
	}
	if (keys[GLFW_KEY_A]) {
		camera.ProcessKeyboard(Camera::LEFT, timer.GetDelta());
	}
	if (keys[GLFW_KEY_D]) {
		camera.ProcessKeyboard(Camera::RIGHT, timer.GetDelta());
	}
	if (keys[GLFW_KEY_SPACE]) {
		camera.ProcessKeyboard(Camera::UP, timer.GetDelta());
	}
	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		camera.ProcessKeyboard(Camera::DOWN, timer.GetDelta());
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}