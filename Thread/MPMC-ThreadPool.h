#ifndef __MPMC_THREAD_POOL_H__
#define __MPMC_THREAD_POOL_H__

#include <stdio.h>
#include <atomic>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>
#include <thread>
#include <functional>

#include "Noncopyable.h"

namespace mylib
{

using Task = std::function<void()>;

class BlockingQueue
{

public:
    BlockingQueue(bool nonblock = false)
        :m_nonblock(nonblock)
    {}

    void Push(const Task &_task)
    {
        std::lock_guard<std::mutex> lock(m_producer_mutex);
        m_producer_queue.push(_task);
        m_not_empty.notify_one();
    }

    bool Pop(Task &_task)
    {
        std::unique_lock<std::mutex> lock(m_consumer_mutex);
        if (m_consumer_queue.empty() && 0 != swapQueue())
        {
            return false;
        }

        _task = m_consumer_queue.front();
        m_consumer_queue.pop();
        return true;
    }

    void Cancel()
    {
        std::lock_guard<std::mutex> lock(m_producer_mutex);
        m_nonblock = true;
        m_not_empty.notify_all();
    }


private:
    size_t swapQueue()
    {
        std::unique_lock<std::mutex> lock(m_producer_mutex);
        m_not_empty.wait(lock, [this] { return !m_producer_queue.empty() || m_nonblock; });
        std::swap(m_producer_queue, m_consumer_queue);
        return m_consumer_queue.size();
    }


private:

    bool m_nonblock;

    std::queue<Task> m_consumer_queue;
    std::mutex m_consumer_mutex;

    std::queue<Task> m_producer_queue;
    std::mutex m_producer_mutex;

    std::condition_variable m_not_empty;
};



class MpmcThreadPool : netlib::Noncopyable
{

public:
    MpmcThreadPool(size_t threadnum)
    {
        for (size_t i = 0; i < threadnum; ++ i)
        {
            m_threads.emplace_back([this] {
                worker();
            });
        }
    }
    ~MpmcThreadPool()
    {
        m_tasks.Cancel();
        for (auto &thread : m_threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }  

    void Post(const Task &_task)
    {
        m_tasks.Push(_task);
    }

private:
    void worker()
    {
        while (true)
        {
            Task _task;
            if (!m_tasks.Pop(_task))
            {
                break;
            }
            _task();
        }
        
    }

private:
    BlockingQueue       m_tasks;
    std::vector<std::thread> m_threads;
};


} // namespace mylib


#endif /** __MPMC_THREAD_POOL_H__ */