# 网络连接出错

<img src="C:\Users\24025\Desktop\myfile\MingX\docs\img\屏幕截图 2025-02-11 011615.png" alt="屏幕截图 2025-02-11 011615" style="zoom: 50%;" />



**端口没问题**：

![](C:\Users\24025\Desktop\myfile\MingX\docs\img\屏幕截图 2025-02-11 010412.png)



**数据库没问题**：chatserver第一次启动的时候就创建好库和表了



**客户端唯一的修改**：改了\flamingoclient\Bin\config\flamingo.ini中的三个IP地址

<img src="C:\Users\24025\Desktop\myfile\MingX\docs\img\屏幕截图 2025-02-11 011130.png"  />



**客户端网络设置没问题**：

<img src="C:\Users\24025\Desktop\myfile\MingX\docs\img\屏幕截图 2025-02-11 011650.png" alt="屏幕截图 2025-02-11 011650" style="zoom: 50%;" />

**本地能ping通**:

<img src="C:\Users\24025\Desktop\myfile\MingX\docs\img\屏幕截图 2025-02-11 120721.png" style="zoom:50%;" />

## 解决方案

重新部署在阿里云上

SB腾讯服务器

# C++版本不同导致编译报错

error: ‘uint8_t’ does not name a type

## 解决方法

确保编译时使用 C++11 或更高标准。

包含<cstdint>头文件

`uint8_t` 定义在 `<cstdint>` 头文件中。如果代码中没有包含该头文件，编译器将无法识别 `uint8_t`。不同机器上的编译器版本或编译标准可能不同。如果一台机器使用的是 C++11 或更高标准，而另一台机器使用的是较低的标准（如 C++98），则可能导致 `uint8_t` 不可用。

