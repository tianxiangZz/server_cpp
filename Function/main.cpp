#include <iostream>
#include "Function.h"

int add(int a, int b)
{
    return a + b;
}
int sub(int a, int b)
{
    return b - a;
}

int main()
{
    function<int(int, int)> f(sub);
    
    
    
    std::cout << f(1, 2) << std::endl;


    return 0;
}