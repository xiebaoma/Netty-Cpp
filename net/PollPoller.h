/*
 *  Filename:   PollPoller.h
 *  Author:     xiebaoma
 *  Date:       2025-06-26
 *  Description:使用Poll模型实现的Poller类，但是在Windows/Linux系统上都不会使用
 */

#pragma once

#ifndef _WIN32
#include "Poller.h"

#include <vector>
#include <map>

struct pollfd;

namespace net
{
    class Channel;
    class EventLoop;

    class PollPoller : public Poller
    {
    public:
        PollPoller(EventLoop *loop);
        virtual ~PollPoller();

        virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels);
        virtual bool updateChannel(Channel *channel);
        virtual void removeChannel(Channel *channel);

        void assertInLoopThread() const;

    private:
        void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

    private:
        typedef std::vector<struct pollfd> PollFdList;
        typedef std::map<int, Channel *> ChannelMap;

        ChannelMap m_channels;
        PollFdList m_pollfds;
        EventLoop *m_ownerLoop;
    };
}

#endif
