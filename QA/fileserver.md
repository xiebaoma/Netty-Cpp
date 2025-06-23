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