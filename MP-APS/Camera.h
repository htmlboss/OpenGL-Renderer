#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Camera {

public:
	Camera() noexcept;

	void SetNear(const float near);
	void SetFar(const float far);
	void SetSpeed(const float speed);

	void Update(const double deltaTime);

	auto GetViewMatrix() const { return lookAt(m_position, m_position + m_front, m_up); }
	// TODO: optimize projection matrix calculation
	auto GetProjMatrix(const float width, const float height) const { return glm::perspective(m_FOV, width / height, m_near, m_far); }
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

	// Processes input received from mouse.
	void updateView(const bool constrainPitch = true);

	// Processes input received from keyboard.
	void processKeyboard(const Direction direction, const double deltaTime) noexcept;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateVectors();

	// Camera Attributes
	glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_front{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };
	glm::vec3 m_right;
	const glm::vec3 m_worldUp{ 0.0f, 1.0f, 0.0f };

	float m_near = 1.0f, m_far = 100.0f;

	// Eular Angles
	float m_yaw{ -90.0f };
	float m_pitch{ 0.0f };

	float m_speed{ 5.0f };
	const float m_sensitivity{ 0.3f };
	const float m_FOV{ 70.0f };

	// Mouse positions
	bool m_firstMouse{ true };
	double m_prevX{ 0.0 }, m_prevY{0.0};

	// Should we update the camera attributes?
	bool m_dirty{ true };
};
