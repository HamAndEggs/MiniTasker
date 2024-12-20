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

#ifndef DISPLAY_WEATHER_H
#define DISPLAY_WEATHER_H

#include "Graphics.h"
#include "Element.h"
#include "../OpenMeteoFetch/open-meteo.h"

#include <ctime>
#include <string>
#include <map>

#include <vector>

class WeatherIcon;

class DisplayWeather : public eui::Element
{
public:

    DisplayWeather(eui::Graphics* graphics,const std::string& pPath,int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS,bool pDayDisplay);
    ~DisplayWeather();

    virtual bool OnUpdate(const eui::Rectangle& pContentRect);
    bool GetIsDay()const{return mIsDay;}

private:
    WeatherIcon* icons[4];
    bool mIsDay = true;
    int tick = 0;
    float anim = 0;
    std::vector<openmeteo::Hourly>mForcast;

    std::map<std::string,uint32_t>WeatherIcons;

    const std::vector<std::string> files =
    {
        "01d",
        "01n",
        "02d",
        "02n",
        "03d",
        "03n",
        "04d",
        "04n",
        "09d",
        "09n",
        "10d",
        "10n",
        "11d",
        "11n",
        "13d",
        "13n",
        "50d",
        "50n",
    };

    bool mHasWeather;
    bool mFirstFail; //!< Sometimes just after boot the fetch fails. Normally if it does I wait an hour before trying again. But for the first time will try in one minutes time.
    std::time_t mFetchLimiter;
    std::time_t mHourlyUpdates;

    std::string mCurrentTemperature;

    void LoadWeatherIcons(eui::Graphics* graphics,const std::string& pPath);
    uint32_t GetRandomIcon();
    bool LoadWeather();

    bool GetForcast(std::time_t theTime,openmeteo::Hourly& found)const;
};

#endif //#ifndef DISPLAY_WEATHER_H
