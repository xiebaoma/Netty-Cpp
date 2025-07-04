/*
 *  Filename:   TcpSession.h
 *  Author:     xiebaoma
 *  Date:       2025-06-22
 *  Description:一个业务类的父类，应该让不同业务的业务类继承这个类
 */

#pragma once

#include <memory>
#include "../net/TcpConnection.h"

using namespace net;

// 为了让业务与逻辑分开，实际应该新增一个子类继承自TcpSession，让TcpSession中只有逻辑代码，其子类存放业务代码
class TcpSession
{
public:
    TcpSession(const std::weak_ptr<TcpConnection> &tmpconn);
    ~TcpSession();

    TcpSession(const TcpSession &rhs) = delete;
    TcpSession &operator=(const TcpSession &rhs) = delete;

    // unique_ptr只有转成shared_ptr才能使用
    std::shared_ptr<TcpConnection> getConnectionPtr()
    {
        std::shared_ptr<TcpConnection> conn = tmpConn_.lock();
        if (!conn)
        {
            // 可选：记录日志、报警、抛出异常等
            return nullptr;
        }
        return conn;
    }

    void send(int32_t cmd, int32_t seq, int32_t errorcode, const std::string &filemd5, int64_t offset, int64_t filesize, const std::string &filedata);

private:
    // 支持大文件，用int64_t来存储包长，记得梳理一下文件上传于下载逻辑
    void sendPackage(const char *body, int64_t bodylength);

protected:
    //？？？
    // 在一个connection上会由多个session
    // TcpSession引用TcpConnection类必须是弱指针，因为TcpConnection可能会因网络出错自己销毁，此时TcpSession应该也要销毁
    std::weak_ptr<TcpConnection> tmpConn_;
};
