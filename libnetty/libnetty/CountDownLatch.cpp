/*
 *  Filename:   CountDownLatch.h
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *  Description:一个辅助实现线程安全的计数类
 */

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) : count_(count)
{
}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (count_ > 0)
    {
        condition_.wait(lock);
    }
}

void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    --count_;
    if (count_ == 0)
    {
        condition_.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return count_;
}