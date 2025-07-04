/*
 *  Filename:   AsyncLog.h
 *  Author:     xiebaoma
 *  Date:       2025-06-17
 *              2025-06-18
 *  Description:一个异步日志类
 */

#pragma once

#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

 // 如果想将这个异步日志类导出成一个动态库，你需要做以下宏定义
 /*
 #ifdef _WIN32
   #ifdef BUILD_LOG_DLL
     #define LOGAPI __declspec(dllexport)  // 编译DLL时导出符号
   #else
     #define LOGAPI __declspec(dllimport)  // 使用DLL时导入符号
   #endif
 #else
   #define LOGAPI   // Linux 下通常不需要显式导出
 #endif
 */
 // 然后定义类：
 /*

 */
enum LOG_LEVEL
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_SYSERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_CRITICAL,
};

#define LOG_TRACE(...) CAsyncLog::output(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) CAsyncLog::output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) CAsyncLog::output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) CAsyncLog::output(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) CAsyncLog::output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_SYSERROR(...) CAsyncLog::output(LOG_LEVEL_SYSERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) CAsyncLog::output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) CAsyncLog::output(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_DEBUG_BIN(buf, buflength) CAsyncLog::outputBinary(buf, buflength)

class CAsyncLog
{
public:
    static bool init(const char* pszLogFileName = nullptr, bool b_TruncateLongLine = false, int64_t nRollSize = 10 * 1024 * 1024);
    static void uninit();
    static void setLevel(LOG_LEVEL nLevel);
    static bool isRunning();

    // 不输出线程ID号和所在的函数签名、行号
    static bool output(long nLevel, const char* pszFmt, ...);
    // 输出线程ID号的所在函数的签名、行号
    static bool output(long nLevel, const char* pszFileName, int nLineNo, const char* pszFmt, ...);
    // 输出数据包的二进制形式
    static bool outputBinary(unsigned char* buffer, size_t size);

private:
    CAsyncLog() = delete;
    ~CAsyncLog() = delete;
    CAsyncLog(const CAsyncLog& rhs) = delete;
    CAsyncLog& operator=(const CAsyncLog& rhs) = delete;

    static void makeLinePrefix(long nLevel, std::string& strPrefix);
    static void getTime(char* pszTime, int nTimeStrLength);
    static bool createNewFile(const char* pszLogFileName);
    static bool writeToFile(const std::string& data);
    static void crash();
    static std::string ullto4Str(int n);
    static char* formLog(int& index, char* szbuf, size_t size_buf, unsigned char* buffer, size_t size);
    static void writeThreadProc();
    static void GetPIDString(char* szPID, size_t size);
    static std::string getCurrentTimeString();

private:
    static bool m_bToTile;                               // 日志写入文件还是控制台
    static FILE* m_hLogFile;                             // 文件句柄
    static std::string m_strFileName;                    // 文件名
    static std::string m_strFileNamePID;                 // 文件名中的进程ID
    static bool m_bTruncateLongLog;                      // 长日志是否截断
    static LOG_LEVEL m_nCurrentLeval;                    // 当前日志级别
    static int64_t m_nFileRollSize;                      // 单个日志文件的最大字节数(10MB)
    static int64_t m_CurrentWrittenSize;                 // 已经写入的字节数
    static std::list<std::string> m_listLinesToWrite;    // 带写入的日志
    static std::unique_ptr<std::thread> m_spWriteThread; // 指针，指向日志线程，用unique_ptr是因为日志线程只由日志类管理
    static std::mutex m_mutexWrite;                      // 锁
    static std::condition_variable m_cvWrite;            // 条件变量
    static bool m_bExit;                                 // 推出标志
    static bool m_bRunning;                              // 运行标志
};