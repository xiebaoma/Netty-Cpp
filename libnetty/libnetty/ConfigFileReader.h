/*
 *  Filename:   ConfigFileReader.h
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *  Description:一个读取配置文件类，将配置信息存储在一个map中
 */

#pragma once

#include <map>
#include <string>

class CConfigFileReader
{
public:
    CConfigFileReader(const char* filename);
    ~CConfigFileReader();

    char* getConfigName(const char* name);
    int setConfigValue(const char* name, const char* value);

private:
    void loadFile(const char* filename);
    int writeFile();
    void parseLine(char* line);
    char* trimSpace(char* name);

    bool m_load_ok;
    std::map<std::string, std::string> m_config_map;
    std::string m_config_file;
};