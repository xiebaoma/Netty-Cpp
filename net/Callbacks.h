/*
 *  Filename:   Callbacks.h
 *  Author:     xiebaoma
 *  Date:       2025-06-27
 *  Description:统一定义网络库中常用的回调函数类型
 */

#pragma once

#include <functional>
#include <memory>
#include "../base/Timestamp.h"

namespace net
{
    class ByteBuffer;
    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
    typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr &, size_t)> HighWaterMarkCallback;

    typedef std::function<void(const TcpConnectionPtr &, ByteBuffer *, Timestamp)> MessageCallback;

    void defaultConnectionCallback(const TcpConnectionPtr &conn);
    void defaultMessageCallback(const TcpConnectionPtr &conn, ByteBuffer *buffer, Timestamp receiveTime);
}
