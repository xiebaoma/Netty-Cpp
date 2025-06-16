/*
 *  Filename:   ConfigFileReaderTest.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *  Description:测试一个读取配置文件类
 */

#include <iostream>
#include "../base/ConfigFileReader.h"

int main()
{
    // 创建配置读取器
    CConfigFileReader configReader("test.conf");

    // 读取现有配置项
    const char *keys[] = {"ip", "port", "mode", "nonexistent"};
    for (const char *key : keys)
    {
        char *value = configReader.getConfigName(key);
        if (value)
        {
            std::cout << key << " = " << value << std::endl;
        }
        else
        {
            std::cout << key << "can't find" << std::endl;
        }
    }

    std::cout << "\nchange config..." << std::endl;

    // 设置新的配置项或修改旧的
    configReader.setConfigValue("port", "8080");
    configReader.setConfigValue("log_level", "info");
    configReader.setConfigValue("new_key", "new_value");

    std::cout << "\nafter change" << std::endl;
    const char *new_keys[] = {"port", "log_level", "new_key"};
    for (const char *key : new_keys)
    {
        char *value = configReader.getConfigName(key);
        if (value)
        {
            std::cout << key << " = " << value << std::endl;
        }
    }

    return 0;
}
