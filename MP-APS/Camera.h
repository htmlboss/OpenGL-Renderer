#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Camera {

public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float FOV = 45.0f) noexcept;

	void Update(const double deltaTime) noexcept;

	auto GetViewMatrix() const { return lookAt(m_position, m_position + m_front, m_up); }
	auto GetProjMatrix(const float width, const float height) const { return glm::perspective(m_FOV, width / height, 1.0f, 1000.0f); }
	auto GetPosition() const noexcept { return m_position; }

private:
	enum class Direction {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void updateView(const bool constrainPitch = true) noexcept;

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void processKeyboard(const Direction direction, const double deltaTime) noexcept;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateVectors();

	// Camera Attributes
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	const glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Eular Angles
	double m_yaw, m_pitch;

	const double m_speed = 50.0;
	const double m_sensitivity = 0.3;
	const float m_FOV;

	// Mouse positions
	bool m_firstMouse;
	double m_prevX, m_prevY;
};
