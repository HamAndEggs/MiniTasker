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

#include "TinyGLES.h"

#include <string>
#include <time.h>

class DisplayClock
{
public:
    DisplayClock(tinygles::GLES& pGL,const std::string& pFontPath);
    ~DisplayClock();

    void SetForground(uint8_t pR,uint8_t pG,uint8_t pB);
    void Update(int pX,int pY,const tm& pCurrentTime);

private:

    void DrawTime(int pX,int pY,int pHour,int pMinute);
    void DrawDay(int pX,int pY,int pWeekDay,int pMonthDay);
    
    const uint32_t mTimeFont = 0;
    const uint32_t mDateFont = 0;

    tinygles::GLES& GL;

	struct
	{
		uint8_t r,g,b;
	}mFG,mBG;
};

#endif //#ifndef DISPLAY_CLOCK_H