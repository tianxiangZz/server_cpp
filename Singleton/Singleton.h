#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/******************************************************************* */

#include <iostream>
#include <atomic>
#include <mutex>

template <typename T>
class Singleton1
{
private:
    class GC
    {
    private:
    public:
        GC() {
            std::cout << "Singleton1::GC" << std::endl;
        }
        ~GC() {
            std::cout << "~Singleton1::GC" << std::endl;
            T *data = Singleton1::GetInstance();
            if (nullptr != data)
                delete data;
        }
    };
    static GC gc;
    static std::mutex mutex;
    static std::atomic<T *> instance;

    Singleton1() = default;
    ~Singleton1() = default;
public:
    Singleton1(const Singleton1<T> &) = delete;
    Singleton1& operator = (const Singleton1<T> &) = delete;

    static T *GetInstance()
    {
        T *tmp = instance.load(std::memory_order_relaxed);
        if (nullptr == tmp)
        {
            std::lock_guard<std::mutex> lock(mutex);
            tmp = instance.load(std::memory_order_relaxed);
            if (nullptr == tmp)
            {
                ((void)(gc));
                tmp = new T();
                instance.store(tmp, std::memory_order_relaxed);
            }
        }
        return tmp;
    }
};

template <typename T>
std::atomic<T *> Singleton1<T>::instance = nullptr;

template <typename T>
std::mutex Singleton1<T>::mutex;

template <typename T>
typename Singleton1<T>::GC Singleton1<T>::gc;


/******************************************************************* */

#include <mutex>

template <typename T>
class Singleton2
{
private:
    class GC
    {
    private:
    public:
        GC() {
            std::cout << "Singleton2::GC" << std::endl;
        }
        ~GC() {
            std::cout << "~Singleton2::GC" << std::endl;
            T *data = Singleton2::GetInstance();
            if (nullptr != data)
                delete data;
        }
    };    

    static GC gc;
    static T* instance;
    static std::once_flag once_flag_;
    Singleton2() = default;

public:
    ~Singleton2() = default;
    Singleton2(const Singleton2<T> &) = delete;
    Singleton2& operator = (const Singleton2<T> &) = delete;

    static T *GetInstance()
    {
        std::call_once(once_flag_, [](){
            ((void)(gc));
            instance = new T();
        });
        return instance;
    }
};

template<typename T>
std::once_flag Singleton2<T>::once_flag_;

template<typename T>
T* Singleton2<T>::instance = nullptr;

template<typename T>
typename Singleton2<T>::GC Singleton2<T>::gc;


/******************************************************************* */




template <typename T>
class Singleton3
{
protected:
    Singleton3() = default;
    virtual ~Singleton3() = default;

public:
    Singleton3(const Singleton3<T> &) = delete;
    Singleton3& operator = (const Singleton3<T> &) = delete;

    static T& GetInstance()
    {
        static T instance;
        return instance;
    }
};




#endif