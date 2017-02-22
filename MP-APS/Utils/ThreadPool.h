/*
* ThreadPool class.
* Keeps a set of threads constantly waiting to execute incoming jobs.
* Adapted from: http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
*/
#pragma once

#include "Queue.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>


namespace Utils {

// ThreadPool singleton class
class ThreadPool {
	
	class IThreadTask {
		
	}; // IThreadTask

	template<typename Func>
	class ThreadTask : public IThreadTask {
		
	}; // ThreadTask

public:

	ThreadPool();
	explicit ThreadPool(const std::uint32_t numThreads);
	
	ThreadPool(const ThreadPool& rhs) = delete;
	ThreadPool& operator=(const ThreadPool& rhs) = delete;
	
	~ThreadPool() { destroy(); }

	template<typename Func, typename... Args>
	auto Submit(Func&& func, Args&&... args);


	template<typename T>
	class TaskFuture {
		
	}; // TaskFuture

private:
	void worker();

	void destroy();

	std::atomic_bool m_done;
	std::vector<std::thread> m_threads;
	Queue<std::unique_ptr<IThreadTask>> m_workQueue;

}; // ThreadPool

	namespace DefThreadPool {
		
		template<typename Func, typename... Args>
		inline auto SubmitJob(Func&& func, Args&&... args) {
			
		}

		// Get an instance handle
		inline ThreadPool& GetThreadPool() {
			static ThreadPool defPool;
			return defPool;
		}

	} // namespace DefThreadPool


} // namespace Utils
