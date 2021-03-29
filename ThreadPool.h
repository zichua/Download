#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <iostream>

using namespace std;

class ThreadPool
{
    public : 
        ThreadPool(size_t n); //构造函数，n为工作线程数

        //添加任务函数到任务队列
        template<class F,class...Args>
        auto Append(F&& f,Args&&... args)->future<typename result_of<F(Args...)>::type>
        // {
        //     //获取返回值类型
        //     using return_type = typename result_of<F(Args...)>::type;
        //     //获取任务函数
        //     auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f),forward<Args>(args)...));
        
        //     future<return_type> res = task->get_future();

        //     unique_lock<mutex> lock(this->tasks_mutex);
        //     this->tasks.emplace([task](){(*task)();});
        //     lock.unlock();

        //     this->condition.notify_one();

        //     return res;
        // }
        ;
        void Wait(); //等待所有线程任务结束

        ~ThreadPool();

        static void work(ThreadPool *,int);
    private : 
        vector<thread> work_threads; //工作线程
        queue<function<void()>> tasks; //任务队列
        mutex tasks_mutex; //互斥锁
        condition_variable condition; //条件变量
        bool stop;

};

template<class F,class ...Args>
auto ThreadPool::Append(F&& f,Args&&... args)->future<typename result_of<F(Args...)>::type>
{
    //获取返回值类型
    using return_type = typename result_of<F(Args...)>::type;
    //获取任务函数
    auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f),forward<Args>(args)...));

    future<return_type> res = task->get_future();

    unique_lock<mutex> lock(this->tasks_mutex);
    this->tasks.emplace([task](){(*task)();});
    lock.unlock();

    this->condition.notify_one();

    return res;
}