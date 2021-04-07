/*
   Copyright (C) 2021, Richard e Collins.

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
   
#ifndef THE_WEATHER_H
#define THE_WEATHER_H

#include "TinyWeather.h"

#include <ctime>
#include <string>
#include <map>

class TheWeather
{
public:
    TheWeather(const std::string& pWeatherApiKey);
    ~TheWeather();

    bool GetHasWeather()const{return mHasWeather;}
    std::string GetCurrentTemperature()const{return mCurrentTemperature;}

    // List of the hourly icons from and including now but not before.
    tinyweather::HourlyIconVector GetNextHourlyIcons()const{return mNextHourlyIcons;}

    void Update(const std::time_t pCurrentTime);

private:
    bool mHasWeather;
    bool mFirstFail; //!< Sometimes just after boot the fetch fails. Normally if it does I wait an hour before trying again. But for the first time will try in one minutes time.
    std::time_t mFetchLimiter;
    std::time_t mHourlyUpdates;
    tinyweather::OpenWeatherMap mWeather;

    std::string mCurrentTemperature;

    tinyweather::HourlyIconVector mNextHourlyIcons;
};

#endif //#ifndef THE_WEATHER_H