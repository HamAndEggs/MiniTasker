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

#include "TinyGLES.h"
#include "TheWeather.h"
#include "Icons.h"
#include "sgp30.h"

class DisplayAirQuality
{
public:
    DisplayAirQuality(tinygles::GLES& pGL,const std::string& pFontPath);
    ~DisplayAirQuality() = default;

    void Update(int pX,int pY);

private:
    const uint32_t mFont = 0;
    tinygles::GLES& GL;
    i2c::SGP30 indoorAirQuality;
    int mResult = i2c::SGP30::READING_RESULT_WARM_UP;
    uint16_t mECO2 = 0;
    uint16_t mTVOC = 0;
};

#endif //#ifndef DISPLAY_WEATHER_H
