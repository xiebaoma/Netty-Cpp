/*
 *  Filename:   AsyncLog.h
 *  Author:     xiebaoma
 *  Date:       2025-06-17
 *  Description:一个异步日志类
 */

#pragma once

#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>

//如果想将这个异步日志类导出成一个动态库，你需要做以下宏定义
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
//然后定义类：
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

#define LOGT(...) CAsyncLog::output(LOG_LEVEL_TRACE, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_DEBUG, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_INFO, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_WARNING, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_ERROR, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_SYSERROR, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_FATAL, _FILE_, _LINE_, _VA_ARGS_);
#define LOGT(...) CAsyncLog::output(LOG_LEVEL_CRITICAL, _FILE_, _LINE_, _VA_ARGS_);

#define LOG_DEBUG_BIN(buf, buflength) CAsyncLog::outputBinary(buf, buflength);

