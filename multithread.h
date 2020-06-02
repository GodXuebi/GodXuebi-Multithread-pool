#ifndef __MUTITHREAD_H__
#define __MUTITHREAD_H__
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "multithread.h"
class fixed_thread_pool
{
private:
	int poolsize_;
	struct poolstatus
	{
		std::mutex mt_;
		std::condition_variable cv_;
		bool isfin_;
		std::queue<std::function<void()>> tasks_;
	};
	std::shared_ptr<poolstatus> sharedp_;

public:
	fixed_thread_pool()=default;
	fixed_thread_pool(fixed_thread_pool&&)=default;
	
	explicit fixed_thread_pool(size_t poolsize):poolsize_(poolsize),sharedp_(std::make_shared<poolstatus>())
	{
		for(int i=0;i<poolsize;i++)
		{
			std::thread(
//--------------------------------------------------------------------
				[sharedp=sharedp_]{
				std::unique_lock<std::mutex> lk(sharedp->mt_);
				for(;;)
				{
					if(!sharedp->tasks_.empty())
					{
						auto curtask=std::move(sharedp->tasks_.front());
						sharedp->tasks_.pop();
						lk.unlock();
						curtask();
						lk.lock();
					}
					else if(sharedp->isfin_)
					{ break;}
					else
					{ sharedp->cv_.wait(lk);}
				}
		}
//--------------------------------------------------------------------
			).detach();
		}
	}

	~fixed_thread_pool()
	{
		if((bool) sharedp_)
		{
			std::lock_guard<std::mutex> lk(sharedp_->mt_);
			sharedp_->isfin_=true;
		}
		sharedp_->cv_.notify_all();
	}


	template<class F>
	void addtask(F&&f)
	{
		{
			std::lock_guard<std::mutex> lk(sharedp_->mt_);	
			sharedp_->tasks_.emplace(std::forward<F>(f));
		}
		sharedp_->cv_.notify_one();

	}
};
#endif
