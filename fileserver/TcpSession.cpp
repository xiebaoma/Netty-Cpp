/*
 *  Filename:   TcpSession.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-22
 *  Description:一个业务类的父类，应该让不同业务的业务类继承这个类
 */

#include "TcpSession.h"
#include "../base/AsyncLog.h"
#include "../net/ProtocolStream.h"
#include "FileMsg.h"

TcpSession::TcpSession(const std::weak_ptr<TcpConnection> &tmpconn) : tmpConn_(tmpconn)
{
}

TcpSession::~TcpSession()
{
}

void TcpSession::send(int32_t cmd, int32_t seq, int32_t errorcode, const std::string &filemd5, int64_t offset, int64_t filesize, const std::string &filedata)
{
    std::string outbuf;
    net::BinaryStreamWriter writeStream(&outbuf);
    writeStream.WriteInt32(cmd);
    writeStream.WriteInt32(seq);
    writeStream.WriteInt32(errorcode);
    writeStream.WriteString(filemd5);
    writeStream.WriteInt64(offset);
    writeStream.WriteInt64(filesize);
    writeStream.WriteString(filedata);
    writeStream.Flush();

    sendPackage(outbuf.c_str(), outbuf.length());
}

void TcpSession::sendPackage(const char *body, int64_t bodylength)
{
    string strPackageData;
    file_msg_header header = {(int64_t)bodylength};
    strPackageData.append((const char *)&header, sizeof(header));
    strPackageData.append(body, bodylength);

    std::shared_ptr<TcpConnection> conn = tmpConn_.lock();
    if (!conn)
    {
        LOG_ERROR("Tcp connection is destroyed, but TcpSession is still alive?");
        return;
    }

    LOG_INFO("Send data, package length: %d, body length: %d", strPackageData.length(), bodylength);
    // LOG_DEBUG_BIN((unsigned char*)body, bodylength);
    conn->send(strPackageData.c_str(), strPackageData.length());
}