#include <semaphore.h>
#include <stddef.h>
#include <atomic>
#include <array>
#include <mutex>
#include "../spin_lock/spin_lock.h"


template <typename T, size_t CAPACITY>
class mpsc_lockfreeQueue
{

#define RECV_WAIT_FOREVER   (-1)
#define RECV_WAIT_TRY       (1)

static_assert(CAPACITY && !(CAPACITY & (CAPACITY - 1)),
    "CAPACITY must be power 2");


public:
    mpsc_lockfreeQueue()
        :m_head(0), m_tail(0), m_size(0), m_capacity(CAPACITY)
    {
        sem_init(&m_recvSem,0,0);
    }
    
    ~mpsc_lockfreeQueue()
    {
        size_t r = m_tail.load(std::memory_order_acquire);
        size_t w = m_head.load(std::memory_order_acquire);
        while (r != w)
        {
            reinterpret_cast<T*>(&m_data[r])->~T();
            r = (r + 1) & (CAPACITY - 1);
        }
        sem_destroy(&m_recvSem);
    }

    void move_data_in(T &&value)
    {
        const size_t w = m_tail.load(std::memory_order_acquire);
        const size_t next_w = (w + 1) & (m_capacity - 1);     // 2的次幂 优化取余操作
    
        new (&m_data[w]) T(std::forward<T>(value));
        m_tail.store(next_w, std::memory_order_release);
        return ;
    }

    void move_data_out(T &value) noexcept
    {
        const size_t r = m_head.load(std::memory_order_acquire);

        value = std::move(*reinterpret_cast<T*>(&m_data[r]));
        reinterpret_cast<T*>(&m_data[r])->~T();

        const size_t new_r = (r + 1) & (CAPACITY - 1);
        m_head.store(new_r, std::memory_order_release);

        return ;
    }

    bool send(T &&value)
    {
        std::unique_lock<SpinLock> lock(m_sendLock);

        size_t _size = m_size.load(std::memory_order_acquire);
        if (_size >= m_capacity)
            return false;
    
        move_data_in(std::forward<T>(value));
        m_size.fetch_add(1, std::memory_order_release);
        lock.unlock();
        
        if (_size == 0)
        {
            int sem_value = 0;
            sem_getvalue(&m_recvSem, &sem_value);
            if (0 == sem_value)
                sem_post(&m_recvSem);
        }

        return true;
    }

    bool recv(T &value, int waitType)
    {
        while (m_size.load(std::memory_order_acquire) == 0)
        {
            if (RECV_WAIT_FOREVER == waitType)
            {
                sem_wait(&m_recvSem);
            }
            else
            {
                return false;
            }
        }
        
        move_data_out(value);
        m_size.fetch_sub(1, std::memory_order_release);
        return true;
    }

private:
    
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
    std::atomic<size_t> m_size;
    size_t m_capacity;
    SpinLock m_sendLock;
    sem_t m_recvSem;
    std::array<T, CAPACITY> m_data;
};


