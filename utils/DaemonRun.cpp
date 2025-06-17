/*
 *  Filename:   DaemonRun.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-17
 *  Description:让服务端以守护进程方式运行的函数
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include <iostream>

bool daemon_run()
{
#ifdef _WIN32
    // Windows：创建一个新的无窗口进程
    FreeConsole(); // 脱离控制台

    HWND hWnd = GetConsoleWindow();
    if (hWnd != NULL)
        ShowWindow(hWnd, SW_HIDE);

    // 你可以进一步通过 CreateProcess 创建一个新进程运行自己（高级）
    return true;
#else
    // Linux 守护进程化标准流程
    pid_t pid = fork();
    if (pid < 0)
    {
        return false;
    }
    if (pid > 0)
    {
        // 父进程退出
        exit(0);
    }

    // 子进程继续
    if (setsid() < 0)
    {
        return false;
    }

    // 第二次 fork，防止重新获得终端
    pid = fork();
    if (pid < 0)
    {
        return false;
    }
    if (pid > 0)
    {
        exit(0);
    }

    // 改变当前目录
    chdir("/");

    // 设置文件权限掩码
    umask(0);

    // 重定向标准输入输出到 /dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO)
            close(fd);
    }

    return true;
#endif
}
