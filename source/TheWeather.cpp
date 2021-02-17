

#include <string>
#include <array>
#include <iostream>

#include "TheWeather.h"

TheWeather::TheWeather(const std::string& pWeatherApiKey):
    mHasWeather(false),
    mFetchingWeather(false),
    mWeather(pWeatherApiKey)
{

}

TheWeather::~TheWeather()
{
    if( mFetchThread.joinable() )
    {
        mFetchThread.join();
    }
}

void TheWeather::Update()
{
    if( mHasWeather == false )
    {
        if( mFetchingWeather == false )
        {
            mFetchingWeather = true;
            mFetchThread = std::thread([this]()
            {
                mWeather.Get(50.72824,-1.15244,[this](const getweather::TheWeather &pTheWeather)
                {
                    std::time_t result = std::time(nullptr);
                    mLastFetchTime = *localtime(&result);

                    // Not building for C++20 so can't use std::format yet.... So go old school.
                    char buf[64];

                    snprintf(buf,sizeof(buf),"%04.2fC",pTheWeather.mCurrent.mTemperature.Day.c);
                    mCurrentTemperature = buf;
                    mHasWeather = true;
                    mFetchingWeather = false;
                });
            });
        }
    }
    else // We have some weather, so see if it needs to be fetched again.
    {
        // See if day has changed.
        std::time_t result = std::time(nullptr);
        tm now = *localtime(&result);

        if( mLastFetchTime.tm_yday != now.tm_yday )
        {
            std::cout << "fetching new weather data\n";
            mLastFetchTime = now;
            mHasWeather = false;
        }

    }
}
