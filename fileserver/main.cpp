/*
 *  Filename:   main.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-23
 *  Description:fileserver程序的入口
 */
#include <iostream>
#include <stdlib.h>

#include "../base/Platform.h"
#include "../base/Singleton.h"
#include "../base/ConfigFileReader.h"
#include "../base/AsyncLog.h"
#include "../net/EventLoop.h"
#include "FileManager.h"

#ifndef _WIN32
#include <string.h>
#include "../utils/DaemonRun.h"
#endif

#include "FileServer.h"

using namespace net;

#ifdef _WIN32
// 初始化Windows socket库
NetworkInitializer windowsNetworkInitializer;
#endif

EventLoop g_mainLoop;

#ifndef _WIN32
void prog_exit(int signo)
{
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;

    Singleton<FileServer>::Instance().uninit();
    g_mainLoop.quit();
}
#endif

int main(int argc, char *argv[])
{
#ifndef _WIN32
    // 设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGTERM, prog_exit);

    int ch;
    bool bdaemon = false;
    while ((ch = getopt(argc, argv, "d")) != -1)
    {
        switch (ch)
        {
        case 'd':
            bdaemon = true;
            break;
        }
    }

    if (bdaemon)
        daemon_run();
#endif

#ifdef _WIN32
    CConfigFileReader config("../etc/fileserver.conf");
#else
    CConfigFileReader config("etc/fileserver.conf");
#endif

    std::string logFileFullPath;

#ifndef _WIN32
    const char *logfilepath = config.getConfigName("logfiledir");
    if (logfilepath == NULL)
    {
        LOG_FATAL("logdir is not set in config file");
        return 1;
    }

    // 如果log目录不存在则创建之
    DIR *dp = opendir(logfilepath);
    if (dp == NULL)
    {
        if (mkdir(logfilepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
        {
            LOG_FATAL("create base dir error, %s , errno: %d, %s", logfilepath, errno, strerror(errno));
            return 1;
        }
    }
    closedir(dp);

    logFileFullPath = logfilepath;
#endif

    const char *logfilename = config.getConfigName("logfilename");
    logFileFullPath += logfilename;

    CAsyncLog::init(logFileFullPath.c_str());

    const char *filecachedir = config.getConfigName("filecachedir");
    Singleton<FileManager>::Instance().init(filecachedir); // 其实就是建了一个目录

    const char *listenip = config.getConfigName("listenip");
    short listenport = (short)atol(config.getConfigName("listenport"));
    Singleton<FileServer>::Instance().init(listenip, listenport, &g_mainLoop, filecachedir);

    LOG_INFO("fileserver initialization completed, now you can use client to connect it.");

    g_mainLoop.loop();

    LOG_INFO("exit fileserver.");

    return 0;
}
