#include <stdio.h>
#include "buddy.h"

#include <math.h>

int main() 
{
    buddy thiz = { 0 };
    buddy_init(&thiz, 8 / 1);   // 16

    void *ptr1 = buddy_alloc(&thiz, 3);
    buddy_show(&thiz);

printf("************************************\n");

    void *ptr2 = buddy_alloc(&thiz, 3);

    buddy_show(&thiz);

printf("************************************\n");
    buddy_free(&thiz, ptr1);

    buddy_show(&thiz);

    return 0;
}

// gcc main.c buddy.c -o main