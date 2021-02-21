

#include <string>
#include <array>
#include <iostream>

#include "TheWeather.h"

TheWeather::TheWeather(const std::string& pWeatherApiKey):
    mHasWeather(false),
    mFetchLimiter(0),
    mWeather(pWeatherApiKey)
{

}

TheWeather::~TheWeather()
{
}

void TheWeather::Update(const tm& pCurrentTime)
{
    if( mHasWeather == false )
    {
        // Because when time is 11:59 the fetch limiter could be set to a big number and when the day changes next minute
        // will never be less that current minute of the day.
        // So I also check the day too, again. Done a second time, once, at the bottom, to see if we need a new days time.
        if( mFetchLimiter < (pCurrentTime.tm_hour*60) + pCurrentTime.tm_min || mLastFetchTime.tm_yday != pCurrentTime.tm_yday )
        {
            mLastFetchTime = pCurrentTime;
            mFetchLimiter = (pCurrentTime.tm_hour*60) + pCurrentTime.tm_min;
            mWeather.Get(50.72824,-1.15244,[this](bool pDownloadedOk,const getweather::TheWeather &pTheWeather)
            {
                if( pDownloadedOk )
                {
                    std::clog << "Fetched weather data " << mLastFetchTime.tm_mday << " " << mLastFetchTime.tm_hour << ":" << mLastFetchTime.tm_min << ":" << mLastFetchTime.tm_sec << "\n";

                    // Not building for C++20 so can't use std::format yet.... So go old school.
                    char buf[64];

                    snprintf(buf,sizeof(buf),"%04.2fC",pTheWeather.mCurrent.mTemperature.Day.c);
                    mCurrentTemperature = buf;
                    mHasWeather = true;
                }
                else
                {
                    std::cerr << "Failed to fetched weather data! " << mLastFetchTime.tm_mday << " " << mLastFetchTime.tm_hour << ":" << mLastFetchTime.tm_min << ":" << mLastFetchTime.tm_sec << "\n";
                    mCurrentTemperature = "Failed";

                }
            });
        }
    }
    else // We have some weather, so see if it needs to be fetched again.
    {
        if( mLastFetchTime.tm_yday != pCurrentTime.tm_yday )
        {
            mHasWeather = false;
        }
    }
}
