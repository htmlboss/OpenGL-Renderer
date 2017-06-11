#pragma once

#include <functional>
#include <array>

#ifdef _DEBUG
#include <cassert>
#endif

class Input {
	Input() = default;
	~Input() = default;
public:

	static auto& GetInstance() {
		static Input instance;
		return instance;
	}

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	void Update() noexcept {
		m_mouseMoved = false;
		m_shouldResize = false;
	}

	// Getters
	// Keyboard
	auto IsKeyPressed(const size_t key) const noexcept {
#ifdef _DEBUG
		assert(key < 1024);
#endif
		return m_keys[key];
	}

	// Mouse
	auto MouseMoved() const noexcept { return m_mouseMoved; }
	auto GetMouseX() const noexcept { return m_xPos; }
	auto GetMouseY() const noexcept { return m_yPos; }

	// Window
	auto ShouldResize() const noexcept { return m_shouldResize; }
	auto GetWidth() const noexcept { return m_width; }
	auto GetHeight() const noexcept { return m_height; }

	// Generic Input Callbacks
	// Mouse moved
	std::function<void(double, double)> mouseMoved = [this](auto xPos, auto yPos) {
		this->m_mouseMoved = true;
		this->m_xPos = xPos;
		this->m_yPos = yPos;
	};

	// Key Pressed
	std::function<void(int, int, int, int)> keyPressed = [this](auto key, auto scancode, auto action, auto mode) {
		if (key >= 0 && key < 1024) {
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
	std::function<void(int, int)> windowResized = [this](auto width, auto height) {
		this->m_shouldResize = true;
		this->m_width = width;
		this->m_height = height;
	};

private:
	// Keyboard
	std::array<bool, 1024> m_keys;

	// Mouse
	bool m_mouseMoved = false;
	double m_xPos, m_yPos;

	// Resize
	bool m_shouldResize = false;
	size_t m_width, m_height;
};
