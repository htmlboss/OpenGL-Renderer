#ifdef _DEBUG
#include "vld.h"
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <log.h>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Skybox.h"
#include "HUDText.h"
#include "Timer.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera
Camera  camera(glm::vec3(0.0f, 2.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main() {
	FILELog::ReportingLevel() = logDEBUG;
	FILE* log_fd = fopen("log.txt", "w");
	Output2FILE::Stream() = log_fd;
	
	Timer timer;
	timer.Init();

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// For wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// OpenGL options (not state-dependent)
	glEnable(GL_CULL_FACE); // Culling
	glCullFace(GL_BACK); // Back-face culling
	glEnable(GL_MULTISAMPLE); // MSAA from GLFW
	glEnable(GL_BLEND); // Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha blending
	
	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // Allocate memory, but do not fill
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Define range of the buffer that links to a binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Projection matrix does not change at runtime (constant window size)
	glm::mat4 projection = glm::perspective(camera.GetFOV(), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	// Insert data into allocated memory block
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// Shaders
	Shader shader("shaders/nanosuitvs.glsl", "shaders/nanosuitps.glsl");
	Shader lightShader("shaders/lampvs.glsl", "shaders/lampps.glsl");
	Shader skyboxShader("shaders/skyboxvs.glsl", "shaders/skyboxps.glsl");
	Shader fontShader("shaders/fontvs.glsl", "shaders/fontps.glsl");

	HUDText debugText(fontShader, "fonts/arial.ttf", WIDTH, HEIGHT);
	Skybox skybox("skybox/");
	Model nanosuit("models/nanosuit/nanosuit.obj");
	Model floor("models/floor/3d-model.obj");
	Light light(glm::vec3(2.3f, 2.0f, -3.0f), glm::vec3(1.0f));

	// Game loop
	while (!glfwWindowShouldClose(window)) {
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update timer
		timer.Update();

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable depth testing for 3D stuff
		glEnable(GL_DEPTH_TEST);
		// Transformations
		glm::mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		light.Draw(lightShader, view, projection);
		
		shader.Use();
		glUniform3f(shader.GetUniformLoc("viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniformMatrix4fv(shader.GetUniformLoc("view"), 1, GL_FALSE, value_ptr(view));
		// We already have 3 texture units active (in this shader) so set the skybox as the 4th texture unit (texture units are 0 based so index number 3)
		glActiveTexture(GL_TEXTURE3);
		glUniform1i(shader.GetUniformLoc("skybox"), 3);
		skybox.BindTexture();

		// Draw loaded models
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.175f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(shader.GetUniformLoc("model"), 1, GL_FALSE, value_ptr(model));
		nanosuit.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.3f));
		glUniformMatrix4fv(shader.GetUniformLoc("model"), 1, GL_FALSE, value_ptr(model));
		floor.Draw(shader);

		//Always draw skybox last
		skybox.Draw(skyboxShader, camera.GetViewMatrix(), projection);

		// Disable depth test for HUD
		glDisable(GL_DEPTH_TEST);
		// Draw Text on top of everything
		debugText.RenderText(fontShader, std::to_string(timer.GetFPS()) + " fps", 0.0f, HEIGHT - 36.0f, 1.0f, glm::vec3(0.0f));
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement() {
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
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