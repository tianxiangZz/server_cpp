#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

// SPSC

#include <type_traits>
#include <atomic>
#include <stddef.h>
#include <new>

template <typename T, size_t CAPACITY>
class Ringbuffer
{
#define _ALIGNAS    alignas(64)

static_assert(CAPACITY && !(CAPACITY & (CAPACITY - 1)),
    "CAPACITY must be power 2");

public:
    Ringbuffer(): m_read(0), m_write(0) {}

    ~Ringbuffer()
    {
        size_t r = m_read.load(std::memory_order_acquire);
        size_t w = m_write.load(std::memory_order_acquire);
        while (r != w)
        {
            reinterpret_cast<T*>(&m_data[r])->~T();
            r = (r + 1) & (CAPACITY - 1);
        }
    }

    template<typename U>
    bool Push(U &&val) noexcept 
    {
        const size_t w = m_write.load(std::memory_order_acquire);
        const size_t next_w = (w + 1) & (CAPACITY - 1);     // 2的次幂 优化取余操作
    
        if (next_w == m_read.load(std::memory_order_acquire))
            return false;
        
        new (&m_data[w]) T(std::forward<U>(val));
        m_write.store(next_w, std::memory_order_release);
        return true;
    }

    bool Pop(T &val) noexcept
    {
        const size_t r = m_read.load(std::memory_order_acquire);

        if (r == m_write.load(std::memory_order_acquire))
        {
            return false;
        }

        val = std::move(*reinterpret_cast<T*>(&m_data[r]));
        reinterpret_cast<T*>(&m_data[r])->~T();

        const size_t new_r = (r + 1) & (CAPACITY - 1);
        m_read.store(new_r, std::memory_order_release);

        return true;
    }

private:
    _ALIGNAS std::atomic<size_t> m_read;
    _ALIGNAS std::atomic<size_t> m_write;
    _ALIGNAS typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data[CAPACITY];
};





#endif /** __RING_BUFFER_H__ */