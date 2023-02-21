#define __STDC_WANT_LIB_EXT1__ 1

#include "DateTime.hpp"

#include <time.h>
#include <stdio.h>
#include <ctime>

using namespace std;
using namespace std::chrono;

namespace SoulFab::Base
{
    const int HOUR_SEC = 60 * 60;
    const int DAY_SEC = HOUR_SEC * 24;
    const int DAY_MSEC = DAY_SEC * 1000;
    const double DB_DATETIME_SHIFT = 70.0 * 365.0 + 19.0;

    DateTime DateTime::Now()
    {
        return DateTime();
    }

	DateTime::DateTime()
    {
        this->TimeValue = system_clock::now();
    }

    DateTime::DateTime(double value)
    {
        long long ms = (long long)(value * DAY_MSEC);

        this->TimeValue = system_clock::time_point(milliseconds(ms));
    } 

    DateTime::operator double()
    {
        auto ms = duration_cast<milliseconds>(this->TimeValue.time_since_epoch());

        return double(ms.count()) / DAY_MSEC;
    }

    string DateTime::CTS() const
    {
        char buf[20];
        buf[19] = '\x0';

        time_t t = system_clock::to_time_t(this->TimeValue);
        tm* NowStruct = gmtime(&t);

        snprintf(buf, 19, "%04d%02d%02d%02d%02d%02d"
            , NowStruct->tm_year + 1900
            , NowStruct->tm_mon + 1
            , NowStruct->tm_mday
            , NowStruct->tm_hour
            , NowStruct->tm_min
            , NowStruct->tm_sec
        );

        return string(buf);
    }

    string DateTime::ISOString() const
    {
        char buf[50];
        buf[19] = '\x0';

        time_t t = system_clock::to_time_t(this->TimeValue);
        auto ms = duration_cast<milliseconds>(this->TimeValue.time_since_epoch());
        tm* NowStruct = gmtime(&t);

        snprintf(buf, 49, "%04d-%02d-%02d %02d:%02d:%02d.%03d"
            , NowStruct->tm_year + 1900
            , NowStruct->tm_mon + 1
            , NowStruct->tm_mday
            , NowStruct->tm_hour
            , NowStruct->tm_min
            , NowStruct->tm_sec
            , (int)(ms.count() % 1000)
        );

        return string(buf);
    }

    string DateTime::LocalCTS() const
    {
        return "";
    }

    DateStruct DateTime::getDateStruct() const
    {
        tm ts;

        time_t t = system_clock::to_time_t(this->TimeValue);
        localtime_s(&ts, &t);

        DateStruct ret = { ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday };

        return ret;
    }
    
    TimeStruct DateTime::getTimeStruct() const
    {
        tm ts;

        time_t t = system_clock::to_time_t(this->TimeValue);
        localtime_s(&ts, &t);

        auto ms = duration_cast<milliseconds>(this->TimeValue.time_since_epoch());

        TimeStruct ret = { ts.tm_hour, ts.tm_min, ts.tm_sec, (int)(ms.count() % 1000)};

        return ret;
    }
}