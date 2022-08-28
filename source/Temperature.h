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
   
#ifndef Temperature_H
#define Temperature_H

#include "Graphics.h"
#include "Element.h"
#include <chrono>

class MQTTData;
class Temperature : public eui::Element
{
public:

    Temperature(int pFont,const eui::Style &pStyle,float CELL_PADDING);
    virtual bool OnDraw(eui::Graphics* pGraphics,const eui::Rectangle& pContentRect);

private:
    MQTTData* mOutsideWeather = nullptr;
    std::map<std::string,std::string> mOutsideData;
    std::chrono::time_point<std::chrono::system_clock> mOutsideTemperatureDelivered = std::chrono::system_clock::now();
    uint32_t mOutsideTemperatureUpdateSeconds = 0;

};


#endif //#ifndef Temperature_H