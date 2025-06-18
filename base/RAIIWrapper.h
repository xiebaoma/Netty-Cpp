/*
 *  Filename:   RAIIWrapper.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *  Description:template class to implement RAII
 *              这个类不会用到，直接使用unique_ptr
 */

#pragma once

template <typename T>
class RAIIWrapper
{
public:
    explicit RAIIWrapper(T *p) : ptr(p) {}

    ~RAIIWrapper()
    {
        delete ptr;
    }

    T *get() const { return ptr; }
    T *operator->() const { return ptr; }
    T &operator*() const { return *ptr; }

    // 禁用拷贝构造和赋值
    RAIIWrapper(const RAIIWrapper &) = delete;
    RAIIWrapper &operator=(const RAIIWrapper &) = delete;

private:
    T *ptr;
};
