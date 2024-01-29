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

#include "Temperature.h"

#include "TinyTools.h"

Temperature::Temperature(int pFont,const eui::Style &pStyle,float CELL_PADDING) : eui::Element(pStyle)
{
    SET_DEFAULT_ID();


    SetPadding(0.05f);
    SetText("100.0C");
    SetPadding(CELL_PADDING);
    SetFont(pFont);
}

bool Temperature::OnDraw(eui::Graphics* pGraphics,const eui::Rectangle& pContentRect)
{
    DrawRectangle(pGraphics,pContentRect,GetStyle());

    const eui::Rectangle textRect = pContentRect.GetScaled(0.8f);

    const int font = GetFont();

    const std::string outside = mOutside.temperature;
    const std::string shed = mShed.temperature;

    eui::Colour outSideColour = GetStyle().mForeground;
    eui::Colour shedColour = GetStyle().mForeground;
    if( mOutside.GetIsOnline() == false )
    {
        outSideColour = eui::COLOUR_RED;
    }

    if( mShed.GetIsOnline() == false )
    {
        shedColour = eui::COLOUR_RED;
    }

    pGraphics->FontPrint(font,textRect,eui::ALIGN_LEFT_CENTER,outSideColour,outside);
    pGraphics->FontPrint(font,textRect,eui::ALIGN_RIGHT_CENTER,shedColour,shed);

    return true;
}

void Temperature::NewShedTemperature(const std::string pTemperature)
{
    mShed.lastUpdate = std::chrono::system_clock::now();
    mShed.temperature = pTemperature;
}

void Temperature::NewShedOutSide(const std::string pTemperature)
{
    mOutside.lastUpdate = std::chrono::system_clock::now();
    mOutside.temperature = pTemperature;
}
