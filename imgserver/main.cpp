/*
 *  Filename:   main.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-23
 *  Description:imgserver程序的入口，使用了和fileserver完全相同的代码，有待斟酌
 */
#include <iostream>
#include <stdlib.h>

#include "../base/Platform.h"
#include "../base/Singleton.h"
#include "../base/ConfigFileReader.h"
#include "../base/AsyncLog.h"
#include "../net/EventLoop.h"
#include "../fileserversrc/FileManager.h"
#include "../fileserversrc/FileServer.h"

#ifndef _WIN32
#include <string.h>
#include "../utils/DaemonRun.h"
#endif

using namespace net;

#ifdef _WIN32
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
    CConfigFileReader config("../etc/imgserver.conf");
#else
    CConfigFileReader config("etc/imgserver.conf");
#endif

    std::string logFileFullPath;

#ifndef _WIN32
    const char *logfilepath = config.getConfigName("logfiledir");
    if (logfilepath == NULL)
    {
        LOG_FATAL("logdir is not set in config file");
        return 1;
    }

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

    const char *filecachedir = config.getConfigName("imgcachedir");
    Singleton<FileManager>::Instance().init(filecachedir);

    const char *listenip = config.getConfigName("listenip");
    short listenport = (short)atol(config.getConfigName("listenport"));
    Singleton<FileServer>::Instance().init(listenip, listenport, &g_mainLoop, filecachedir);

    LOG_INFO("imgserver initialization complete, now you can use client to connect it.");

    g_mainLoop.loop();

    LOG_INFO("exit imgserver.");

    return 0;
}
