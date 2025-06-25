/*
 *  Filename:   SelectPoller.h
 *  Author:     xiebaoma
 *  Date:       2025-06-25
 *  Description:select 模型，windows系统将使用这个模型
 */

#include "SelectPoller.h"

#include <sstream>
#include <string.h>

#include "../base/Platform.h"
#include "../base/AsyncLog.h"
#include "EventLoop.h"
#include "Channel.h"

using namespace net;

namespace
{
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

SelectPoller::SelectPoller(EventLoop *loop) : m_ownerLoop(loop)
{
}

SelectPoller::~SelectPoller()
{
}

bool SelectPoller::hasChannel(Channel *channel) const
{
    assertInLoopThread();
    ChannelMap::const_iterator it = m_channels.find(channel->fd());
    return it != m_channels.end() && it->second == channel;
}

void SelectPoller::assertInLoopThread() const
{
    m_ownerLoop->assertInLoopThread();
}

Timestamp SelectPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    // 获取最大fd
    int maxfd = 0;

    int tmpfd;
    for (const auto &iter : m_channels)
    {
        tmpfd = iter.first;
        if (tmpfd > maxfd)
            maxfd = tmpfd;

        if (iter.second->events() & XPOLLIN)
            FD_SET(tmpfd, &readfds);

        if (iter.second->events() & XPOLLOUT)
            FD_SET(tmpfd, &writefds);
    }

    struct timeval timeout;
    timeout.tv_sec = timeoutMs / 1000;
    timeout.tv_usec = (timeoutMs - timeoutMs / 1000 * 1000) * 1000;
    int numEvents = select(maxfd + 1, &readfds, &writefds, NULL, &timeout);
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels, readfds, writefds);
        if (static_cast<size_t>(numEvents) == m_events.size())
        {
            m_events.resize(m_events.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        // LOG_TRACE << " nothing happended";
    }
    else
    {
        int savedErrno;
#ifdef _WIN32
        savedErrno = (int)WSAGetLastError();
#else
        savedErrno = errno;
#endif

        LOG_SYSERROR("SelectPoller::poll() error, errno: %d", savedErrno);
    }

    return now;
}

void SelectPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels, fd_set &readfds, fd_set &writefds) const
{
    Channel *channel = NULL;
    bool eventTriggered = false;
    // TODO：每次遍历channels_，效率太低，能否改进？
    int currentCount = 0;
    for (const auto &iter : m_channels)
    {
        channel = iter.second;

        if (FD_ISSET(iter.first, &readfds))
        {
            channel->add_revents(XPOLLIN);
            eventTriggered = true;
        }

        if (FD_ISSET(iter.first, &writefds))
        {
            channel->add_revents(XPOLLOUT);
            eventTriggered = true;
        }

        if (eventTriggered)
        {
            activeChannels->push_back(channel);
            // 重置标志
            eventTriggered = false;

            ++currentCount;
            if (currentCount >= numEvents)
                break;
        }
    } // end for-loop
}
// 当前遍历是通过 m_channels 映射进行的，如果 fd 数量比较大，但活跃数较少（典型场景），可以反过来从 fd=0~FD_SETSIZE 扫描 fd_set，只有在 FD_ISSET() 为 true 时再查 m_channels。
/**
void SelectPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels, fd_set &readfds, fd_set &writefds) const
{
    for (int fd = 0, count = 0; fd < FD_SETSIZE && count < numEvents; ++fd)
    {
        bool eventTriggered = false;
        int revents = 0;

        if (FD_ISSET(fd, &readfds))
        {
            revents |= XPOLLIN;
            eventTriggered = true;
        }
        if (FD_ISSET(fd, &writefds))
        {
            revents |= XPOLLOUT;
            eventTriggered = true;
        }

        if (eventTriggered)
        {
            auto it = m_channels.find(fd);
            if (it != m_channels.end())
            {
                Channel* channel = it->second;
                channel->set_revents(revents);
                activeChannels->push_back(channel);
                ++count;
            }
        }
    }
}

 */

bool SelectPoller::updateChannel(Channel *channel)
{
    assertInLoopThread();
    const int index = channel->index();
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            if (m_channels.find(fd) != m_channels.end())
            {
                LOG_ERROR("fd = %d must not exist in channels_", fd);
                return false;
            }
            m_channels[fd] = channel;
        }
        else
        {
            if (m_channels.find(fd) == m_channels.end())
            {
                LOG_ERROR("fd = %d must not exist in channels_", fd);
                return false;
            }

            if (m_channels[fd] != channel)
            {
                LOG_ERROR("current channel is not matched current fd, fd = %d", fd);
                return false;
            }
        }
        channel->set_index(kAdded);

        return update(XEPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        if (m_channels.find(fd) == m_channels.end() || m_channels[fd] != channel || index != kAdded)
        {
            LOG_ERROR("current channel is not matched current fd, fd = %d, channel = 0x%x", fd, channel);
            return false;
        }

        if (channel->isNoneEvent())
        {
            if (update(XEPOLL_CTL_DEL, channel))
            {
                channel->set_index(kDeleted);
                return true;
            }
            return false;
        }
        else
        {
            return update(XEPOLL_CTL_MOD, channel);
        }
    }
}

void SelectPoller::removeChannel(Channel *channel)
{
    assertInLoopThread();
    int fd = channel->fd();

    auto it = m_channels.find(fd);
    if (it == m_channels.end() || it->second != channel)
    {
        LOG_ERROR("Channel not found or mismatched in removeChannel, fd=%d", fd);
        return;
    }

    if (!channel->isNoneEvent())
    {
        LOG_WARN("Trying to remove a channel that is still monitoring events, fd=%d", fd);
        // 也可以考虑自动 disable：
        // channel->disableAll();
        return;
    }

    int index = channel->index();
    size_t n = m_channels.erase(fd);
    if (n != 1)
    {
        LOG_ERROR("Failed to erase channel, fd=%d", fd);
        return;
    }

    if (index == kAdded)
    {
        update(XEPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}

bool SelectPoller::update(int operation, Channel *channel)
{
    int fd = channel->fd();
    if (operation == XEPOLL_CTL_ADD)
    {
        struct epoll_event event;
        memset(&event, 0, sizeof event);
        event.events = channel->events();
        event.data.ptr = channel;

        m_events.push_back(std::move(event));
        return true;
    }

    if (operation == XEPOLL_CTL_DEL)
    {
        for (auto iter = m_events.begin(); iter != m_events.end(); ++iter)
        {
            if (iter->data.ptr == channel)
            {
                m_events.erase(iter);
                return true;
            }
        }
    }
    else if (operation == XEPOLL_CTL_MOD)
    {
        for (auto iter = m_events.begin(); iter != m_events.end(); ++iter)
        {
            if (iter->data.ptr == channel)
            {
                iter->events = channel->events();
                return true;
            }
        }
    }

    std::ostringstream os;
    os << "SelectPoller update fd failed, op = " << operation << ", fd = " << fd;
    os << ", events_ content: \n";
    for (const auto &iter : m_events)
    {
        os << "fd: " << iter.data.fd << ", Channel: 0x%x: " << iter.data.ptr << ", events: " << iter.events << "\n";
    }
    LOGE("%s", os.str().c_str());

    return false;
}
