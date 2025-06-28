FileSession.h:

* 一个session有一个weak_ptr，指向一个connection，当这个connection销毁时，这session会自动销毁吗?
不会，除非循环引用，

* 客户端分段上传文件
使用offset
offset 表示这段 filedata 在整个文件中的起始位置。比如说：
文件总大小为 1000 字节
客户端这次上传 filedata 的长度为 100 字节
offset = 300
那么表示：客户端这次发过来的数据应该写入到文件的第 300 ~ 399 字节。

* 服务端响应客户下载文件时感觉存在线程安全问题？

* 文件服务器和图片服务器使用了相同的一套代码，是否合适？

* SelectPoller.h, Poller每次都要遍历整个channellist链表，效率太低
可以反过来遍历fd_set，有事件的时候再找到对应的Channel.

* SelectPoller.h：channellist的延迟删除机制，
| Channel 状态 | 操作意图   | 会发生什么                  | index 变化     |
| ----------- | ------ | ---------------------- | ------------ |
| `kNew`     | 第一次注册  | 加入 `m_channels`，执行 ADD | → `kAdded`   |
| `kDeleted` | 重新启用   | 不加入 map，只执行 ADD        | → `kAdded`   |
| `kAdded`   | 删除事件监听 | 执行 DEL                 | → `kDeleted` |
| `kAdded`   | 修改监听事件 | 执行 MOD                 | 保持不变         |
可能很快又会重新启用（避免反复插入删除 map）；
保留对象存在，避免悬空指针或 iterator 失效；
更利于统一管理 Channel 生命周期。

* SelectPoller.h 
有两个列表，一个是Channel注册列表，一个是给操作系统内核的事件列表，

* SelectPoller.cpp::removeChannel()不会内存泄漏吗？

* 一个高效、线程安全、非阻塞的写操作机制
用户线程：
   TcpConnection::send() → 通过 runInLoop → TcpConnection::sendInLoop()
         ↓
   若能直接写完 → 写完成回调（可选）
         ↓
   若没写完 → 数据进入 m_outputBuffer + 注册 EPOLLOUT 监听
         ↓

I/O线程（事件触发）：
   EPOLLOUT触发 → TcpConnection::handleWrite()
         ↓
   写 socket → 如果写完 → 关闭写事件 + 写完成回调
            → 如果还有数据 → 等下次继续写
            → 如果失败 → handleClose()
