#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t n) : stop(false)
{
    for(size_t i=0;i<n;++i)
    {
        work_threads.emplace_back(work,this,i);
    }
}

ThreadPool::~ThreadPool()
{
    unique_lock<mutex> lock(this->tasks_mutex);
    this->stop = true;
    lock.unlock();
    //唤醒所有wait状态的线程
    condition.notify_all();
    //等待所有线程执行完
    for(thread &x : work_threads)
    {
        x.join();
    }
}

void ThreadPool::Wait()
{
    for(auto &x : work_threads)
    {
        x.join();
    }
}

void ThreadPool::work(ThreadPool *t,int t_num)
{
    while(!t->stop)
    {
        unique_lock<mutex> lock(t->tasks_mutex);
        t->condition.wait(lock,[t]{return t->stop || !t->tasks.empty();});

        //结束时
        if(t->stop && t->tasks.empty())
            return;
        //从任务队列拿出函数
        function<void()> task = t->tasks.front();
        t->tasks.pop();
        lock.unlock();
        //执行函数
        task();
    }
}