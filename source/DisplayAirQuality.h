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

#ifndef DISPLAY_AIR_QUALITY_H
#define DISPLAY_AIR_QUALITY_H

#include "Graphics.h"
#include "Element.h"
#include "sgp30.h"
#include "MQTTData.h"

#include <string>
#include <map>

class DisplayAirQuality : public eui::Element
{
public:

    DisplayAirQuality(int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS);
    ~DisplayAirQuality();

    virtual bool OnUpdate();

private:

    eui::ElementPtr eCO2,tOC,outsideTemp;
    i2c::SGP30 indoorAirQuality;
    uint16_t mECO2 = 0;
    uint16_t mTVOC = 0;
    int mResult = i2c::SGP30::READING_RESULT_WARM_UP;
    std::map<std::string,std::string> outsideData;
    std::time_t outsideTemperatureDelivered = 0;

    // MQTT data
    const std::vector<std::string>& topics = {"/outside/temperature","/outside/hartbeat"};
    MQTTData OutsideWeather;

};

#endif //#ifndef DISPLAY_WEATHER_H
