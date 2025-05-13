#ifndef __SPIN_LOCK_H__
#define __SPIN_LOCK_H__

#include <sched.h>
#include <atomic>

class SpinLock
{

public:
    SpinLock() :m_lock(false) {}
    ~SpinLock() {}

    void lock()
    {
        bool expected = false;
        while (!m_lock.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            // sched_yield();
            expected = false;
        }
    }
    
    void unlock()
    {
        m_lock.store(false, std::memory_order_release);
    }

    bool isLock() const
    {
        return m_lock.load(std::memory_order_acquire);
    }

private:
    std::atomic_bool m_lock;
};





# endif /** __SPIN_LOCK_H__ */