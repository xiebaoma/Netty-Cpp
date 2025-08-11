# Netty-Cpp

我参考 Moduo 的 Reactor 模型和 Netty 的缓冲区设计，实现了一个高性能网络库，支持事件驱动，非阻塞 I/O 与多线程并发处理。

* 实现了 select、poll、epoll 的事件循环(EventLoop)，Ractor 模型与线程池(ThreadPool)，支持高并发连接处理
* 封装了 TCP 连接管理(TcpConnection)，支持事件回调、消息收发等功能
* 借鉴 Netty 的 ByteBuf 设计，构建了一个灵活的缓冲区类，支持动态扩容、读写索引分离，有效提高 I/O 性能
* 实现了定时器管理模块，支持高效定时任务调度，应用于连接超时处理等场景
* 项目使用现代 C++开发，使用智能指针管理资源
* 对核心模块、辅助功能（utils）使用 Google Test 实现单元测试，确保程序的稳定
* 基于该网络库开发了一个高性能、轻量的文件服务器。能在 Linux 和 Windows 系统上运行