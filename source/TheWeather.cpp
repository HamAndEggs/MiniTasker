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

#include "TinyTools.h"
#include "TheWeather.h"

#include <string>
#include <sstream>
#include <array>
#include <iostream>
#include <cmath>

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
    mFirstFail(true),
    mFetchLimiter(0),
    mHourlyUpdates(0),
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
        mWeather.Get(50.72824,-1.15244,[this,pCurrentTime](bool pDownloadedOk,const auto &pTheWeather)
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

                const tinyweather::WeatherTime nextDownload(mFetchLimiter);
                std::clog << "Next download scheduled for " << nextDownload.GetDate() << " " << nextDownload.GetTime() << "\n";

                // Reset this time out to force a rebuild.
                mHourlyUpdates = 0;
            }
            else
            {
                const tinyweather::WeatherTime now(pCurrentTime);
                std::cerr << "Failed to fetched weather data! " << now.GetDate() << " " << now.GetTime() << "\n";
                mCurrentTemperature = "Failed";
                if( mFirstFail )
                {
                    mFirstFail = false;
                    mFetchLimiter = pCurrentTime + 60;// Just booting up, try again in a minute.
                }
                else
                {
                    mFetchLimiter = pCurrentTime + (60*60);// If it fails, this will make the next attempt an hour later.
                }
            }
        });
    }

    if( mHasWeather )
    {
        // Rebuild the Next Hourly Icons vector so its always correct an hour after the last time.
        if( mHourlyUpdates < pCurrentTime )
        {
            std::stringstream log;

            if( mHourlyUpdates == 0 )
                log << "Updating icons after fetching new weather data. ";
            else
                log << "Updating icons " << GetTimeString(mHourlyUpdates) << ". ";

            // Update in an hour and on the hour.
            mHourlyUpdates = NextHour(pCurrentTime);
            log << "Next icon update at " << GetTimeString(mHourlyUpdates) << ". ";

            mNextHourlyIcons = mWeather.GetHourlyIconCodes(pCurrentTime);

            // Not building for C++20 so can't use std::format yet.... So go old school.
            const float temperature = mWeather.GetHourlyTemperature(pCurrentTime);

            char buf[64];
            snprintf(buf,sizeof(buf),"%03.1fC",tinytools::math::RoundToPointFive(temperature));
            mCurrentTemperature = buf;
            log << "This hours temperature is " << buf << " ";

            // Output as one line.
            std::clog << log.str();
        }
    }

}
