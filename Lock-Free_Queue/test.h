#include <atomic>
#include <memory>
#include <type_traits>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;  // 使用共享指针便于内存管理
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        explicit Node(T&& value) 
            : data(std::make_shared<T>(std::move(value))), next(nullptr) {}
    };

    // 带标签的指针结构，防止ABA问题
    struct TaggedPtr {
        Node* ptr;
        size_t tag;

        bool operator==(const TaggedPtr& other) const {
            return ptr == other.ptr && tag == other.tag;
        }
    };

    // 保证原子操作的结构体
    static_assert(sizeof(TaggedPtr) <= 2 * sizeof(uintptr_t),
                "TaggedPtr too large for atomic operations");

    std::atomic<TaggedPtr> head;  // 虚拟头节点
    std::atomic<TaggedPtr> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        TaggedPtr init{dummy, 0};
        head.store(init, std::memory_order_relaxed);
        tail.store(init, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        while (pop());  // 清空队列
        delete head.load().ptr;  // 删除虚拟节点
    }

    // 禁止拷贝
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

    void push(T&& value) {
        Node* newNode = new Node(std::move(value));
        TaggedPtr oldTail = tail.load(std::memory_order_acquire);
        
        while (true) {
            TaggedPtr oldNext{nullptr, 0};
            oldTail.ptr->next.compare_exchange_weak(
                oldNext.ptr, 
                newNode,
                std::memory_order_release,
                std::memory_order_acquire
            );

            // CAS成功则更新尾节点
            if (oldNext.ptr == nullptr) {
                TaggedPtr newTail{newNode, oldTail.tag + 1};
                tail.compare_exchange_weak(
                    oldTail,
                    newTail,
                    std::memory_order_release,
                    std::memory_order_relaxed
                );
                return;
            }
            // 帮助其他线程完成尾节点更新
            else {
                TaggedPtr newTail{oldNext.ptr, oldTail.tag + 1};
                tail.compare_exchange_weak(
                    oldTail,
                    newTail,
                    std::memory_order_release,
                    std::memory_order_relaxed
                );
                oldTail = tail.load(std::memory_order_acquire);
            }
        }
    }

    std::shared_ptr<T> pop() {
        TaggedPtr oldHead = head.load(std::memory_order_acquire);
        
        while (true) {
            Node* nextPtr = oldHead.ptr->next.load(std::memory_order_acquire);
            
            // 队列为空
            if (nextPtr == nullptr) {
                return nullptr;
            }

            TaggedPtr newHead{nextPtr, oldHead.tag + 1};
            
            // 尝试原子更新头节点
            if (head.compare_exchange_weak(
                oldHead,
                newHead,
                std::memory_order_release,
                std::memory_order_relaxed)) 
            {
                std::shared_ptr<T> res = nextPtr->data;
                // 安全删除旧头节点
                delete oldHead.ptr;
                return res;
            }
        }
    }

    bool empty() const {
        return head.load(std::memory_order_acquire).ptr->next.load(
            std::memory_order_acquire) == nullptr;
    }
};