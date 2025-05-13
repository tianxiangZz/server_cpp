
#include <stddef.h>
#include <assert.h>
#include <algorithm>

template <typename T>
class Vector
{
public:
    typedef T value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

public:
    Vector() 
        :_start(nullptr), _finish(nullptr), _end_of_storage(nullptr) 
        {}

    Vector(size_t size, const T& _val = T())
        :_start(new T[size]),
        _finish(_start + size),
        _end_of_storage(_finish)
    {
        for (size_t i = 0; i < size; ++ i)
            *(_start + i) = _val;
    }

    template <typename inputIter>
    Vector(inputIter begin, inputIter end)
    {
        size_t size = end - begin;
        _start = new T[size];
        _finish = _end_of_storage = _start + size;

        for (size_t i = 0; i < size; ++ i)
        {
            *(_start + i) = *(begin + i);
        }
    }

    Vector(const Vector<T> &other)
        :_start(new T[other.capacity()]),
        _finish(_start + other.size()),
        _end_of_storage(_start + other.capacity())
    {
        iterator iter = other.begin();
        for (size_t i = 0; i < other.size(); ++ i)
            *(_start + i) = *(iter + i);
    }

    Vector(Vector<T> &&other) noexcept
        :_start(other._start),
        _finish(other._finish),
        _end_of_storage(other._end_of_storage)
    {
        other._start = other._finish = other._end_of_storage = nullptr;
    }

    ~Vector() 
    {
        delete[] _start;
        _start = _finish = _end_of_storage = nullptr;
    }

    void push_back(const T &_val)
    {
        if (_finish == _end_of_storage)
        {
            size_t _size = capacity() == 0 ? 2 : (capacity() * 2);
            reserve(_size);
        }
        *_finish = _val;
        ++ _finish;
    }

    void pop_back()
    {
        assert(!empty());
        --_finish;
    }

    iterator insert(iterator pos, const T &_val)
    {
        assert(pos < _finish);
        if (_finish == _end_of_storage)
        {
            size_t len = pos - _start;
            
            size_t _size = capacity() == 0 ? 2 : (capacity() * 2);
            reserve(_size);
            
            pos = _start + len;    
        }

        for (iterator end = _finish; end > pos; -- end)
            *end = *(end - 1);

        *(pos) = _val;
        ++ _finish;
        return pos;
    }

    iterator erase(iterator pos)
    {
        assert(pos < _finish);
        for (iterator iter = pos; pos < _finish; ++ iter)
            *iter = *(iter + 1);
        -- _finish;
        return pos;
    }

    void reserve(size_t _size)
    {
        if (_size <= capacity())
            return ;

        size_t  old_size = size();
        iterator ptr = new T[_size];
        if (_start && old_size > 0)
        {
            for (size_t i = 0; i < old_size; ++ i)
                *(ptr + i) = *(_start + i);
            delete[] _start;
        }

        _start = ptr;
        _finish = ptr + old_size;
        _end_of_storage = ptr + _size;
    }

    void resize(size_t _size, const T& _val = T())
    {
        if (_size > capacity())
        {
            reserve(2 * capacity());
        }

        for (size_t i = size(); i < _size; ++ i)
            *(_start + i) = _val;

        _finish = _start + _size;
    }

    Vector& operator=(const Vector<T> &other)
    {
        if (this != &other)
        {
            delete[] _start;
            _start = new T[other.capacity()];
            _finish = _start + other.size();
            _end_of_storage = _start + other.capacity();

            iterator iter = other.begin();
            for (size_t i = 0; i < other.size(); ++ i)
                *(_start + i) = *(iter + i);
        }
        return *this;
    }

    Vector& operator=(Vector<T> &&other) noexcept
    {
        if (this != &other)
        {
            delete[] _start;
            _start = other._start;
            _finish = other._finish;
            _end_of_storage = other._end_of_storage;

            other._start = other._finish = other._end_of_storage = nullptr;
        }
        return *this;
    }

    T& operator[](size_t pos)
    {
        assert(pos < this->size());
        return *(_start + pos);
    }
    
    const T& operator[](size_t pos) const
    {
        assert(pos < this->size());
        return *(_start + pos);
    }

    iterator begin() const { return _start; }
    iterator end() const { return _finish; }
    size_t size() const { return _finish - _start; }
    size_t capacity() const { return _end_of_storage - _start; }
    bool empty() const { return _finish == _start; }

    void swap(Vector<T> &v)
    {
        std::swap(_start, v._start);
        std::swap(_finish, v._finish);
        std::swap(_end_of_storage, v._end_of_storage);
    }

private:
    iterator _start;
    iterator _finish;
    iterator _end_of_storage;
};
