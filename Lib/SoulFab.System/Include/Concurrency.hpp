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
        //????????????????????????????????????
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

        //push?????????CAS???tail???
        bool push(T val);

        //pop?????????CAS???head???
        bool tryPop(T& val);

        //??????????????????????????????????????????????????????
        T pop();

    private:
        struct Node {
            std::atomic<T> val;
            std::atomic<Node*>next = NULL;
            Node(T val) :val(val) {

            }
        };
        const T Empty = 0;
        std::atomic<int>count = { 0 };  //?????????
        std::atomic<Node*>head;  //?????????
        std::atomic<Node*>tail;   //?????????
        bool islockfree;
    };

    //push?????????CAS???tail???
    template<typename T>
    bool LockFreeLinkedQueue<T>::push(T val) {
        Node* t = NULL;
        Node* node = new Node(val);
        while (true) {
            //t==NULL?????????tail?????????
            if (t == NULL) {
                t = tail.load();
                continue;
            }
            //?????????tail???
            if (!tail.compare_exchange_weak(t, NULL))
                continue;
            break;
        }
        t->next.store(node);
        ++count;
        Node* expected = NULL;
        //??????tail???
        bool flag = tail.compare_exchange_weak(expected, t->next);
        assert(flag);
        return true;
    }

    //pop?????????CAS???head???
    template<typename T>
    bool LockFreeLinkedQueue<T>::tryPop(T& val) {
        Node* h = NULL, * h_next = NULL;
        while (true) {
            //h==NULL?????????head?????????
            if (h == NULL) {
                h = head.load();
                continue;
            }
            //?????????head???
            if (!head.compare_exchange_weak(h, NULL))
                continue;
            h_next = h->next.load();
            //h->next != NULL ??? count == 0  
            //   ?????????push?????????????????????count???????????????????????????????????????????????????
            if (h_next != NULL) {
                while (count.load() == 0)
                    std::this_thread::yield();  //???
            }
            break;
        }
        Node* expected = NULL;
        Node* desired = h;
        //???h_next==NULL???
        //   ????????????????????????
        if (h_next != NULL) {
            val = h_next->val;
            delete h;
            desired = h_next;
            --count;
        }
        //CAS head?????????head???
        bool flag = head.compare_exchange_weak(expected, desired);
        assert(flag);
        return h_next != NULL;
    }

    //??????????????????????????????????????????????????????
    template<typename T>
    T LockFreeLinkedQueue<T>::pop() {
        Node* h = NULL, * h_next = NULL;
        while (true) {
            //h==NULL?????????head?????????
            if (h == NULL) {
                h = head.load();
                continue;
            }
            //?????????head???
            if (!head.compare_exchange_weak(h, NULL))
                continue;
            h_next = h->next.load();
            //h->next == NULL 
            //   ????????????????????????????????????head??????
            //h->next != NULL ??? count == 0  
            //   ?????????push?????????????????????count???????????????????????????????????????????????????
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
        //CAS head?????????head???
        bool flag = head.compare_exchange_weak(expected, desired);
        assert(flag);
        return val;
    }

#pragma endregion

}

#endif
