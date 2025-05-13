#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <functional>
#include <chrono>
#include <map>

namespace mylib
{

class Timer;

class TimerTask
{
    friend class Timer;
    using Callback = std::function<void(TimerTask *task)>;

public:
    TimerTask(uint64_t addTime, uint64_t execTime, Callback cb)
        :m_addTime(addTime), 
        m_execTime(execTime)
    {
        m_cb = std::move(cb);
    }

    uint64_t AddTime() const {
        return m_addTime;
    }
    uint64_t ExecTime() const {
        return m_execTime;
    }


private:
    void run()
    {
        m_cb(this);
    }

    uint64_t m_addTime;         /** 添加任务的时间 */
    uint64_t m_execTime;        /** 执行任务的时间 */
    Callback    m_cb;
};


class Timer
{
    using Milliseconds = std::chrono::milliseconds;
public:
    static uint64_t GetTick()
    {
        auto sc = std::chrono::time_point_cast<Milliseconds>(std::chrono::steady_clock::now());
        auto tmp = std::chrono::duration_cast<Milliseconds>(sc.time_since_epoch());
        return tmp.count();
    }

    TimerTask *AddTimeout(uint64_t offset, TimerTask::Callback cb)
    {
        auto nowTime = GetTick();
        auto execTime = offset + nowTime;
        auto timerTask = new TimerTask(nowTime, execTime, std::move(cb));

        // 性能优化  大量时间相似节点
        // 插入到最后
        if (m_timerManager.empty() || execTime >= m_timerManager.crbegin()->first)
        {
            auto ele = m_timerManager.emplace_hint(m_timerManager.end(), execTime, timerTask);
            return ele->second;
        }


        auto ele = m_timerManager.emplace(execTime, timerTask);
        return ele->second;
    }

    void DelTimeout(TimerTask *task)
    {
        /** 返回迭代器范围 */
        auto range = m_timerManager.equal_range(task->ExecTime());
        for (auto iter = range.first; iter != range.second;)
        {
            if (iter->second == task)
            {
                iter = m_timerManager.erase(iter);
            }
            else
            {
                ++ iter;
            }
        }
    }

    void Update(uint64_t now)
    {
        auto nowTime = GetTick();
        auto iter = m_timerManager.begin();
        while (iter != m_timerManager.end() && iter->first <= nowTime)
        {
            iter->second->run();
            delete iter->second;
            iter = m_timerManager.erase(iter);
        }
    }

    int waitTime()
    {
        auto iter = m_timerManager.begin();
        if (iter == m_timerManager.end())
            return -1;
        int diss = iter->first - GetTick();
        return diss > 0 ? diss : 0;
    }

private:
    std::multimap<uint64_t, TimerTask*> m_timerManager;
};

#if 0

int main ()
{
    Timer timer;
    int n = epoll_wait(epfd, ev, evSize, timer.waitTime());
    timer.Update();
    return 0;
}

#endif


} // namespace mylib

#endif /** __TIMER_H__ */