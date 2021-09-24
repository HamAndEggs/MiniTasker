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

#include "TinyGLES.h"
#include "TheWeather.h"
#include "Icons.h"

#include <vector>

class DisplayWeather
{
public:
    DisplayWeather(tinygles::GLES& pGL,const std::string& pIconFolder);
    ~DisplayWeather() = default;

    /**
     * @brief Draws a set of icons to get a brief over view of the weather coming up.
     * 
     * @param pFB 
     * @param pX 
     * @param pY 
     */
    void RenderWeatherForcast(int pY,const tm& pCurrentTime,const TheWeather& pWeather,const Icons& pTheIcons);

    /**
     * @brief Renders a box with the temprature in it.
     * 
     * @param pY 
     * @param pTemperature
     * @param pIsOnline True if the data is valid, false if it's a bit too old. Just changes the render colour.
     */
    int RenderTemperature(int pX,int pY,const std::string& pTemperature,bool pIsOnline = true);

private:
    const uint32_t mIconFont = 0;
    const uint32_t mTemperatureFont = 0;
    tinygles::GLES& GL;

};

#endif //#ifndef DISPLAY_WEATHER_H
