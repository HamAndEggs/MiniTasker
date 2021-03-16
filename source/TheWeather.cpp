/*
   Copyright (C) 2017, Richard e Collins.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#include <string>
#include <array>
#include <iostream>
#include <cmath>

#include "TinyTools.h"
#include "TheWeather.h"

const std::time_t ONE_MINUTE = (60);
const std::time_t ONE_HOUR = (ONE_MINUTE * 60);
const std::time_t ONE_DAY = (ONE_HOUR*24);

static const std::time_t RoundToHour(const std::time_t pTime)
{
    return pTime - (pTime%ONE_HOUR);
}

static const std::time_t NextHour(const std::time_t pTime)
{
    return RoundToHour(pTime) + ONE_HOUR;
}

// Mainly for debugging.
static std::string GetTimeString(const std::time_t pTime)
{
    tm currentTime = *localtime((time_t*)&pTime);
	return std::to_string(currentTime.tm_hour) + ":" + std::to_string(currentTime.tm_min) + ":" + std::to_string(currentTime.tm_sec);
}

TheWeather::TheWeather(const std::string& pWeatherApiKey):
    mHasWeather(false),
    mFetchLimiter(0),
    mWeather(pWeatherApiKey),
    mHourlyUpdates(0)
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
                mFetchLimiter = pCurrentTime + ONE_DAY;
                // Now round to start of day plus one minute to be safe, 00:01. The weather forcast may have changed. Also if we boot in the evening don't want all downloads at the same time every day.
                mFetchLimiter -= (mFetchLimiter%ONE_DAY);
                mFetchLimiter += ONE_MINUTE;

                const getweather::WeatherTime nextDownload(mFetchLimiter);
                std::clog << "Next download scheduled for " << nextDownload.GetDate() << " " << nextDownload.GetTime() << "\n";

                // Reset this time out to force a rebuild.
                mHourlyUpdates = 0;
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
        // Rebuild the Next Hourly Icons vector so its always correct an hour after the last time.
        if( mHourlyUpdates < pCurrentTime )
        {
            std::clog << "Updating icons " << GetTimeString(mHourlyUpdates) << "\n";
            // Update in an hour and on the hour.
            mHourlyUpdates = NextHour(pCurrentTime);
            std::clog << "Next icon update at " << GetTimeString(mHourlyUpdates) << "\n";

            mNextHourlyIcons = mWeather.GetHourlyIconCodes(pCurrentTime);

            // Not building for C++20 so can't use std::format yet.... So go old school.
            char buf[64];
            const auto t = mWeather.GetHourlyForcast(pCurrentTime);

            snprintf(buf,sizeof(buf),"%03.1fC",tinytools::math::RoundToPointFive(t->mTemperature.c));
            mCurrentTemperature = buf;
        }
    }

}
