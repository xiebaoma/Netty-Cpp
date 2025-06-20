#include "FileManager.h"
/*
 *  Filename:   FileManager.h
 *  Author:     xiebaoma
 *  Date:       2025-06-20
 *  Description:文件管理类
 */

#include <string>

#include "FileManager.h"
#include "../base/AsyncLog.h"
#include "../base/Platform.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

bool FileManager::init(const char *basepath)
{
    m_basepath = basepath;
#ifdef _WIN32
    if (!PathFileExistsA(basepath))
    {
        // 可以尝试 SHCreateDirectoryExA 自动创建多层目录
        if (SHCreateDirectoryExA(NULL, basepath, NULL) != ERROR_SUCCESS)
        {
            DWORD err = GetLastError();
            LOG_ERROR("Failed to create base directory: %s, error code: %lu", basepath, err);
            return false;
        }
        else
        {
            LOG_INFO("Directory created: %s", basepath);
        }
    }
#else
    DIR *dp = opendir(basepath);
    if (dp == NULL)
    {
        if (errno == ENOENT)
        {
            if (mkdir(basepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
            {
                LOG_INFO("Created directory: %s", basepath);
                return true;
            }
            else
            {
                LOG_ERROR("mkdir failed, %s , errno: %d, %s", basepath, errno, strerror(errno));
                return false;
            }
        }
        else
        {
            LOG_ERROR("opendir failed, errno: %d, %s", errno, strerror(errno));
            return false;
        }
    }

    // 清空旧文件列表
    m_listFiles.clear();

    struct dirent *dirp;
    while ((dirp = readdir(dp)) != NULL)
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;

        m_listFiles.emplace_back(dirp->d_name);
        LOG_INFO("filename: %s", dirp->d_name);
    }

    closedir(dp);
#endif
    return true;
}

bool FileManager::isFileExsit(const char *filename)
{
    std::lock_guard<std::mutex> guard(m_mtFile);
    // 先查看缓存
    for (const auto &iter : m_listFiles)
    {
        if (iter == filename)
            return true;
    }

    // 再查看文件系统
    std::string filepath = m_basepath;
    filepath += filename;
    FILE *fp = fopen(filepath.c_str(), "r");
    if (fp != NULL)
    {
        fclose(fp);
        m_listFiles.emplace_back(filename);
        return true;
    }

    return false;
}

void FileManager::addFile(const char *filename)
{
    std::lock_guard<std::mutex> guard(m_mtFile);
    m_listFiles.emplace_back(filename);
}