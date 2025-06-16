/*
 *  Filename:   Timestamp.h
 *  Author:     xiebaoma
 *  Date:       2025-06-15
 *  Description:Encapsulates a timestamp
 */

#pragma once

#include <algorithm>
#include <string>

static const int MicroSecondsPerSecond = 1000 * 1000;

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t t);

    Timestamp &operator+=(Timestamp &t);
    Timestamp &operator-=(Timestamp &t);
    Timestamp &operator+=(int64_t t);
    Timestamp &operator-=(int64_t t);
    void swap(Timestamp &t);

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const;
    int64_t microSecondsSinceEpoch() const;
    time_t secondSinceEpoch() const;

    static Timestamp now();
    static Timestamp invalid();

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() < r.microSecondsSinceEpoch();
}

inline bool operator>(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() > r.microSecondsSinceEpoch();
}

inline bool operator>=(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() >= r.microSecondsSinceEpoch();
}

inline bool operator<=(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() <= r.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() == r.microSecondsSinceEpoch();
}

inline bool operator!=(Timestamp &l, Timestamp &r)
{
    return l.microSecondsSinceEpoch() != r.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp &l, Timestamp &r)
{
    int64_t diff = l.microSecondsSinceEpoch() - r.microSecondsSinceEpoch();
    return static_cast<double>(diff / MicroSecondsPerSecond);
}

inline Timestamp addTime(Timestamp &t, int64_t ms)
{
    return Timestamp(t.microSecondsSinceEpoch() + ms);
}
