#include "Timer.h"
#include <math.h>

/***********************************************************************************/
Timer::Timer() : m_FPS(0.0f), m_time1(0.0f), m_time2(0.0f), m_diffTime(0.0f), m_framesElapsed(0) {
}


/***********************************************************************************/
Timer::~Timer() {
}


/***********************************************************************************/
// Select an approproate timer
bool Timer::Init() {
	// check to see if we are going to be using the performance counter
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency)) {
		
		m_perfTimer = true;

		QueryPerformanceCounter((LARGE_INTEGER*)&m_perfTimerStart);

		m_resolution = static_cast<float>(static_cast<double>(1.0f) / static_cast<double>(m_frequency));

		m_perfTimerElapsed = m_perfTimerStart;
	}

	return m_perfTimer;
}

/***********************************************************************************/
// Perform calculations (update timer)
void Timer::Update() {
	m_framesElapsed++;

	if (m_framesElapsed % 10 == 1) {
		m_time1 = GetTime() / 1000;
	}

	else if (m_framesElapsed % 10 == 0) {
		m_time1 = m_time2;
		m_time2 = GetTime() / 1000;
		m_diffTime = static_cast<float>(fabs(m_time2 - m_time1));
	}

	m_FPS = 10 / m_diffTime;
}

/***********************************************************************************/
// Return the time since the Init() function was called
float Timer::GetTime() {
	
	__int64 Time;
	QueryPerformanceCounter((LARGE_INTEGER*)&Time);

	return static_cast<float>((Time - m_perfTimerStart) * m_resolution) * 1000.0f;
}

