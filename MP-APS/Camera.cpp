#include "Camera.h"

/***********************************************************************************/
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float FOV) : 
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)), 
	m_FOV(FOV),
	m_position(position),
	m_yaw(yaw),
	m_pitch(pitch) {

	updateVectors();
}

/***********************************************************************************/
void Camera::ProcessKeyboard(const Camera_Movement direction, const double deltaTime) {
	const float velocity = m_speed * static_cast<float>(deltaTime);

	switch (direction) {
	case FORWARD:
		m_position += m_front * velocity;
		break;
	case BACKWARD:
		m_position -= m_front * velocity;
		break;
	case LEFT:
		m_position -= m_right * velocity;
		break;
	case RIGHT:
		m_position += m_right * velocity;
		break;
	case UP:
		m_position += m_worldUp * velocity;
		break;
	case DOWN:
		m_position -= m_worldUp * velocity;
		break;
	}
}

/***********************************************************************************/
void Camera::ProcessMouseMovement(double xoffset, double yoffset, const bool constrainPitch /*= true*/) {
	xoffset *= m_sensitivity;
	yoffset *= m_sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (m_pitch > 89.0f) {
			m_pitch = 89.0f;
		}
		if (m_pitch < -89.0f) {
			m_pitch = -89.0f;
		}
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	updateVectors();
}

/***********************************************************************************/
void Camera::updateVectors() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	
	m_front = glm::normalize(front);
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
