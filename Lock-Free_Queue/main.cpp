#include <iostream>
#include <stdio.h>
#include <thread>
#include "mpsc_lockfreeQueue.h"

struct Node
{
    int proId;
    int index;
    Node () {}
    Node (int id, int idx): proId(id), index(idx) {}
    Node (const Node &other): proId(other.proId), index(other.index) {}
} ;

mpsc_lockfreeQueue<Node, 16> thiz;
#define TEST_NUM (8)

void producer(int ID)
{
    for (int i = 0; i < TEST_NUM; ++ i)
    {
        while (true != thiz.send(Node(ID, i)))
        {
            printf("pro[%d] Send[%d] error!\n", ID, i);
        }
        printf("pro[%d] Send[%d] sucess!\n", ID, i);
    }
}

void consumer()
{
    size_t cnt = 0;
    Node node(0, 0);
    while (thiz.recv(node, RECV_WAIT_FOREVER))
    {
        printf("consumer Recv  pro[%d] Index[%d] sucess! [%ld]\n", node.proId, node.index, cnt ++);
    }
    printf("consumer Recv error!\n");
    return ;
}



int main() 
{
    std::thread producers[4];
    for (int i = 0; i < 4; ++ i)
    {
        producers[i] = std::thread(producer, i);
    }
    std::thread _consumer(consumer);

    producers->join();
    _consumer.join();
    return 0;
}