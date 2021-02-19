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
   
#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include <string>
#include <time.h>

#include "framebuffer.h"

class ClockDisplay
{
public:
    ClockDisplay(const std::string& pFontPath);
    ~ClockDisplay();

    void SetForground(uint8_t pR,uint8_t pG,uint8_t pB);
    void SetBackground(uint8_t pR,uint8_t pG,uint8_t pB);

    void Update(FBIO::FrameBuffer* pFB,int pX,int pY,const tm& pCurrentTime,const std::string& pCurrentTemperature);

private:

    void DrawTime(FBIO::FrameBuffer* pFB,int pX,int pY,int pHour,int pMinute);
    void DrawDay(FBIO::FrameBuffer* pFB,int pX,int pY,int pWeekDay,int pMonthDay);

    FBIO::FreeTypeFont mTimeFont;
    FBIO::FreeTypeFont mDateFont;
    FBIO::FreeTypeFont mTemperatureFont;    

	struct
	{
		uint8_t r,g,b;
	}mFG,mBG;
};

#endif //#ifndef CLOCK_DISPLAY_H