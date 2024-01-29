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

    void NewShedTemperature(const std::string pTemperature);
    void NewShedOutSide(const std::string pTemperature);

private:

    struct Data
    {
        std::chrono::time_point<std::chrono::system_clock> lastUpdate;
        bool GetIsOnline()const
        {
            const uint32_t TimeOut = 60 * 30;// 30 Minutes.
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count() < TimeOut;
        }
        std::string temperature;

    }mOutside,mShed;

};


#endif //#ifndef Temperature_H