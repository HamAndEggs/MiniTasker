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
#include "style.h"

#include "TinyTools.h"

Temperature::Temperature(int pFont,int pSmallFont,float CELL_PADDING) : mSmallFont(pSmallFont)
{
    SET_DEFAULT_ID();


    SetPadding(0.05f);
    SetPadding(CELL_PADDING);
    GetStyle().mFont = pFont;

    mShed.temperature = "N/A";
    mOutside.temperature = "N/A";
}

bool Temperature::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( dayDisplay )
    {
        eui::Style s;
        s.mBackground = eui::MakeColour(100,255,100);
        s.mThickness = BORDER_SIZE;
        s.mBorder = eui::COLOUR_WHITE;
        s.mRadius = RECT_RADIUS;
        s.mForeground = eui::COLOUR_BLACK;
        SetStyle(s);
    }
    else
    {
        eui::Style s;
        s.mBorder = eui::COLOUR_DARK_GREY;
        s.mThickness = 3;
        s.mRadius = RECT_RADIUS;
        s.mForeground = eui::COLOUR_GREY;
        SetStyle(s);
    }
    return true;
}

bool Temperature::OnDraw(eui::Graphics* pGraphics,const eui::Rectangle& pContentRect)
{
    DrawRectangle(pGraphics,pContentRect,GetStyle());

    const eui::Rectangle textRect = pContentRect.GetScaled(0.8f);

    const int font = GetFont();

    const std::string outside = mOutside.temperature;
    const std::string shed = mShed.temperature;
    float loft = 0;

    if( mLoft.temperature.size()>0 )
    {
        loft = std::stof(mLoft.temperature);
    }

    char loftS[16];
    sprintf(loftS,"%2.2fC",loft);

    eui::Colour outSideColour = GetStyle().mForeground;
    eui::Colour shedColour = GetStyle().mForeground;
    eui::Colour loftColour = GetStyle().mForeground;
    if( dayDisplay )
    {
        if( mOutside.GetIsOnline() == false )
        {
            outSideColour = eui::COLOUR_RED;
        }

        if( mShed.GetIsOnline() == false )
        {
            shedColour = eui::COLOUR_RED;
        }

        if( mLoft.GetIsOnline() == false )
        {
            loftColour = eui::COLOUR_RED;
        }
    }
    else
    {
        if( mOutside.GetIsOnline() == false )
        {
            outSideColour = eui::COLOUR_DARK_RED;
        }

        if( mShed.GetIsOnline() == false )
        {
            shedColour = eui::COLOUR_DARK_RED;
        }

        if( mLoft.GetIsOnline() == false )
        {
            loftColour = eui::COLOUR_DARK_RED;
        }
    }    

    pGraphics->FontPrint(font,textRect,eui::ALIGN_LEFT_CENTER,outSideColour,outside);
    pGraphics->FontPrint(font,textRect,eui::ALIGN_CENTER_CENTER,shedColour,shed);
    pGraphics->FontPrint(font,textRect,eui::ALIGN_RIGHT_CENTER,loftColour,loftS);

    const eui::Colour smallCol = dayDisplay?eui::COLOUR_DARK_GREEN:eui::COLOUR_DARK_GREY;
    pGraphics->FontPrint(mSmallFont,textRect,eui::ALIGN_LEFT_BOTTOM,smallCol,"Outside");
    pGraphics->FontPrint(mSmallFont,textRect,eui::ALIGN_CENTER_BOTTOM,smallCol,"Shed");
    pGraphics->FontPrint(mSmallFont,textRect,eui::ALIGN_RIGHT_BOTTOM,smallCol,"Loft");

    return true;
}

void Temperature::NewShedTemperature(const std::string pTemperature)
{
    mShed.lastUpdate = std::chrono::system_clock::now();
    mShed.temperature = pTemperature;
}

void Temperature::NewOutSideTemperature(const std::string pTemperature)
{
    mOutside.lastUpdate = std::chrono::system_clock::now();
    mOutside.temperature = pTemperature;
}

void Temperature::NewLoftTemperature(const std::string pTemperature)
{
    mLoft.lastUpdate = std::chrono::system_clock::now();
    mLoft.temperature = pTemperature;
}
