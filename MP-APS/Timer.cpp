#include "Timer.h"

#include <iostream>

/***********************************************************************************/
Timer::Timer() noexcept : m_delta(0.0), m_lastFrame(0.0), m_lastTime(0.0), m_nbFrames(0) {
}

/***********************************************************************************/
void Timer::Update(const double time) noexcept {
	const auto currentFrame = time;
	++m_nbFrames;

	// Calculates frame time (in milliseconds)
	if (currentFrame - m_lastTime >= 1.0) {
		const auto frameTime = 1000.0 / static_cast<double>(m_nbFrames);

		std::cout << frameTime << " ms - " << 1.0 / (frameTime / 1000.0) << " fps" << '\n';

		m_nbFrames = 0;
		m_lastTime += 1.0;
	}

	m_delta = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
}
