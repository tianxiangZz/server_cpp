#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <stdio.h>
#include <atomic>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>

#include "Noncopyable.h"

namespace mylib
{
class ThreadPool : netlib::Noncopyable
{

public:
    using Task = std::function<void()>;


    explicit ThreadPool(const std::string &name = "")
        :m_name(name),
        m_maxTaskNum(16),
        m_running(false)
    { }

    ~ThreadPool() 
    {
        if (m_running)
        {
            stop();
        }
    }

    size_t start(size_t threadNum)
    {
        if (m_running || !m_threads.empty())
        {
            return 0;
        }
        if (threadNum < 1)
        {
            return 0;
        }

        m_running = true;
        m_threads.reserve(threadNum);

        for (size_t i = 0; i < threadNum; ++ i)
        {
            m_threads.push_back(
                std::thread(&ThreadPool::threadMainFunc, this)
            );
        }
        return threadNum;
    }

    void stop()
    {
        {        
            std::lock_guard<std::mutex> lock(m_mutex);
            m_running = false;
            m_hasTask.notify_all();
        }

        for (auto &thread : m_threads)
        {
            thread.join();
        }
    }
    
    void addTask(const Task &task)
    {
        if (m_threads.empty())
        {
            task();
        }
        else
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (overLoaded())
            {
                m_acceptNewTask.wait(lock);
            }
            m_tasks.push_back(task);
            m_hasTask.notify_one();
        }
    }

    void setMaxTaskNum(size_t num) { m_maxTaskNum = num; }



    /** base */
    size_t getMaxTaskNum() const { return m_maxTaskNum; }
    size_t getThreadNum() const { return m_threads.size(); }
    const std::string &getName() const { return m_name; }
    size_t getCurTaskNum() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

private:
    /** 超负载了, 此函数在临界区调用  保护m_tasks  */
    bool overLoaded() const 
    {
        // std::lock_guard<std::mutex> lock(m_mutex);
        return (m_maxTaskNum > 0 && m_tasks.size() > m_maxTaskNum);
    }

    void threadMainFunc()
    {
        while (m_running)
        {
            Task task = ScheduleTask();
            if (task)
                task();
        }
    }

    /** 调度任务 */
    Task ScheduleTask()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        while (m_tasks.empty() && m_running)
        {
            m_hasTask.wait(lock);
        }

        Task _task;
        if (!m_tasks.empty())
        {
            _task = m_tasks.front();
            m_tasks.pop_front();

            if (m_tasks.size() < m_maxTaskNum)
            {
                m_acceptNewTask.notify_one();
            }
        }
        return _task;
    }

private:
    std::string     m_name;
    mutable std::mutex      m_mutex;
    std::condition_variable m_hasTask;
    std::condition_variable m_acceptNewTask;
    std::vector<std::thread> m_threads;
    std::deque<Task> m_tasks;
    size_t m_maxTaskNum;
    std::atomic_bool m_running;
};


} // namespace mylib


#endif /** __THREAD_POOL_H__ */