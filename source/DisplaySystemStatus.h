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
   
#ifndef DISPLAY_SYSTEM_STATUS_H
#define DISPLAY_SYSTEM_STATUS_H

#include "Graphics.h"
#include "Element.h"
#include "TinyTools.h"

#include <string>
#include <time.h>

class DisplaySystemStatus : public eui::Element
{
public:

    DisplaySystemStatus(int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS,bool pDayDisplay);
    virtual bool OnUpdate(const eui::Rectangle& pContentRect);

private:
    eui::ElementPtr uptime;
    eui::ElementPtr localIP;
    eui::ElementPtr hostName;
    eui::ElementPtr cpuLoad;
    eui::ElementPtr ramUsed;

    std::map<int,tinytools::system::CPULoadTracking> trackingData;

};

#endif //#ifndef DISPLAY_SYSTEM_STATUS_H