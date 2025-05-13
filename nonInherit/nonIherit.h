/**
 *  _nonInherit:  将构造函数定义为 私有函数
 *                让继承其的类T为它的友元类，
 *                因此它的子类T可以继承，且调用它的构造函数
 * 
 * nonInherit:   通过虚继承_nonInherit类，来达到  该类不能被继续 继承的作用
 * 
 * 虚继承: 要求最后继承的类去构造基类
 *          也就是Derived去构造_nonInherit类
 *          但是_nonInherit 构造函数是私有的，Derived也不是其友元，因此无法构造
 *          所以不能被继承
 * 
 * 
 *          因此 nonInherit 是最后的一个继承类
 */



template <typename T>
class _nonInherit
{
    friend T;
private:
    _nonInherit() {}
    virtual ~_nonInherit() {}
};


class nonInherit : virtual public _nonInherit<nonInherit>
{
private:
    /* data */
public:
    nonInherit() { }
    ~nonInherit() { }
};


class Derived: public nonInherit
{
private:

public:
    Derived() {}
    ~Derived() { }
};



