/*
 *  Filename:   CountDownLatch.h
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *  Description:һ������ʵ���̰߳�ȫ�ļ�����
 */

#pragma once

#include <mutex>
#include <condition_variable>

class CountDownLatch
{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
    int getCount() const;

private:
    mutable std::mutex m_mutex;
    std::condition_variable condition_;
    int count_;
};