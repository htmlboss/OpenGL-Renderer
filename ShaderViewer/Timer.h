#pragma once

class Time {
public:
	Time();
	~Time();

	void Update(const double time);
	double GetDelta() const { return m_delta; }

private:
	double m_delta;
	double m_lastFrame;
	double m_lastTime;
	int m_nbFrames;
};

