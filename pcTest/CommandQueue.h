#ifndef __COMMANDQUEUE__H
#define __COMMANDQUEUE__H

#include <condition_variable>
#include <mutex>
#include <memory>
#include <deque>

class CommandQueue
{
public:
	CommandQueue() {}
	~CommandQueue() {}
public:
	static CommandQueue& instance()
	{
		if (!instance_)
		{
			std::unique_lock<std::mutex> lock(mtx_);
			if (!instance_)
				instance_ = new CommandQueue;
		}
		return *instance_;
	}
private:
	static CommandQueue* instance_;
	static std::mutex mtx_;
private:
	std::condition_variable condition_;
	std::mutex mutex_;
	std::deque<int> container_;
public:
	bool empty()
	{
		return container_.empty();
	}
	
	void pop(int& obj)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (container_.empty())
			condition_.wait(lock);
		int o = container_.front();
		container_.pop_front();
		obj = o;
	}

	void push(const int& obj)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		container_.push_back(obj);
		condition_.notify_one();
	}
	//
};

#endif//__COMMANDQUEUE__H
