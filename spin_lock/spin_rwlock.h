#ifndef __SPIN_RW_LOCK_H__
#define __SPIN_RW_LOCK_H__

#include <unistd.h>
#include <sched.h>
#include <atomic>

class RWSpinLock
{

public:
    RWSpinLock() :readers(0), writer(0) {}
    ~RWSpinLock() {}

    void read_lock()
    {
        for (;;)
        {
            // 存在写者 自选等待
            while (writer.load(std::memory_order_acquire))
                usleep(0);
        
            readers.fetch_add(1, std::memory_order_release);

            // 此时存在写者  读者退让
            if (writer.load(std::memory_order_acquire))
                readers.fetch_sub(1, std::memory_order_release);
        }
    }

    void read_unlock()
    {
        readers.fetch_sub(1, std::memory_order_release);
    }

    void write_lock()
    {
        for (;;)
        {
            bool flag = false;
            if (writer.compare_exchange_weak(flag, true, std::memory_order_acq_rel))
                break;
            usleep(0);      // 等待已有的写者退出
        }

        // 等待读者退出完毕
        while (readers.load(std::memory_order_acquire))
            usleep(0);
    }

    void write_unlock() {
        writer.store(0, std::memory_order_release);  // 直接重置为0
    }

private:
    std::atomic_uint32_t readers;
    std::atomic_bool writer;
};





# endif /** __SPIN_RW_LOCK_H__ */