/*
 *  Filename:   EventLoop.h
 *  Author:     xiebaoma
 *  Date:       2025-06-24
 *  Description:循环类
 */

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../base/Timestamp.h"
#include "../base/Platform.h"
#include "Callbacks.h"
#include "Sockets.h"
#include "TimerId.h"
#include "TimerQueue.h"

namespace net
{
    class EventLoop;
    class Channel;
    class Poller;
    class CTimerHeap;

    class EventLoop
    {
    public:
        typedef std::function<void()> Functor;
        EventLoop();
        ~EventLoop();
        void loop();
        void quit();
        Timestamp pollReturnTime() const { return m_pollReturnTime; }
        int64_t iteration() const { return m_iteration; }
        void runInLoop(const Functor &cb);
        void queueInLoop(const Functor &cb);
        TimerId runAt(const Timestamp &time, const TimerCallback &cb);
        TimerId runAfter(int64_t delay, const TimerCallback &cb);
        TimerId runEvery(int64_t interval, const TimerCallback &cb);
        void cancel(TimerId timerId, bool off);
        void remove(TimerId timerId);
        TimerId runAt(const Timestamp &time, TimerCallback &&cb);
        TimerId runAfter(int64_t delay, TimerCallback &&cb);
        TimerId runEvery(int64_t interval, TimerCallback &&cb);
        void setFrameFunctor(const Functor &cb);
        bool updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        bool hasChannel(Channel *channel);
        void assertInLoopThread()
        {
            if (!isInLoopThread())
            {
                abortNotInLoopThread();
            }
        }
        bool isInLoopThread() const { return m_threadId == std::this_thread::get_id(); }
        bool eventHandling() const { return m_eventHandling; }
        const std::thread::id getThreadID() const
        {
            return m_threadId;
        }

    private:
        bool createWakeupfd();
        bool wakeup();
        void abortNotInLoopThread();
        bool handleRead();
        void doOtherTasks();
        void printActiveChannels() const;

    private:
        typedef std::vector<Channel *> ChannelList;

        bool m_looping;
        bool m_quit;
        bool m_eventHandling;
        bool m_doingOtherTasks;
        const std::thread::id m_threadId;
        Timestamp m_pollReturnTime;
        std::unique_ptr<Poller> m_poller;
        std::unique_ptr<TimerQueue> m_timerQueue;
        int64_t m_iteration;

#ifdef _WIN32
        SOCKET m_wakeupFdSend;
        SOCKET m_wakeupFdListen;
        SOCKET m_wakeupFdRecv;
#else
        SOCKET m_wakeupFd;
#endif
        std::unique_ptr<Channel> m_wakeupChannel;

        ChannelList m_activeChannels;
        Channel *currentActiveChannel_;
        std::mutex m_mutex;
        std::vector<Functor> m_pendingFunctors;
        Functor m_frameFunctor;
    };
}