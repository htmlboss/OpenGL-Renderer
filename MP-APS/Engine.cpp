#include "Engine.h"
#include "Model.h"

#include <GLFW/glfw3.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::size_t width, const std::size_t height) : m_engineState(engineState::LOADING), 
																	m_mainWindow(new GLWindow(width, height, "MP-APS")), 
																	m_renderer(new GLRenderer(width, height)) 
																	{
}

/***********************************************************************************/
void Engine::Execute() {

	// Shaders
	GLShaderProgram lightShader("Lamp Shader", { Shader("shaders/lampvs.glsl", Shader::VertexShader), Shader("shaders/lampps.glsl", Shader::PixelShader) });
	lightShader.AddUniforms({ "model", "lightColor" });

	GLShaderProgram sphereShader("Sphere Shader", { Shader("shaders/spherevs.glsl", Shader::VertexShader), Shader("shaders/sphereps.glsl", Shader::PixelShader) });
	sphereShader.AddUniforms({ "projection", "view", "model"});
	
	GLShaderProgram skyboxShader("Skybox Shader", { Shader("shaders/skyboxvs.glsl", Shader::VertexShader), Shader("shaders/skyboxps.glsl", Shader::PixelShader) });
	skyboxShader.AddUniforms({ "projection", "view", "skybox" });

	GLShaderProgram geometryPassShader("Deferred Geometry Pass Shader", { Shader("shaders/geometrypassvs.glsl", Shader::VertexShader), Shader("shaders/geometrypassps.glsl", Shader::PixelShader) });
	geometryPassShader.AddUniforms({ "model", "normalMatrix", "texture_diffuse1", "texture_specular1"});

	GLShaderProgram lightingPassShader("Deferred Lighting Pass Shader", { Shader("shaders/lightingpassvs.glsl", Shader::VertexShader), Shader("shaders/lightingpassps.glsl", Shader::PixelShader) });
	lightingPassShader.AddUniforms({"viewPos", "gPosition", "gNormal", "gAlbedoSpec"});
	
	// Set samplers
	lightingPassShader.Bind();
	lightingPassShader.SetUniformi("gPosition", 0);
	lightingPassShader.SetUniformi("gNormal", 1);
	lightingPassShader.SetUniformi("gAlbedoSpec", 2);

	// Models
	Model sponza("models/crytek-sponza/sponza.obj", "Crytek Sponza");


	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n\n";
	std::cout << "**************************************************" << '\n';

	while (!m_mainWindow->ShouldClose()) {
		this->update();

		// Geometry pass
		m_renderer->BeginGeometryPass();

		geometryPassShader.Bind();

		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.1f));
		geometryPassShader.SetUniform("model", model);
		geometryPassShader.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sponza.Draw(geometryPassShader);

		m_renderer->EndGeometryPass();
		
		// Lighting pass
		m_renderer->BeginLightingPass();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightingPassShader.Bind();
		m_renderer->m_gBuffer->BindTextures();

		lightingPassShader.SetUniform("viewPos", m_renderer->GetCameraPos());

		m_renderer->Render();
		m_renderer->RenderSkybox(skyboxShader);

		m_mainWindow->SwapBuffers();
	}

	this->shutdown();
}

/***********************************************************************************/
void Engine::update() {
	m_mainWindow->Update();
	m_timer.Update(glfwGetTime());
	m_renderer->Update(m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() {
	m_mainWindow->DestroyWindow();
}
