
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "ThreadPool.h"
#include "MPMC-ThreadPool.h"

using namespace mylib;

#define THREAD_NUM  (4)

// printf("[F:%s][L:%d]\r\n", __FUNCTION__, __LINE__);

int main()
{
    MpmcThreadPool _pool(10);
    sleep(10086);


    ThreadPool pool("");
    if (THREAD_NUM != pool.start(THREAD_NUM))
    {
        printf("[F:%s][L:%d]\r\n", __FUNCTION__, __LINE__);
    }

    for (int i = 0; i < 10; ++ i)
    {
        pool.addTask([i] {
            printf("Thread[%lu] doTask[%d]\r\n", 
                syscall(SYS_gettid) % THREAD_NUM, i);
        });
    }

    sleep(10086);

    return 0;
}


// g++ main.cpp -o main
