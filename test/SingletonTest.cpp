/*
 *  Filename:   SingletonTest.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *              2025-06-17
 *  Description:test singleton mode
 *  command:    g++ SingletonTest.cpp -o test
 */

#include <iostream>

#include "../base/Singleton.h"

class mytype
{
public:
    int a = 10;
};

int main()
{
    mytype &a = Singleton<mytype>::Instance();
    mytype &b = Singleton<mytype>::Instance();

    if (&a == &b)
    {
        std::cout << "a and b point to the same param";
    }
    else
    {
        std::cout << "a and b point to the different param";
    }
    return 0;
}