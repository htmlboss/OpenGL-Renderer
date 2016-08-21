#pragma once

#include <windows.h>
#include <stdio.h>

class Timer {
public:
	Timer();
	~Timer();

	bool Init();
	void Update();
	float GetTime();

	float GetElapsedTime() const { return m_diffTime; }
	float GetFPS() const { return m_FPS; }

private:
	__int64 m_perfTimerStart;
	__int64 m_perfTimerElapsed;

	// General timing stuff
	__int64 m_frequency;
	float m_resolution;
	bool m_perfTimer;

	// FPS
	float m_time1;
	float m_time2;
	float m_diffTime;
	float m_FPS;
	int m_framesElapsed;
};

