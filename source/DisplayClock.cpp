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
#include "style.h"

#include <time.h>
#include <chrono>
#include <string>
#include <array>

DisplayClock::DisplayClock(int pBigFont,int pNormalFont,int pMiniFont)
{
    this->SetID("clock");
    this->SetPos(0,0);
    this->SetSpan(2,1);
    
    this->SetPadding(CELL_PADDING);

    clock = new eui::Element;
        clock->SetPadding(0.05f);
        clock->GetStyle().mAlignment = eui::ALIGN_CENTER_TOP;
        clock->GetStyle().mFont = (pBigFont);
    this->Attach(clock);

    dayName = new eui::Element;
        dayName->SetPadding(0.05f);
        dayName->GetStyle().mAlignment = eui::ALIGN_LEFT_BOTTOM;
        dayName->GetStyle().mFont = (pNormalFont);
    this->Attach(dayName);

    dayNumber = new eui::Element;
        dayNumber->SetPadding(0.05f);
        dayNumber->GetStyle().mAlignment = eui::ALIGN_RIGHT_BOTTOM;
        dayNumber->GetStyle().mFont = (pNormalFont);
    this->Attach(dayNumber);

}

bool DisplayClock::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( dayDisplay )
    {
        eui::Style s;
        s.mBackground = eui::COLOUR_BLACK;
        s.mThickness = BORDER_SIZE;
        s.mBorder = eui::COLOUR_WHITE;
        s.mRadius = 0.1f;
        this->SetStyle(s);

        clock->GetStyle().mForeground = eui::COLOUR_WHITE;
        dayName->GetStyle().mForeground = eui::COLOUR_WHITE;
        dayNumber->GetStyle().mForeground = eui::COLOUR_WHITE;
    }
    else
    {
        eui::Style s;
        s.mBackground = eui::COLOUR_BLACK;
        s.mThickness = BORDER_SIZE;
        s.mBorder = eui::COLOUR_DARK_GREY;
        s.mRadius = 0.1f;
        this->SetStyle(s);

        clock->GetStyle().mForeground = eui::COLOUR_GREY;
        dayName->GetStyle().mForeground = eui::COLOUR_GREY;
        dayNumber->GetStyle().mForeground = eui::COLOUR_GREY;
    }

    std::time_t result = std::time(nullptr);
    tm *currentTime = localtime(&result);
    if( currentTime )
    {
        clock->SetTextF("%02d:%02d",currentTime->tm_hour,currentTime->tm_min);

        std::array<const char*,7> Days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
        dayName->SetText(Days[currentTime->tm_wday]);

        // Do month day.
        const char* monthDayTag = "th";
        const int monthDay = currentTime->tm_mday;
        if( monthDay == 1 || monthDay == 21 || monthDay == 31 )
        {
            monthDayTag = "st";
        }
        else if( monthDay == 2 || monthDay == 22 )
        {
            monthDayTag = "nd";
        }
        else if( monthDay == 3 || monthDay == 23 )
        {
            monthDayTag = "rd";
        }

        dayNumber->SetTextF("%d%s",monthDay,monthDayTag);
    }
    else
    {
        clock->SetText("NO CLOCK");
        dayName->SetText("NO DAY");
        dayNumber->SetText("NO DAY");
    }
    return true;
}
