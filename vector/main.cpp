#include <stdio.h>
#include "Vector.h"

void Printf(Vector<int> &thiz)
{
    printf("size = %ld, capacity = %ld\n", thiz.size(), thiz.capacity());
    for (size_t i = 0; i < thiz.size(); ++ i)
        printf("[%ld] = %d ", i, thiz[i]);
    printf("\n");
}

void TEST(Vector<int> &thiz)
{
    thiz.reserve(6);
    Printf(thiz);

    thiz.push_back(1);
    Printf(thiz);

    thiz.push_back(2);
    Printf(thiz);

    thiz.push_back(3);
    Printf(thiz);

    thiz.resize(2);
    Printf(thiz);
}


int main()
{
    Vector<int> a;

    TEST(a);


    return 0;
}