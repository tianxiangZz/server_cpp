#include <stddef.h>
#include <atomic>
#include <memory>

template <typename T>
class mpmc_lockfreeQueue
{

public:
    mpmc_lockfreeQueue()
        :head_(nullptr), tail_(nullptr), size_(0)
    {
        Node *sentinel = new Node(T());
        head_.store(sentinel);
        tail_.store(sentinel);
    }

    ~mpmc_lockfreeQueue()
    {
        while (Node *node = head_.load())
        {
            head_.store(node->next_.load());
            delete node;
        }
    }

    void Push(const T &data)
    {
        Node* newNode = new Node(data);
        Node *oldTail = nullptr, *next = nullptr;

        do {
            oldTail = tail_.load();
            next = oldTail->next_.load();
            if (oldTail != tail_.load())
                continue;
            if (nullptr != next)
            {
                tail_.compare_exchange_weak(oldTail, next);
                continue;
            }
        } while (!(oldTail->next_.compare_exchange_weak(next, newNode)));

        tail_.compare_exchange_weak(oldTail, newNode);
        size_.fetch_add(1);
    }

    bool Pop(T &data)
    {
        if (size_.load() < 1)
            return false;
        
        Node *oldHead = nullptr, *oldTail = nullptr, *next = nullptr;
        do {
            oldHead = head_.load();
            oldTail = tail_.load();
            next = oldHead->next_.load();
            if (oldHead != head_.load())
                continue;
            if (oldHead == oldTail && next == nullptr)
                return false;
        } while (!head_.compare_exchange_weak(oldHead, next));
        data = std::move(next->data_);
        delete oldHead;
    }


private:
    struct Node {
        std::atomic<Node *>     next_;
        T                       data_;

        Node (const T &data, Node *node = nullptr)
            :data_(data), next_(node)
        {}
    } ;

    std::atomic<Node *> head_;
    std::atomic<Node *> tail_;
    std::atomic<size_t> size_;
};


