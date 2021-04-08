/*
   Copyright (C) 2017, Richard e Collins.

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

#include "DisplayClock.h"

#include <string>
#include <array>


DisplayClock::DisplayClock(tinygles::GLES& pGL,const std::string& pFontPath):
    mTimeFont( pGL.FontLoad( pFontPath + "LiberationSerif-Bold.ttf",160)),
    mDateFont( pGL.FontLoad( pFontPath + "LiberationSerif-Regular.ttf",52)),
    GL(pGL)
{
}

DisplayClock::~DisplayClock()
{

}

void DisplayClock::SetForground(uint8_t pR,uint8_t pG,uint8_t pB)
{
    mFG.r = pR;
    mFG.g = pG;
    mFG.b = pB;
    GL.FontSetColour(mTimeFont,pR,pG,pB);
    GL.FontSetColour(mDateFont,pR,pG,pB);
}

void DisplayClock::Update(int pX,int pY,const tm& pCurrentTime)
{
    GL.FillRoundedRectangle(pX-8,pY-8,pX + 400,pY + 230,35,255,255,255);
    GL.FillRoundedRectangle(pX-4,pY-4,pX + 400-4,pY + 230-4,32,0,0,0);

    DrawTime(pX,pY,pCurrentTime.tm_hour,pCurrentTime.tm_min);
    DrawDay(pX + 8,pY + 140,pCurrentTime.tm_wday,pCurrentTime.tm_mday);
}

void DisplayClock::DrawTime(int pX,int pY,int pHour,int pMinute)
{
    GL.FontPrintf(mTimeFont,pX + 4,pY + 120,"%02d:%02d",pHour,pMinute);
}

void DisplayClock::DrawDay(int pX,int pY,int pWeekDay,int pMonthDay)
{
    static const std::array<std::string,7> Days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

    GL.FontPrint(mDateFont,pX + 4,pY + 44,Days[pWeekDay].c_str());

    // Do month day.
    std::string monthDayTag = "th";
    if( pMonthDay == 1 || pMonthDay == 21 || pMonthDay == 31 )
    {
        monthDayTag = "st";
    }
    else if( pMonthDay == 2 || pMonthDay == 22 )
    {
        monthDayTag = "nd";
    }
    else if( pMonthDay == 3 || pMonthDay == 23 )
    {
        monthDayTag = "rd";
    }

    GL.FontPrintf(mDateFont,pX + 272,pY + 44,"%d%s",pMonthDay,monthDayTag.c_str());
}
