/*
 *  Filename:   FileServer.h
 *  Author:     xiebaoma
 *  Date:       2025-06-21
 *  Description:文件服务器主服务器类
 *              一个server ,每个连接都会产生一个唯一的connection, 一个connection会有多个session, session由server管理
 */

#pragma once
#include <memory>
#include <list>
#include <map>
#include <mutex>
#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "FileSession.h"

using namespace net;

struct StoredUserInfo
{
    int32_t userid;
    std::string username;
    std::string password;
    std::string nickname;
};

class FileServer final主l
{
public:
    FileServer() = default;
    ~FileServer() = default;

    FileServer(const FileServer &rhs) = delete;
    FileServer &operator=(const FileServer &rhs) = delete;

    bool init(const char *ip, short port, EventLoop *loop, const char *fileBaseDir = "filecache/");
    void uninit();

private:
    // 新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在oop里面调用
    void onConnected(std::shared_ptr<TcpConnection> conn);
    // 连接断开
    void onDisconnected(const std::shared_ptr<TcpConnection> &conn);

private:
    std::unique_ptr<TcpServer> m_server;                // FileServer 拥有并独占 TcpServer 对象，其他任何地方不应该持有这个 TcpServer 的指针
    std::list<std::shared_ptr<FileSession>> m_sessions; // 一个 FileSession（代表一个客户端会话）可能会被多个地方持有
    std::mutex m_sessionMutex;                          // 多线程之间保护m_sessions
    std::string m_strFileBaseDir;                       // 文件目录
};
