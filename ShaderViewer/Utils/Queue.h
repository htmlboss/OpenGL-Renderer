/*
* A thread-safe queueueueueueue.
* Provides a wrapper around a basic queue to provide thread safety.
* Adapted from http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
*/
#pragma once

#include <atomic>
#include <mutex>
#include <queue>

namespace Utils {

template<typename T>	
class Queue {

public:
	Queue() = default;
	~Queue() { Invalidate(); }

	/*
	* Attempt to get the first value in the queue.
	* Returns true if a value was successfully written to the out parameter, false otherwise.
	*/
	bool TryPop(T& output) {
		std::lock_guard<std::mutex> lock{ m_mutex };
		
		if (m_queue.empty() || !m_valid) {
			return false;
		}
		output = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	/*
	* Get the first value in the queue.
	* Will block until a value is available unless clear is called or the instance is destructed.
	* Returns true if a value was successfully written to the out parameter, false otherwise.
	*/
	bool WaitPop(T& output) {
		std::unique_lock<std::mutex> lock{ m_mutex };

		m_condition.wait(lock, [this]() { return !m_queue.empty() || !m_valid; });
			if (!m_valid) {
			return false;
		}

		output = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	/*
	* Push a new value onto the queue.
	*/
	void Push(const T& value) {
		std::lock_guard<std::mutex> lock{ m_mutex };

		m_queue.push(std::move(value));
		m_condition.notify_one();
	}

	/*
	* Check whether or not the queue is empty.
	*/
	bool IsEmpty() {
		std::lock_guard<std::mutex> lock{ m_mutex };

		return m_queue.empty();
	}

	/*
	* Empty queue.
	*/
	void Clear() {
		std::lock_guard<std::mutex> lock{ m_mutex };

		while (!m_queue.empty()) {
			m_queue.pop();
		}
		m_condition.notify_all();
	}

	/*
	* Invalidate the queue.
	* Used to ensure no conditions are being waited on in waitPop when
	* a thread or the application is trying to exit.
	* The queue is invalid after calling this method and it is an error
	* to continue using a queue after this method has been called.
	*/
	void Invalidate() {
		std::lock_guard<std::mutex> lock{ m_mutex };
		
		m_valid = false;
		m_condition.notify_all();
	}

	/*
	* Returns whether or not this queue is valid.
	*/
	bool IsValid() const {
		std::lock_guard<std::mutex> lock{ m_mutex };
		
		return m_valid;
	}

private:
	std::atomic_bool m_valid{ true };
	mutable std::mutex m_mutex;
	std::queue<T> m_queue;
	std::condition_variable m_condition;

}; // Queue

}// namespace Utils
