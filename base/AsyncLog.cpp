/*
 *  Filename:   AsyncLog.h
 *  Author:     xiebaoma
 *  Date:       2025-06-18
 *              2025-06-19
 *              2025-06-20
 *  Description:一个异步日志类
 */

#include <algorithm>
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
#define NOMINMAX     // 避免 Windows 头文件污染 min/max 某些老旧系统或库（比如 Windows.h）中，有可能定义了 min 宏，会污染命名空间
#include <windows.h> // GetCurrentProcessId
#else
#include <unistd.h> // getpid
#endif

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

void CAsyncLog::uninit()
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
    return output(nLevel, nullptr, 0, pszFmt);
}

bool CAsyncLog::output(long nLevel, const char *pszFileName, int nLineNo, const char *pszFmt, ...)
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

    // 如果有文件名和行号，则追加
    if (pszFileName != nullptr)
    {
        char szFileName[512] = {0};
        snprintf(szFileName, sizeof(szFileName), " [%s:%d]", pszFileName, nLineNo);
        strLine += szFileName;
    }

    // log正文
    std::string strLogMsg;

    // 计算一下不定参数的长度
    va_list ap;
    va_start(ap, pszFmt);
    int nLogMsgLength = vsnprintf(NULL, 0, pszFmt, ap);
    va_end(ap);

    // 容量必须算上最后一个0
    if ((int)strLogMsg.capacity() < nLogMsgLength + 1)
    {
        strLogMsg.resize(nLogMsgLength + 1);
    }
    va_list aq;
    va_start(aq, pszFmt);
    vsnprintf((char *)strLogMsg.data(), strLogMsg.capacity(), pszFmt, aq);
    va_end(aq);

    // string内容正确但length不对，恢复一下length
    std::string strMsgFormal;
    strMsgFormal.append(strLogMsg.c_str(), nLogMsgLength);

    // 如果日志开启截断，长日志只取前MAX_LINE_LENGTH个字符
    if (m_bTruncateLongLog)
    {
        strMsgFormal = strMsgFormal.substr(0, MAX_LINE_LENGTH);
    }

    strLine += strMsgFormal;

    // 如果不输出到控制台，就在每一行末尾加一个'\n'
    if (!m_strFileName.empty())
    {
        strLine += '\n';
    }
    // 到此，日志内容已准备好

    // 如果日志级别不是FATAL，就添加到缓冲区，否之就直接crash
    if (nLevel != LOG_LEVEL_FATAL)
    {
        std::lock_guard<std::mutex> lock_guard(m_mutexWrite);
        m_listLinesToWrite.push_back(strLine);
        m_cvWrite.notify_one();
    }
    else
    {
        // FATAL级别的日志需要立刻crash程序，所以采取同步写日志
        std::cout << strLine << std::endl;
#ifdef _WIN32
        // 向调试器输出调试信息
        OutputDebugStringA(strLine.c_str());
        OutputDebugStringA("\n");
#endif

        if (!m_strFileName.empty())
        {
            if (m_hLogFile == nullptr)
            {
                // 准备时间
                std::string szNow;
                szNow = getCurrentTimeString();

                // 准备文件名
                std::string strNewFileName(m_strFileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_strFileNamePID;
                strNewFileName += ".log";

                // 新建文件,之后文件句柄已经指向新的文件
                if (!createNewFile(strNewFileName.c_str()))
                {
                    return false;
                }
            }
            // 将日志内容写入新文件
            writeToFile(strLine);
        }
        // 让程序主动crash
        crash();
    }
    return true;
}

bool CAsyncLog::outputBinary(unsigned char *buffer, size_t size)
{
    std::ostringstream os;
    static const size_t PRINTSIZE = 512; // 每次读取的最大字节数
    char szbuf[PRINTSIZE * 3 + 8];       // 一个字符最多三个字节，再加8个字节保险
    size_t lsize = 0;                    // 已读取的字节数
    int index = 0;

    // 使用 reinterpret_cast<uintptr_t> 更跨平台，避免 long 在 32/64 位下表现不一致
    os << "address[" << reinterpret_cast<uintptr_t>(buffer) << "] size[" << size << "] \n";

    while (lsize < size)
    {
        size_t lprintbufsize = std::min(size - lsize, PRINTSIZE);
        formLog(index, szbuf, sizeof(szbuf), buffer + lsize, lprintbufsize);
        os << szbuf;
        lsize += lprintbufsize;
    }

    {
        std::lock_guard<std::mutex> lock_guard(m_mutexWrite);
        m_listLinesToWrite.push_back(os.str());
    }
    m_cvWrite.notify_one();
    return true;
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
    if (!m_hLogFile)
        return false;

    size_t written = fwrite(data.c_str(), 1, data.size(), m_hLogFile);
    if (written != data.size())
    {
        fprintf(stderr, "writeToFile failed: %s\n", strerror(errno));
        return false;
    }

    fflush(m_hLogFile); // 或者考虑延迟刷盘优化性能
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

/*
ask GPT, not very understand
格式化逻辑：
每 32 字节输出一行，行前加上 ullto4Str(index) 作为“行头编号”，并加空格。
每 16 字节额外加个空格，便于阅读。
每个字节按 XX 十六进制格式输出。
e.g:
000000  4a5f1e2a7b0c...（32 字节）
000001  9d0a22ffab34...（32 字节）
*/
char *CAsyncLog::formLog(int &index, char *szbuf, size_t size_buf, unsigned char *buffer, size_t size)
{
    size_t len = 0;
    size_t lsize = 0;
    const char hexchars[] = "0123456789abcdef";

    while (lsize < size && len + 4 < size_buf) // 保留末尾 '\n' 和 '\0'
    {
        if (lsize % 32 == 0)
        {
            if (lsize != 0 && len + 1 < size_buf)
                szbuf[len++] = '\n';

            int headlen = snprintf(szbuf + len, size_buf - len, "%s ", ullto4Str(index++));
            if (headlen <= 0 || static_cast<size_t>(headlen) >= size_buf - len)
                break; // snprintf 出错或缓冲不足
            len += headlen;
        }

        if (lsize % 16 == 0 && len + 1 < size_buf)
            szbuf[len++] = ' ';

        if (len + 2 < size_buf)
        {
            szbuf[len++] = hexchars[(buffer[lsize] >> 4) & 0xF];
            szbuf[len++] = hexchars[buffer[lsize] & 0xF];
        }
        else
        {
            break; // 防止越界
        }

        lsize++;
    }

    if (len + 2 <= size_buf)
    {
        szbuf[len++] = '\n';
        szbuf[len] = '\0';
    }
    else if (len < size_buf)
    {
        szbuf[len] = '\0'; // 结尾兜底
    }
    return szbuf;
}

void CAsyncLog::writeThreadProc()
{
    m_bRunning = true;
    while (true)
    {
        if (!m_strFileName.empty())
        {
            if (m_hLogFile == nullptr || m_CurrentWrittenSize >= m_nFileRollSize)
            {
                m_CurrentWrittenSize = 0;

                // 准备时间
                std::string szNow;
                szNow = getCurrentTimeString();

                std::string strNewFileName(m_strFileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_strFileNamePID;
                strNewFileName += ".log";
                if (!createNewFile(strNewFileName.c_str()))
                    return;
            }
        }

        std::string strLine;

        /*
        条件变量 wait() 可能被虚假唤醒（spurious wakeup），也就是说，即使没有 notify_one() 调用，它也可能突然返回。
        因此，你必须重新检查条件是否满足（即日志队列是否非空）。
        */
        {
            std::unique_lock<std::mutex> guard(m_mutexWrite);
            while (m_listLinesToWrite.empty())
            {
                if (m_bExit)
                    return;
                m_cvWrite.wait(guard);
            }
            strLine = m_listLinesToWrite.front();
            m_listLinesToWrite.pop_front();
        }

        std::cout << strLine << std::endl;

#ifdef _WIN32
        OutputDebugStringA(strLine.c_str());
        OutputDebugStringA("\n");
#endif

        if (!m_strFileName.empty())
        {
            if (!writeToFile(strLine))
                return;
            m_CurrentWrittenSize += strLine.length();
        }
    }
    m_bRunning = false;
}

void CAsyncLog::GetPIDString(char *szPID, size_t size)
{
#ifdef _WIN32
    snprintf(szPID, size, "%05d", (int)GetCurrentProcessId());
#else
    snprintf(szPID, size, "%05d", (int)getpid());
#endif
}

std::string CAsyncLog::getCurrentTimeString()
{
    char szNow[64];
    time_t now = time(NULL);
    tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now);
#else
    localtime_r(&now, &timeinfo);
#endif
    strftime(szNow, sizeof(szNow), "%Y%m%d%H%M%S", &timeinfo);
    return std::string(szNow);
}
