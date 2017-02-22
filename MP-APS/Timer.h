#pragma once

class Timer {
public:
	Timer();
	~Timer();

	void Update(const double time);
	double GetDelta() const { return m_delta; }

private:
	double m_delta;
	double m_lastFrame;
	double m_lastTime;
	int m_nbFrames;
};

