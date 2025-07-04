/*
 *  Filename:   AsyncLog.h
 *  Author:     xiebaoma
 *  Date:       2025-06-17
 *              2025-06-18
 *  Description:һ���첽��־��
 */

#pragma once

#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

 // ����뽫����첽��־�ർ����һ����̬�⣬����Ҫ�����º궨��
 /*
 #ifdef _WIN32
   #ifdef BUILD_LOG_DLL
     #define LOGAPI __declspec(dllexport)  // ����DLLʱ��������
   #else
     #define LOGAPI __declspec(dllimport)  // ʹ��DLLʱ�������
   #endif
 #else
   #define LOGAPI   // Linux ��ͨ������Ҫ��ʽ����
 #endif
 */
 // Ȼ�����ࣺ
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

    // ������߳�ID�ź����ڵĺ���ǩ�����к�
    static bool output(long nLevel, const char* pszFmt, ...);
    // ����߳�ID�ŵ����ں�����ǩ�����к�
    static bool output(long nLevel, const char* pszFileName, int nLineNo, const char* pszFmt, ...);
    // ������ݰ��Ķ�������ʽ
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
    static bool m_bToTile;                               // ��־д���ļ����ǿ���̨
    static FILE* m_hLogFile;                             // �ļ����
    static std::string m_strFileName;                    // �ļ���
    static std::string m_strFileNamePID;                 // �ļ����еĽ���ID
    static bool m_bTruncateLongLog;                      // ����־�Ƿ�ض�
    static LOG_LEVEL m_nCurrentLeval;                    // ��ǰ��־����
    static int64_t m_nFileRollSize;                      // ������־�ļ�������ֽ���(10MB)
    static int64_t m_CurrentWrittenSize;                 // �Ѿ�д����ֽ���
    static std::list<std::string> m_listLinesToWrite;    // ��д�����־
    static std::unique_ptr<std::thread> m_spWriteThread; // ָ�룬ָ����־�̣߳���unique_ptr����Ϊ��־�߳�ֻ����־�����
    static std::mutex m_mutexWrite;                      // ��
    static std::condition_variable m_cvWrite;            // ��������
    static bool m_bExit;                                 // �Ƴ���־
    static bool m_bRunning;                              // ���б�־
};