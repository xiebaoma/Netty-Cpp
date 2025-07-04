/*
 *  Filename:   Singleton.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *  Description:Template class for singleton mode
 */

#pragma once

template <typename T>
class Singleton
{
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }

    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};