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

#include <string>
#include <time.h>

#include "Tiny2D.h"

class DisplayClock
{
public:
    DisplayClock(const std::string& pFontPath);
    ~DisplayClock();

    void SetForground(uint8_t pR,uint8_t pG,uint8_t pB);
    void SetBackground(uint8_t pR,uint8_t pG,uint8_t pB);

    void Update(tiny2d::DrawBuffer& RT,int pX,int pY,const tm& pCurrentTime);

private:

    void DrawTime(tiny2d::DrawBuffer& RT,int pX,int pY,int pHour,int pMinute);
    void DrawDay(tiny2d::DrawBuffer& RT,int pX,int pY,int pWeekDay,int pMonthDay);

    tiny2d::FreeTypeFont mTimeFont;
    tiny2d::FreeTypeFont mDateFont;

	struct
	{
		uint8_t r,g,b;
	}mFG,mBG;
};

#endif //#ifndef DISPLAY_CLOCK_H