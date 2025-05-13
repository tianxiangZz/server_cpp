#include <iostream>
#include <thread>
#include <mutex>
#include "spin_lock.h"

#define NUM  (1000)
#define THREAD_NUM  (30)
static int ans = 0;


int main()
{
    SpinLock lock;
    std::thread threads[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; ++ i)
    {
        threads[i] = std::thread([&lock](){
            for (int i = 0; i < NUM; ++ i)
            {
                std::lock_guard<SpinLock> _Lock(lock);
                ans += 1;
            }
        });
    }
    
    for(int i = 0; i < THREAD_NUM; ++ i)
    {
        threads[i].join();
    }

    std::cout << ans << std::endl;
    return 0;
}