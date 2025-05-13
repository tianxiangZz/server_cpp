#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

#include <atomic>


template <typename T>
class shared_ptr
{
public:
    shared_ptr() : m_ptr(nullptr), m_ref_count(nullptr) {}

    explicit shared_ptr(T *ptr = nullptr)
        :m_ptr(ptr)
    {
        m_ref_count = (nullptr == ptr) ? 
                    nullptr :
                    new std::atomic<std::size_t>(1);
    }

    ~shared_ptr()
    {
        if (m_ptr)
        {
            release();
        }
    }

    // 拷贝构造
    shared_ptr(const shared_ptr<T> &other)
        :m_ptr(other.m_ptr),
        m_ref_count(other.m_ref_count)
    {
        if (nullptr != m_ptr && nullptr != m_ref_count)
        {
            m_ref_count->fetch_add(1, std::memory_order_relaxed);
        }
    }

    shared_ptr<T>& operator = (const shared_ptr<T> &other)
    {
        if (this != &other)
        {
            release();   // 释放自己
            m_ptr = other.m_ptr;
            m_ref_count = other.m_ref_count;
            if (nullptr != m_ptr && nullptr != m_ref_count)
            {
                m_ref_count->fetch_add(1, std::memory_order_relaxed);
            } 
        }
        return *this;
    }

    // 移动构造
    shared_ptr(shared_ptr<T> &&other) noexcept
        :m_ptr(other.m_ptr),
        m_ref_count(other.m_ref_count)
    {
        other.m_ptr = nullptr;
        other.m_ref_count = nullptr;
    }

    shared_ptr<T>& operator = (shared_ptr<T> &&other) noexcept
    {
        if (this != &other)
        {
            release();
            m_ptr = other.m_ptr; other.m_ptr = nullptr;
            m_ref_count = other.m_ref_count; other.m_ref_count = nullptr;
        }
        return *this;
    }


    T& operator * () const { return *m_ptr; }
    T* operator -> () const { return m_ptr; }
    T* get() const { return m_ptr; }
    void reset(T *ptr = nullptr)
    {
        release();
        m_ptr = ptr;
        m_ref_count = (m_ptr == nullptr) ?
            nullptr : 
            new std::atomic<std::size_t>(1);
    }

    std::size_t use_count() const
    {
        if (nullptr == m_ref_count)
            return 0; 
        return m_ref_count->load(std::memory_order_acquire); 
    }

private:
    void release()
    {
        if (nullptr != m_ref_count)
        {
            if (1 == m_ref_count->fetch_sub(1, std::memory_order_acq_rel))
            {
                delete m_ptr; m_ptr = nullptr;
                delete m_ref_count; m_ref_count = nullptr;
            }
        }
    }

private:
    T *m_ptr;
    std::atomic<std::size_t> *m_ref_count;
};





#endif /** __SHARED_PTR_H__ */