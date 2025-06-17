/*
 *  Filename:   DaemonRunTest.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-17
 *  Description:测试让服务端以守护进程方式运行的函数
 *  Command:    g++ DaemonRunTest.cpp ../utils/DaemonRun.cpp -o test
 */
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>

#include "../utils/DaemonRun.h"

#ifdef _WIN32
#include <windows.h>
#define sleep_seconds(x) Sleep((x) * 1000) // Windows 的 Sleep 单位是毫秒
#else
#include <unistd.h>
#define sleep_seconds(x) sleep(x) // Linux 的 sleep 单位是秒
#endif

// 写日志的函数
void write_log(const std::string &message)
{
#ifdef _WIN32
    std::ofstream log("daemon_log_test.txt", std::ios::app);
#else
    std::ofstream log("daemon_log_test.txt", std::ios::app);
#endif
    if (log.is_open())
    {
        log << message << std::endl;
    }
}

std::string current_time()
{
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&now));
    return std::string(buf);
}

int main()
{
    if (!daemon_run())
    {
        std::cerr << "Failed to daemonize!" << std::endl;
        return 1;
    }

    // 每2秒写一次日志，总共写5次（运行10秒）
    const int interval_seconds = 2;
    const int total_duration_seconds = 10;
    int count = total_duration_seconds / interval_seconds;

    for (int i = 0; i < count; ++i)
    {
        write_log("Daemon alive at: " + current_time());
        sleep_seconds(interval_seconds);
    }

    write_log("Daemon exiting at: " + current_time());
    return 0;
}