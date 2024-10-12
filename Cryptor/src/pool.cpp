#include <kitty/pool.h>

ThreadPool::ThreadPool(size_t numThreads = thread::hardware_concurrency())
{
	for (size_t i = 0; i < numThreads; ++i)
	{
		threads_.emplace_back([this] {
			while (true)
			{
				function<void()> task;

				// unlock queue
				{
					unique_lock<mutex> lock(queue_mutex_);

					cv_.wait(lock, [this] {
						return !tasks_.empty() || stop_;
					});

					if (stop_ && tasks_.empty())
						return;

					task = move(tasks_.front());
					tasks_.pop();
				}

				task();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	{
		// lock queue
		unique_lock<mutex> lock(queue_mutex_);
		stop_ = true;
	}

	cv_.notify_all();

	for (auto& thread : threads_)
		thread.join();
}

void ThreadPool::enqueue(function<void()> task)
{
	{
		unique_lock<std::mutex> lock(queue_mutex_);
		tasks_.emplace(move(task));
	}

	cv_.notify_one();
}