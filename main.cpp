/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>

template <typename T>
struct Node {
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node<T>> next;
    Node<T>* prev;
    // 如果能改成 unique_ptr 就更好了!

    T value;

    // 这个构造函数有什么可以改进的？
    Node(T val) : value(val) {}

    void insert(T val) {
        auto node = std::make_unique<Node<T>>(val);
        node->next = std::move(next);
        node->prev = prev;
        if (node->next)
            node->next->prev = node.get();
        if (prev)
            prev->next = std::move(node);
    }

    void erase() {
        if (prev) {
            prev->next = std::move(next);
            if (prev->next)
                prev->next->prev = prev;
        }
        else if (next)
            next->prev = prev;
    }

    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

template <typename T>
struct List {

    std::unique_ptr<Node<T>> head;

    List() = default;

    List(List<T> const &other) {
        printf("List 被拷贝！\n");
        // head = other.head;  // 这是浅拷贝！
        // 请实现拷贝构造函数为 **深拷贝**
        Node<T>* otherCurr{other.front()};
        head = std::make_unique<Node<T>>(otherCurr->value);
        for (Node<T>* curr{front()}; otherCurr->next; otherCurr = otherCurr->next.get(), curr = curr->next.get()) {
            curr->next = std::make_unique<Node<T>>(otherCurr->next->value);
        }
    }

    List<T> &operator=(List<T> const &) = delete;  // 为什么删除拷贝赋值函数也不出错？

    List(List<T> &&) = default;
    List<T> &operator=(List<T> &&) = default;

    Node<T> *front() const {
        return head.get();
    }

    T pop_front() {
        T ret = head->value;
        head = std::move(head->next);
        return ret;
    }

    template <typename... Ts>
    void push_front(T value, Ts... others) {
        auto node = std::make_unique<Node<T>>(value);
        node->next = std::move(head);
        if (node->next)
            node->next->prev = node.get();
        head = std::move(node);
        if constexpr (sizeof...(others) != 0)
            push_front(others...);
    }

    Node<T> *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }

    class iterator
    {
        Node<T>* ptr_;
    public:
        iterator(Node<T>* p = nullptr) : ptr_(p) {}

        T operator*() const {
            return ptr_->value;
        }

        Node<T>* operator->() const {
            return ptr_;
        }

        iterator& operator++() {
            ptr_ = ptr_->next.get();
            return *this;
        }

        iterator operator++(int) {
            Node<T>* ret{ ptr_ };
            ptr_ = ptr_->next;
            return ret;
        }

        bool operator==(iterator const& other) const {
            return other.ptr_ == ptr_;
        }

        bool operator!=(iterator const& other) const {
            return other.ptr_ != ptr_;
        }
    };

    iterator begin() const {
        return iterator(head.get());
    }

    iterator end() const {
        return nullptr;
    }
};

template <typename T>
void print(List<T> const& lst) {  // 有什么值得改进的？
    printf("[");
    for (auto curr : lst) {
        printf(" %d", curr);
    }
    printf(" ]\n");
}

int main() {
    List<int> a;

    a.push_front(7, 5, 8, 2, 9, 4, 1);

    print(a);   // [ 1 4 9 2 8 5 7 ]

    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List<int> b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    return 0;
}
