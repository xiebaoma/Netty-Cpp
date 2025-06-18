/*
 *  Filename:   AsyncLog.h
 *  Author:     xiebaoma
 *  Date:       2025-06-18
 *  Description:一个异步日志类
 */

#include <ctime>
#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <cstdio>
#ifdef _WIN32
#include <windows.h> // GetCurrentProcessId
#else
#include <unistd.h> // getpid
#endif

#include "../base/Platform.h"
#include "AsyncLog.h"

#define MAX_LINE_LENGTH 256
#define DEFAULT_POLL_SIZE 10 * 1024 * 1024

bool CAsyncLog::m_bTruncateLongLog = false;
FILE *CAsyncLog::m_hLogFile = nullptr;
std::string CAsyncLog::m_strFileName = "default";
std::string CAsyncLog::m_strFileNamePID = "";
LOG_LEVEL CAsyncLog::m_nCurrentLeval = LOG_LEVEL_INFO;
int64_t CAsyncLog::m_nFileRollSize = DEFAULT_POLL_SIZE;
int64_t CAsyncLog::m_CurrentWrittenSize = 0;
std::list<std::string> CAsyncLog::m_listLinesToWrite;
std::unique_ptr<std::thread> CAsyncLog::m_spWriteThread;
std::mutex CAsyncLog::m_mutexWrite;
std::condition_variable CAsyncLog::m_cvWrite;
bool CAsyncLog::m_bExit = false;
bool CAsyncLog::m_bRunning = false;

bool CAsyncLog::init(const char *pszLogFileName, bool b_TruncateLongLine, int64_t nRollSize)
{
    m_bTruncateLongLog = b_TruncateLongLine;
    m_nFileRollSize = nRollSize;
    if (pszLogFileName == nullptr || pszLogFileName[0] == 0)
    {
        m_strFileName.clear();
    }
    else
    {
        m_strFileName = pszLogFileName;
    }

    char szPID[8];
    GetPIDString(szPID, sizeof(szPID));
    m_strFileNamePID = szPID;

    m_spWriteThread.reset(new std::thread(writeThreadProc));

    return true;
}

bool CAsyncLog::uninit()
{
    m_bExit = true;

    m_cvWrite.notify_one();

    if (m_spWriteThread->joinable())
    {
        m_spWriteThread->join();
    }

    if (m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
        m_hLogFile = nullptr;
    }
}

void CAsyncLog::setLevel(LOG_LEVEL nLevel)
{
    if (nLevel < LOG_LEVEL_TRACE || nLevel > LOG_LEVEL_FATAL)
    {
        return;
    }
    m_nCurrentLeval = nLevel;
}

bool CAsyncLog::isRunning()
{
    return m_bRunning;
}

bool CAsyncLog::output(long nLevel, const char *pszFmt, ...)
{
    if (nLevel != LOG_LEVEL_CRITICAL)
    {
        if (nLevel < m_nCurrentLeval)
        {
            return false;
        }
    }

    std::string strLine;
    makeLinePrefix(nLevel, strLine);
}

void CAsyncLog::makeLinePrefix(long nLevel, std::string &strPrefix)
{
    strPrefix = "[INFO]";
    if (nLevel == LOG_LEVEL_TRACE)
        strPrefix = "[TRACE]";
    else if (nLevel == LOG_LEVEL_DEBUG)
        strPrefix = "[DEBUG]";
    else if (nLevel == LOG_LEVEL_WARNING)
        strPrefix = "[WARNING]";
    else if (nLevel == LOG_LEVEL_ERROR)
        strPrefix = "[ERROR]";
    else if (nLevel == LOG_LEVEL_SYSERROR)
        strPrefix = "[SYSERROR]";
    else if (nLevel == LOG_LEVEL_FATAL)
        strPrefix = "[FATAL]";
    else if (nLevel == LOG_LEVEL_CRITICAL)
        strPrefix = "[CRITICAL]";

    char szTime[64] = {0};
    getTime(szTime, sizeof(szTime));
    strPrefix += "[";
    strPrefix += szTime;
    strPrefix += "]";

    char szThreadID[32] = {0};
    std::ostringstream osThreadID;
    osThreadID << std::this_thread::get_id();
    snprintf(szThreadID, sizeof(szThreadID), "[%s]", osThreadID.str().c_str());
    strPrefix += szThreadID;
}

void CAsyncLog::getTime(char *pszTime, int nTimeStrLength)
{
    struct timeb tp;
    ftime(&tp);

    time_t now = tp.time;
    tm time;
#ifdef _WIN32
    localtime_s(&time, &now);
#else
    localtime_r(&now, &time);
#endif

    snprintf(pszTime, nTimeStrLength, "[%04d-%02d-%02d %02d:%02d:%02d:%03d]",
             time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
             time.tm_hour, time.tm_min, time.tm_sec, tp.millitm);
}

bool CAsyncLog::createNewFile(const char *pszLogFileName)
{
    if (m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
    }
    m_hLogFile = fopen(pszLogFileName, "w+");
    return m_hLogFile != nullptr;
}

bool CAsyncLog::writeToFile(const std::string &data)
{
    std::string strLocal(data);
    int ret = 0;
    while (true)
    {
        ret = fwrite(strLocal.c_str(), 1, strLocal.length(), m_hLogFile);
        if (ret <= 0)
        {
            return false;
        }
        else if (ret < (int)strLocal.length())
        {
            strLocal.erase(0, ret);
        }

        if (strLocal.empty())
        {
            break;
        }
    }
    fflush(m_hLogFile);
    return true;
}

void CAsyncLog::crash()
{
    char *p = nullptr;
    *p = 0;
}

std::string CAsyncLog::ullto4Str(int n)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%06u", static_cast<unsigned int>(n));
    return std::string(buf);
}

void CAsyncLog::writeThreadProc()
{
}

void CAsyncLog::GetPIDString(char *szPID, size_t size)
{
#ifdef _WIN32
    snprintf(szPID, size, "%05d", (int)GetCurrentProcessId());
#else
    snprintf(szPID, size, "%05d", (int)getpid());
#endif
}
