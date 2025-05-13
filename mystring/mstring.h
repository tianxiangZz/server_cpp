#ifndef __M_STRING_H__
#define __M_STRING_H__

#include <cstring>
#include <stdexcept>
#include <algorithm>

class string
{
    static const size_t min_capacity;
public:
    string()
    {
        m_size = 0;
        m_capacity = min_capacity;
        m_data = new char[m_capacity + 1];
        m_data[0] = '\0';
    }

    string(const char *str)
    {
        if (str == nullptr)
        {
            throw std::invalid_argument("nullptr ptr");
        }

        m_size = std::strlen(str);
        m_capacity = std::max(min_capacity, m_size);
        m_data = new char[m_capacity + 1];
        std::memcpy(m_data, str, m_size);
        m_data[m_size] = '\0';
    }
    
    string(const void *ptr, size_t len)
    {
        if (ptr == nullptr)
        {
            throw std::invalid_argument("nullptr ptr");
        }

        m_size = len;
        m_capacity = std::max(min_capacity, m_size);
        m_data = new char[m_capacity + 1];
        std::memcpy(m_data, ptr, m_size);
        m_data[m_size] = '\0';
    }

    string(const string &other)
        :m_size(other.m_size),
        m_capacity(other.m_capacity)
    {
        m_data = new char[m_capacity + 1];
        std::memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = '\0';
    }

    string(string &&other) noexcept
        :m_size(other.m_size),
        m_capacity(other.m_capacity),
        m_data(m_data)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    ~string()
    {
        if (m_data)
        {
            delete[] m_data;
        }
    }

    string& operator = (const string &other)
    {
        if (this != &other)
        {
            if (m_data)
                delete[] m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = new char[m_capacity + 1];
            std::memcpy(m_data, other.m_data, m_size);
            m_data[m_size] = '\0';
        }
        return *this;
    }

    string& operator = (string &&other) noexcept
    {
        if (this != &other)
        {
            if (m_data)
                delete[] m_data;

            m_size = other.m_size;
            other.m_size = 0;
            m_capacity = other.m_capacity;
            other.m_capacity = 0;
            m_data = other.m_data;
            other.m_data = nullptr;
        }
        return *this;
    }

    void reserve(size_t size)
    {
        if (size > m_capacity)
        {
            realloc(size);
        }
    }

    void shrink_to_fit()
    {
        if (m_capacity > m_size)
        {
            realloc(m_size);
        }
    }

    string append(const void *ptr, size_t len)
    {
        if (nullptr == ptr)
        {
            throw std::invalid_argument("nullptr ptr");
        }

        if (m_size + len > m_capacity)
        {
            reserve(m_size + len);
        }

        std::memcpy(m_data + m_size, ptr, len);
        m_size += len;
        m_data[m_size] = '\0';
        return *this;
    }

    string append(const char *str)
    {
        if (nullptr == str)
        {
            throw std::invalid_argument("nullptr ptr");
        }

        return append(str, strlen(str));
    }

    const char *c_str() const noexcept { return m_data; }
    const char *data() const noexcept { return m_data; }
    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }
    bool empty() const noexcept { return m_size == 0; }

private:
    bool realloc(size_t new_capacity)
    {
        new_capacity = std::max(new_capacity, min_capacity);
        char *new_data = new char[new_capacity + 1];
        if (m_size > 0)
        {
            std::memcpy(new_data, m_data, m_size);
        }
        new_data[m_size] = '\0';
        delete[] m_data;

        m_data = new_data;
        m_capacity = new_capacity;
        return true;
    }

private:
    char *m_data;
    size_t m_size;
    size_t m_capacity;
};


const size_t string::min_capacity = 15;


#endif /** __M_STRING_H__ */