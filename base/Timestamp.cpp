/*
 *  Filename:   Timestamp.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *              2025-06-16
 *  Description:Encapsulates a timestamp
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdio.h>

#include "Timestamp.h"

static_assert(sizeof(Timestamp) == sizeof(int64_t), "sizeof Timestamp error");

Timestamp::Timestamp() : microSecondsSinceEpoch_(0)
{
}

Timestamp::Timestamp(int64_t t) : microSecondsSinceEpoch_(t)
{
}

Timestamp &Timestamp::operator+=(Timestamp &t)
{
    this->microSecondsSinceEpoch_ += t.microSecondsSinceEpoch_;
    return *this;
}

Timestamp &Timestamp::operator-=(Timestamp &t)
{
    this->microSecondsSinceEpoch_ -= t.microSecondsSinceEpoch_;
    return *this;
}

Timestamp &Timestamp::operator+=(int64_t t)
{
    this->microSecondsSinceEpoch_ += t;
    return *this;
}

Timestamp &Timestamp::operator-=(int64_t t)
{
    this->microSecondsSinceEpoch_ -= t;
    return *this;
}

void Timestamp::swap(Timestamp &t)
{
    std::swap(this->microSecondsSinceEpoch_, t.microSecondsSinceEpoch_);
}

std::string Timestamp::toString() const
{
    char buf[64] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / MicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % MicroSecondsPerSecond;
    snprintf(buf, sizeof(buf) - 1,
             "%lld.%06lld",
             (long long int)seconds, (long long int)microseconds);
    return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / MicroSecondsPerSecond);
    struct tm tm_time;

#if defined WIN32
    localtime_s(&tm_time, &seconds);
#else
    struct tm *ptm;
    ptm = localtime(&seconds);
    tm_time = *ptm;
#endif

    char buf[32] = {0};
    if (showMicroseconds)
    {
        int microseconds = static_cast<int64_t>(microSecondsSinceEpoch_ % MicroSecondsPerSecond);
#ifdef WIN32
        _snprintf_s(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                    microseconds);
#else
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microseconds);
#endif
    }
    else
    {
#ifdef WIN32
        _snprintf_s(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#else
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif
    }
    return buf;
}

bool Timestamp::valid() const
{
    return microSecondsSinceEpoch_ > 0;
}

int64_t Timestamp::microSecondsSinceEpoch() const
{
    return microSecondsSinceEpoch_;
}

time_t Timestamp::secondSinceEpoch() const
{
    return static_cast<time_t>(microSecondsSinceEpoch_ / MicroSecondsPerSecond);
}

Timestamp Timestamp::now()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> now =
        std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    int64_t microSeconds = now.time_since_epoch().count();
    Timestamp time(microSeconds);
    return time;
}

Timestamp Timestamp::invalid()
{
    return Timestamp();
}