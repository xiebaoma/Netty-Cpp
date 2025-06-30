/*
 *  Filename:   EventLoopThread.h
 *  Author:     xiebaoma
 *  Date:       2025-06-25
 *  Description:线程的封装类，每个线程都有一个loop
 */

#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <functional>

namespace net
{
    class EventLoop;

    class EventLoopThread
    {
    public:
        typedef std::function<void(EventLoop *)> ThreadInitCallback;

        EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = "");
        ~EventLoopThread();
        EventLoop *startLoop();
        void stopLoop();

    private:
        void threadFunc();

        EventLoop *m_loop;
        bool m_exiting;
        std::unique_ptr<std::thread> m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        ThreadInitCallback m_callback;
    };

}
