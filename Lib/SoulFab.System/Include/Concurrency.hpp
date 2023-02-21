#pragma once

#ifndef _SF_CONCURRENCY_H_
#define _SF_CONCURRENCY_H_

#include <list>
#include <map>
#include <atomic>
#include <functional>

#include "Synchronization.hpp"
#include "Atomic.hpp"

namespace SoulFab::System
{

#pragma region ConcurrencyList

    template<class T>
    class ConcurrencyList
    {
    public:
        ConcurrencyList();

        void Add(T& item);
        void Clear();
        void Foreach(std::function<bool(T&)> func);

        int Count();

    private:
        CriticalSection Locker;
        std::list<T> _List;

    };

    template<class T>
    ConcurrencyList<T>::ConcurrencyList()
    {
    }

    template<class T>
    void ConcurrencyList<T>::Add(T& item)
    {
        LockerGuard LG(this->Locker);

        this->_List.push_back(item);
    }

    template<class T>
    void ConcurrencyList<T>::Clear()
    {
        LockerGuard LG(this->Locker);

        this->_List.clear();
    }

    template<class T>
    int ConcurrencyList<T>::Count()
    {
        LockerGuard LG(this->Locker);

        return this->_List.size();
    }

    template<class T>
    void ConcurrencyList<T>::Foreach(std::function<bool(T&)> func)
    {
        LockerGuard LG(this->Locker);

        for (auto it = this->_List.begin(); it != this->_List.end(); it++)
        {
            if (!func(*it))
            {
                it = this->_List.erase(it);
            }
        }
    }

#pragma endregion



    class ConcurrencyMap
    {

    };

    //
    //
    //

#pragma region ConcurrencyQueue

    template <typename T>
    struct LinkNode
    {
        T Data;
        LinkNode<T>* volatile Next;

        LinkNode(T v)
            : Next(nullptr)
        {
            Data = v;
        }
    };

    template <typename T>
    class ConcurrencyQueue
    {
    public:
        ConcurrencyQueue();

        void Push(T t);
        T Pop(void);

        bool IsEmpty(void);
        int Length();

    private:
        LinkNode<T>* volatile Head;
        LinkNode<T>* volatile Tail;

        unsigned long Count;
    };

    template <typename T>
    ConcurrencyQueue<T>::ConcurrencyQueue()
        : Head(nullptr), Tail(nullptr), Count(0)
    {
    }

    template <typename T>
    void ConcurrencyQueue<T>::Push(T v)
    {
        LinkNode<T>* new_node = new LinkNode<T>(v);
        LinkNode<T>* p;
        do
        {
            p = (LinkNode<T>*)Tail;
        } while (!Atomic::CompareExchange(&p->Next, new_node, (LinkNode<T>*)nullptr));

        Atomic::CompareExchange(&Tail, new_node, p);

        Count++;
    }

    template <typename T>
    T ConcurrencyQueue<T>::Pop()
    {
        LinkNode<T>* p;

        do
        {
            //record the first node.
            p = (LinkNode<T>*)Head->Next;
            if (!p)
            {
                return 0;
            }
        } while (!Atomic::CompareExchange(&(Head->Next), (LinkNode<T>*)(p->Next), p));

        if (Count > 0) Count--;
        if (Count == 0)
        {
            // if the queue is empty then the tail to header.
            do
            {

            } while (!Atomic::CompareExchange(&Tail, (LinkNode<T>*)Head, p));
        }

        return p->Data;
    }

    template <typename T>
    bool ConcurrencyQueue<T>::IsEmpty()
    {
        if (Count == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename T>
    int ConcurrencyQueue<T>::Length()
    {
        return Count;
    }

#pragma endregion

#pragma region LockFreeLinkedQueue

    template<typename T>
    class LockFreeLinkedQueue {
    public:
        //保证初始化在单线程下完成
        LockFreeLinkedQueue() {
            Node* node = new Node(Empty);
            head.store(node);
            tail.store(node);
            islockfree = node->val.is_lock_free();
        }
        ~LockFreeLinkedQueue() {
            T val = Empty;
            while (tryPop(val));
            Node* node = head.load();
            if (node != NULL)
                delete node;
        }

        bool is_lock_free() {
            return islockfree;
        }

        bool isEmpty() { return count.load() == 0; }
        bool isFull() { return false; }

        //push操作，CAS加tail锁
        bool push(T val);

        //pop操作，CAS加head锁
        bool tryPop(T& val);

        //不建议使用，当队列中无元素时，会自旋
        T pop();

    private:
        struct Node {
            std::atomic<T> val;
            std::atomic<Node*>next = NULL;
            Node(T val) :val(val) {

            }
        };
        const T Empty = 0;
        std::atomic<int>count = { 0 };  //计数器
        std::atomic<Node*>head;  //头结点
        std::atomic<Node*>tail;   //尾结点
        bool islockfree;
    };

    //push操作，CAS加tail锁
    template<typename T>
    bool LockFreeLinkedQueue<T>::push(T val) {
        Node* t = NULL;
        Node* node = new Node(val);
        while (true) {
            //t==NULL，表示tail锁被抢
            if (t == NULL) {
                t = tail.load();
                continue;
            }
            //尝试加tail锁
            if (!tail.compare_exchange_weak(t, NULL))
                continue;
            break;
        }
        t->next.store(node);
        ++count;
        Node* expected = NULL;
        //释放tail锁
        bool flag = tail.compare_exchange_weak(expected, t->next);
        assert(flag);
        return true;
    }

    //pop操作，CAS加head锁
    template<typename T>
    bool LockFreeLinkedQueue<T>::tryPop(T& val) {
        Node* h = NULL, * h_next = NULL;
        while (true) {
            //h==NULL，表示head锁被抢
            if (h == NULL) {
                h = head.load();
                continue;
            }
            //尝试加head锁
            if (!head.compare_exchange_weak(h, NULL))
                continue;
            h_next = h->next.load();
            //h->next != NULL 且 count == 0  
            //   此时在push函数中数据以及count计数器没有来得及更新，因此进行自旋
            if (h_next != NULL) {
                while (count.load() == 0)
                    std::this_thread::yield();  //???
            }
            break;
        }
        Node* expected = NULL;
        Node* desired = h;
        //当h_next==NULL时
        //   表示当前链表为空
        if (h_next != NULL) {
            val = h_next->val;
            delete h;
            desired = h_next;
            --count;
        }
        //CAS head，释放head锁
        bool flag = head.compare_exchange_weak(expected, desired);
        assert(flag);
        return h_next != NULL;
    }

    //不建议使用，当队列中无元素时，会自旋
    template<typename T>
    T LockFreeLinkedQueue<T>::pop() {
        Node* h = NULL, * h_next = NULL;
        while (true) {
            //h==NULL，表示head锁被抢
            if (h == NULL) {
                h = head.load();
                continue;
            }
            //尝试加head锁
            if (!head.compare_exchange_weak(h, NULL))
                continue;
            h_next = h->next.load();
            //h->next == NULL 
            //   当前队列为空，是否需要解head锁？
            //h->next != NULL 且 count == 0  
            //   此时在push函数中数据以及count计数器没有来得及更新，因此进行自旋
            while (h_next == NULL || count.load() == 0) {
                std::this_thread::yield();  //???
                if (h_next == NULL)
                    h_next = h->next.load();
            }
            break;
        }
        T val = h_next->val;
        delete h;
        --count;
        Node* expected = NULL;
        Node* desired = h_next;
        //CAS head，释放head锁
        bool flag = head.compare_exchange_weak(expected, desired);
        assert(flag);
        return val;
    }

#pragma endregion

}

#endif
