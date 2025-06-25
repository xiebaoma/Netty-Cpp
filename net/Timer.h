/*
 *  Filename:   Timer.h
 *  Author:     xiebaoma
 *  Date:       2025-06-25
 *  Description:定时任务类
 */

#pragma once

#include <atomic>
#include <stdint.h>
#include "../base/Timestamp.h"
#include "../net/Callbacks.h"

namespace net
{
    class Timer
    {
    public:
        Timer(const TimerCallback &cb, Timestamp when, int64_t interval, int64_t repeatCount = -1);
        Timer(TimerCallback &&cb, Timestamp when, int64_t interval);

        void run();

        bool isCanceled() const
        {
            return m_canceled;
        }

        void cancel(bool off)
        {
            m_canceled = off;
        }

        Timestamp expiration() const { return m_expiration; }
        int64_t getRepeatCount() const { return m_repeatCount; }
        int64_t sequence() const { return m_sequence; }

        static int64_t numCreated() { return s_numCreated; }

    private:
        Timer(const Timer &rhs) = delete;
        Timer &operator=(const Timer &rhs) = delete;

    private:
        const TimerCallback m_callback;
        Timestamp m_expiration;
        const int64_t m_interval;
        int64_t m_repeatCount; // 重复次数，-1 表示一直重复下去
        const int64_t m_sequence;
        bool m_canceled; // 是否处于取消状态

        static std::atomic<int64_t> s_numCreated; // 静态变量，统计创建了多少个定时器，线程安全，用于标记定时器的序号
    };
}
