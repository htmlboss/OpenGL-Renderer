#pragma once

#include <functional>
#include <array>

#include "GLRenderer.h"

class Input {
	Input() = default;
	~Input() = default;
public:

	static Input* GetInstance() {
		static Input instance;
		return &instance;
	}

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	bool IsKeyPressed(const std::size_t key) const { return m_keys[key]; }

	// Generic Input Callbacks
	// Mouse moved
	std::function<void(double, double)> mouseMoved = [this](auto xPos, auto yPos)
	{
		GLRenderer::UpdateCameraView(xPos, yPos);
	};

	// Key Pressed
	std::function<void(int, int, int, int)> keyPressed = [this](auto key, auto scancode, auto action, auto mode)
	{	
		if (key >=0 && key < 1024) {
			switch (action) {
			// Pressed
			case 1:
				this->m_keys[key] = true;
				break;
			// Released
			case 0:
				this->m_keys[key] = false;
				break;
			}
		}
	};

	// Window size changed
	std::function<void(int, int)> windowResized = [this](auto width, auto height)
	{
		GLRenderer::Resize(width, height);
	};
private:
	// Keyboard
	std::array<bool, 1024> m_keys;
};
