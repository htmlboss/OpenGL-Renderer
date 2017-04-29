#pragma once
#include <cstdint>

class Timer {
public:
	Timer() noexcept;
	~Timer() = default;

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	void Update(const double time);
	double GetDelta() const { return m_delta; }

private:
	double m_delta, m_lastFrame, m_lastTime;
	uint32_t m_nbFrames;
};

