#ifndef __NETLIB_NONCOPYABLE_H__    // pragma once 编译器级别
#define __NETLIB_NONCOPYABLE_H__

/**
 * from Boost
 * noncopyable 被继承后
 *      派生类对象可以正常构造和析构
 *      但是无法进行 拷贝构造和赋值构造
 * 
 * 两种做法:
 * 1.将拷贝构造和赋值构造函数设置为私有函数 即可防止派生类使用
 * 2.将拷贝构造和赋值构造函数利用C++ 关键字delete删除函数
 **/

class Noncopyable
{
private:
    // Noncopyable(const Noncopyable &);
    // Noncopyable& operator=(const Noncopyable &);

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
public:
    // 禁止拷贝构造
    Noncopyable(const Noncopyable &) = delete;
    // 禁止赋值构造
    Noncopyable& operator = (const Noncopyable &) = delete;
    // void operator = (const Noncopyable &) = delete;
};

#endif  /** __NETLIB_NONCOPYABLE_H__ */