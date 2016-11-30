#include "Timer.h"
#include <iostream>

/***********************************************************************************/
Time::Time() : m_lastFrame(0.0f), m_lastTime(0.0f), m_nbFrames(0) {
}

/***********************************************************************************/
Time::~Time() {
}

/***********************************************************************************/
void Time::Update(const double time) {
	double currentFrame = time;
	m_nbFrames++;

	// Calculates frame time (in ms)
	if (currentFrame - m_lastTime >= 1.0f) {
		std::cout << 1000.0f / static_cast<double>(m_nbFrames) << " ms" << std::endl;
		m_nbFrames = 0;
		m_lastTime += 1.0f;
	}

	m_delta = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
}
