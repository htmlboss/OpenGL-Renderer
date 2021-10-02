#pragma once

#include <functional>

class Timer {
	using TimerCallbackFunc = std::function<void()>;
public:
	
	Timer(const double secondsBetweenCallbackTriggers, TimerCallbackFunc callback) 
		: m_secondsBetweenCallbackTriggers{ secondsBetweenCallbackTriggers },
		  m_callbackFunc{ callback } {}

	void Update(const double time) {
		if (time - m_prevTriggerTime >= m_secondsBetweenCallbackTriggers) {
			m_callbackFunc();

			m_prevTriggerTime += m_secondsBetweenCallbackTriggers;
		}

		m_delta = time - m_prevTime;
		m_prevTime = time;
	}

	auto GetDelta() const noexcept {
		return m_delta;
	}

private:
	double m_prevTriggerTime{ 0.0 }, m_prevTime{ 0.0 }, m_delta{ 0.0 };
	const double m_secondsBetweenCallbackTriggers;
	const TimerCallbackFunc m_callbackFunc;
};
