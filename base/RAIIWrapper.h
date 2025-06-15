/*
 *  Filename:   RAIIWrapper.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *  Description:template class to implement RAII
 */

#pragma once

template <typename T>
class RAIIWrapper
{
public:
    RAIIWrapper(T *p) : ptr(p)
    {
    }

    virtual ~RAIIWrapper()
    {
        if (ptr != nullptr)
        {
            delet ptr;
            ptr = nullptr;
        }
    }

private:
    T *ptr;
};