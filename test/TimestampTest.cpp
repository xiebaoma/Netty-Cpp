/*
 *  Filename:   TimestampTest.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *  Description:test timestamp
 */

#include <iostream>
#include "../base/Timestamp.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
void sleep_ms(int ms)
{
    Sleep(ms);
}
#else
#include <unistd.h>
void sleep_ms(int ms)
{
    usleep(ms * 1000);
}
#endif

void testConstructors()
{
    Timestamp t1;
    Timestamp t2(1000000);

    std::cout << "t1 (default): " << t1.toString() << std::endl;
    std::cout << "t2 (1s):      " << t2.toString() << std::endl;
}

void testArithmetic()
{
    Timestamp t1(2000000);
    Timestamp t2(500000);

    t1 += t2;
    std::cout << "t1 += t2: " << t1.toString() << std::endl;

    t1 -= t2;
    std::cout << "t1 -= t2: " << t1.toString() << std::endl;

    t1 += 1000000;
    std::cout << "t1 += 1s: " << t1.toString() << std::endl;

    t1 -= 1500000;
    std::cout << "t1 -= 1.5s: " << t1.toString() << std::endl;
}

void testStaticNow()
{
    Timestamp now = Timestamp::now();
    std::cout << "now: " << now.toString() << std::endl;

    sleep_ms(100); // 改为平台兼容的 sleep

    Timestamp later = Timestamp::now();
    std::cout << "later: " << later.toString() << std::endl;

    std::cout << "elapsed microseconds: "
              << later.microSecondsSinceEpoch() - now.microSecondsSinceEpoch()
              << std::endl;
}

void testValidAndInvalid()
{
    Timestamp valid = Timestamp::now();
    Timestamp invalid = Timestamp::invalid();

    std::cout << "valid?   " << (valid.valid() ? "yes" : "no") << std::endl;
    std::cout << "invalid? " << (invalid.valid() ? "yes" : "no") << std::endl;
}

void testSwap()
{
    Timestamp t1(1000000);
    Timestamp t2(3000000);

    std::cout << "Before swap:\nt1 = " << t1.toString() << "\nt2 = " << t2.toString() << std::endl;

    t1.swap(t2);

    std::cout << "After swap:\nt1 = " << t1.toString() << "\nt2 = " << t2.toString() << std::endl;
}

void testFormattedString()
{
    Timestamp now = Timestamp::now();
    std::cout << "formatted (default): " << now.toFormattedString() << std::endl;
    std::cout << "formatted (no usec): " << now.toFormattedString(false) << std::endl;
}

int main()
{
    std::cout << "=== Constructor Tests ===\n";
    testConstructors();

    std::cout << "\n=== Arithmetic Tests ===\n";
    testArithmetic();

    std::cout << "\n=== Static now() Test ===\n";
    testStaticNow();

    std::cout << "\n=== Validity Check Test ===\n";
    testValidAndInvalid();

    std::cout << "\n=== Swap Test ===\n";
    testSwap();

    std::cout << "\n=== toFormattedString Test ===\n";
    testFormattedString();

    return 0;
}
