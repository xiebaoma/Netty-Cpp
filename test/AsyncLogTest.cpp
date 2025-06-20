/*
 *  Filename:   AsyncLogTest.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-20
 *  Description:测试日志
 *  command:    g++ AsyncLogTest.cpp ../base/AsyncLog.cpp -o test
 */

#include "../base/AsyncLog.h"
#include <cstring>
#include <thread>
#include <chrono>

int main()
{
    // 初始化日志系统
    if (!CAsyncLog::init("test_log.txt"))
    {
        printf("Failed to initialize async log system.\n");
        return 1;
    }

    // 设置日志级别
    CAsyncLog::setLevel(LOG_LEVEL_DEBUG);

    // 输出不同级别的日志

    LOG_TRACE("This is a TRACE message.");
    LOG_DEBUG("This is a DEBUG message.");
    LOG_INFO("This is an INFO message.");
    LOG_WARN("This is a WARNING message.");
    LOG_ERROR("This is an ERROR message.");
    LOG_SYSERROR("This is a SYSERROR message.");
    LOG_FATAL("This is a FATAL message.");
    LOG_CRITICAL("This is a CRITICAL message.");

    // 模拟日志包含变量的输出
    int value = 42;
    const char *name = "AsyncLogger";
    LOG_INFO("Logging some variables: value = %d, name = %s", value, name);

    // 输出二进制日志
    unsigned char binaryData[32];
    for (int i = 0; i < sizeof(binaryData); ++i)
    {
        binaryData[i] = static_cast<unsigned char>(i);
    }
    LOG_DEBUG_BIN(binaryData, sizeof(binaryData));

    // 模拟等待日志写入线程工作
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 反初始化
    CAsyncLog::uninit();

    return 0;
}
