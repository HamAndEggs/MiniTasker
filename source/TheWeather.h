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

#include <thread>
#include <ctime>
#include <string>

#include "GetWeather.h"

class TheWeather
{
public:
    TheWeather(const std::string& pWeatherApiKey);
    ~TheWeather();

    bool GetHasWeather()const{return mHasWeather;}
    std::string GetCurrentTemperature()const{return mCurrentTemperature;}

    void Update();

private:
    bool mHasWeather;
    bool mFetchingWeather;
    tm mLastFetchTime;
    std::thread mFetchThread;
    getweather::GetWeather mWeather;

    std::string mCurrentTemperature;

};

#endif //#ifndef THE_WEATHER_H