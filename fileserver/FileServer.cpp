/*
 *  Filename:   FileServer.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-21
 *  Description:文件服务器主服务器类
 */

#include "FileServer.h"
#include "../net/InetAddress.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "FileSession.h"

bool FileServer::init(const char *ip, short port, EventLoop *loop, const char *fileBaseDir /* = "filecache/"*/)
{
    m_strFileBaseDir = fileBaseDir;

    InetAddress addr(ip, port);
    m_server.reset(new TcpServer(loop, addr, "ZYL-MYImgAndFileServer", TcpServer::kReusePort));
    // 设置有连接的回调函数
    m_server->setConnectionCallback(std::bind(&FileServer::onConnected, this, std::placeholders::_1));
    // 启动侦听
    m_server->start(6);

    return true;
}

void FileServer::uninit()
{
    if (m_server)
        m_server->stop();
}

void FileServer::onConnected(std::shared_ptr<TcpConnection> conn)
{
    if (conn->connected())
    {
        LOG_INFO("client connected: %s", conn->peerAddress().toIpPort().c_str());
        std::shared_ptr<FileSession> spSession(new FileSession(conn, m_strFileBaseDir.c_str()));
        conn->setMessageCallback(std::bind(&FileSession::onRead, spSession.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        std::lock_guard<std::mutex> guard(m_sessionMutex);
        m_sessions.push_back(spSession);
    }
    else
    {
        onDisconnected(conn);
    }
}

void FileServer::onDisconnected(const std::shared_ptr<TcpConnection> &conn)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);

    for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
    {
        auto sessionConn = (*iter)->getConnectionPtr();
        if (sessionConn == nullptr)
        {
            continue; // 忽略异常 session
        }

        if (sessionConn == conn)
        {
            LOG_INFO("client disconnected: %s", conn->peerAddress().toIpPort().c_str());
            m_sessions.erase(iter); // 安全删除
            return;                 // 只处理一个 session，立即退出
        }
    }

    LOG_WARN("Disconnected connection not found in sessions list: %s", conn->peerAddress().toIpPort().c_str());
}
