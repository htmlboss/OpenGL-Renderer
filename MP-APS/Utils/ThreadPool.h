/*
* ThreadPool class.
* Keeps a set of threads constantly waiting to execute incoming jobs.
* Adapted from: http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
*/
#pragma once

#include "Queue.h"

#include <algorithm>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>


namespace Utils {

// ThreadPool singleton class
class ThreadPool {
	
	class IThreadTask {
	
	public:
		IThreadTask() = default;
		virtual ~IThreadTask() = default;
		
		IThreadTask(const IThreadTask& rhs) = delete;
		IThreadTask& operator=(const IThreadTask& rhs) = delete;
		IThreadTask(IThreadTask&& other) = default;
		IThreadTask& operator=(IThreadTask&& other) = default;

		// Run the task.
		virtual void Execute() = 0;

	}; // IThreadTask

	template<typename Func>
	class ThreadTask : public IThreadTask {

	public:
		ThreadTask(Func&& func) : m_func(std::move(func)) { }
		~ThreadTask() override = default;
		
		ThreadTask(const ThreadTask& rhs) = delete;
		ThreadTask& operator=(const ThreadTask& rhs) = delete;
		ThreadTask(ThreadTask&& other) = default;
		ThreadTask& operator=(ThreadTask&& other) = default;

		// Run the task.
		void Execute() override { m_func(); }

	private:
		Func m_func;

	}; // ThreadTask

public:

	ThreadPool() : ThreadPool { std::max(std::thread::hardware_concurrency(), 2u) - 1u } {
		/*
		* Always create at least one thread.  If hardware_concurrency() returns 0,
		* subtracting one would turn it to UINT_MAX, so get the maximum of
		* hardware_concurrency() and 2 before subtracting 1.
		*/
	}
	explicit ThreadPool(const std::uint32_t numThreads) : m_done{false}, m_workQueue{}, m_threads{} {
		try {
			for (std::uint32_t i = 0u; i < numThreads; ++i) {
				m_threads.emplace_back(&ThreadPool::worker, this);
			}
		}
		catch (...) {
			destroy();
			throw;
		}
	}
	~ThreadPool() { destroy(); }
	
	ThreadPool(const ThreadPool& rhs) = delete;
	ThreadPool& operator=(const ThreadPool& rhs) = delete;

	// Submit a job to be run by the pool
	template<typename Func, typename... Args>
	auto Submit(Func&& func, Args&&... args) {
		auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
		using ResultType = std::result_of_t<decltype(boundTask)()>;
		using PackagedTask = std::packaged_task<ResultType()>;
		using TaskType = ThreadTask<PackagedTask>;

		PackagedTask task { std::move(boundTask) };
		TaskFuture<ResultType> result { task.get_future() };
		
		m_workQueue.Push(std::make_unique<TaskType>(std::move(task)));
		
		return result;
	}


	/*
	* A wrapper around a std::future that adds the behavior of futures returned from std::async.
	* Specifically, this object will block and wait for execution to finish before going out of scope.
	*/
	template<typename T>
	class TaskFuture {

	public:
		TaskFuture(std::future<T>&& future) : m_future(std::move(future)) { }
		~TaskFuture() { if (m_future.valid()) m_future.get(); }

		TaskFuture(const TaskFuture& rhs) = delete;
		TaskFuture& operator=(const TaskFuture& rhs) = delete;
		TaskFuture(TaskFuture&& other) = default;
		TaskFuture& operator=(TaskFuture&& other) = default;

		auto Get() const { return m_future.get(); }

	private:
		std::future<T> m_future;
		
	}; // TaskFuture

private:

	// Constantly running function each thread uses to acquire work items from the queue.
	void worker() {
		while (!m_done) {
			std::unique_ptr<IThreadTask> pTask{ nullptr };
			if (m_workQueue.WaitPop(pTask)) {
				pTask->Execute();
			}
		}
	}

	// Invalidates the queue and joins all running threads.
	void destroy() {
		m_done = true;
		m_workQueue.Invalidate();
		
		for (auto& thread : m_threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

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
