#include <iostream>
#include "Singleton.h"

class TEST1
{
private:
    /* data */
public:
    TEST1() {
        std::cout << "TEST1()" << std::endl;
    }
    ~TEST1() {
        std::cout << "~TEST1()" << std::endl;
    }
};


class TEST2
{
private:
    /* data */
public:
    TEST2() {
        std::cout << "TEST2()" << std::endl;
    }
    ~TEST2() {
        std::cout << "~TEST2()" << std::endl;
    }
};


class TEST3 : Singleton3<TEST3>
{
    friend class Singleton3<TEST3>;
private:
    TEST3() {
        std::cout << "TEST3()" << std::endl;
    }
    ~TEST3() {
        std::cout << "~TEST3()" << std::endl;
        }
public:

};


int a = 10;

template <typename T>
class TEST4
{
private:
    class TEST5
    {
        public:
        int a;
        TEST5() { 
            std::cout << "TEST5()" << std::endl;
        }
        ~TEST5() { 
            std::cout << "~TEST5()" << std::endl;
        }
    };
    
public:

TEST4() { 
    
    std::cout << "TEST4()" << std::endl;
}
~TEST4() { 
    std::cout << "~TEST4()" << std::endl;
}
    static TEST5 A;
};

template <typename T>
typename TEST4<T>::TEST5 TEST4<T>::A;

// template class TEST4<int>;

int main()
{

    std::cout << Singleton1<int>::GetInstance() << std::endl;
    std::cout << Singleton1<int>::GetInstance() << std::endl;
    std::cout << Singleton1<int>::GetInstance() << std::endl;

    std::cout << Singleton1<TEST1>::GetInstance() << std::endl;
    std::cout << Singleton1<TEST1>::GetInstance() << std::endl;
    std::cout << Singleton1<TEST1>::GetInstance() << std::endl;

    std::cout << std::endl;

    std::cout << Singleton2<int>::GetInstance() << std::endl;
    std::cout << Singleton2<int>::GetInstance() << std::endl;
    std::cout << Singleton2<int>::GetInstance() << std::endl;

    std::cout << Singleton2<TEST2>::GetInstance() << std::endl;
    std::cout << Singleton2<TEST2>::GetInstance() << std::endl;
    std::cout << Singleton2<TEST2>::GetInstance() << std::endl;


    std::cout << std::endl;


    std::cout << &(Singleton3<int>::GetInstance()) << std::endl;
    std::cout << &(Singleton3<int>::GetInstance()) << std::endl;
    std::cout << &(Singleton3<int>::GetInstance()) << std::endl;

    std::cout << &(Singleton3<TEST3>::GetInstance()) << std::endl;
    std::cout << &(Singleton3<TEST3>::GetInstance()) << std::endl;
    std::cout << &(Singleton3<TEST3>::GetInstance()) << std::endl;
    return 0;
}