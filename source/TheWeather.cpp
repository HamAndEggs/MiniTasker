

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

void TheWeather::Update(const std::time_t pCurrentTime)
{
   if( mFetchLimiter < pCurrentTime )
    {
        mHasWeather = false;
        mWeather.Get(50.72824,-1.15244,[this,pCurrentTime](bool pDownloadedOk,const getweather::TheWeather &pTheWeather)
        {
            if( pDownloadedOk )
            {
                std::clog << "Fetched weather data " << pTheWeather.mCurrent.mTime.GetDate() << " " << pTheWeather.mCurrent.mTime.GetTime() << "\n";
                mHasWeather = true;

                // It worked, do the next fetch in a days time.
                mFetchLimiter = pCurrentTime + (60*60*24);
            }
            else
            {
                const getweather::WeatherTime now(pCurrentTime);
                std::cerr << "Failed to fetched weather data! " << now.GetDate() << " " << now.GetTime() << "\n";
                mCurrentTemperature = "Failed";
                mFetchLimiter = pCurrentTime + (60*60);// If it fails, this will make the next attempt an hour later.
            }
        });
    }

    if( mHasWeather )
    {
        // Not building for C++20 so can't use std::format yet.... So go old school.
        char buf[64];

        float c = 0.0f;

        const auto t = mWeather.GetHourlyForcast(pCurrentTime);
        snprintf(buf,sizeof(buf),"%04.2fC",t->mTemperature.c);
        mCurrentTemperature = buf;
    }

}
