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
   
#ifndef DISPLAY_CLOCK_H
#define DISPLAY_CLOCK_H

#include "Graphics.h"
#include "Element.h"

class DisplayClock : public eui::Element
{
public:

    DisplayClock(int pBigFont,int pNormalFont,int pMiniFont);
    virtual bool OnUpdate(const eui::Rectangle& pContentRect);

private:
    eui::ElementPtr clock = nullptr;
    eui::ElementPtr dayName = nullptr;
    eui::ElementPtr dayNumber = nullptr;

};


#endif //#ifndef DISPLAY_CLOCK_H