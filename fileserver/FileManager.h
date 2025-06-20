/*
 *  Filename:   FileManager.h
 *  Author:     xiebaoma
 *  Date:       2025-06-20
 *  Description:文件管理类
 */

#pragma once

#include <string>
#include <string>
#include <list>
#include <mutex>

class FileManager final
{
public:
    FileManager();
    ~FileManager();

    FileManager(const FileManager &rhs) = delete;
    FileManager &operator=(const FileManager &ths) = delete;

    bool init(const char *basepath);
    bool isFileExsit(const char *filename);
    void addFile(const char *filename);

private:
    std::list<std::string> m_listFiles;
    std::mutex m_mtFile;
    std::string m_basepath;
};