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
#include "MQTTData.h"
#include "TinyTools.h"

Temperature::Temperature(int pFont,const eui::Style &pStyle,float CELL_PADDING) : eui::Element(pStyle)
{
    SET_DEFAULT_ID();

    // MQTT data
    const std::vector<std::string> topics =
    {
        "/outside/temperature","/outside/hartbeat",
        "/shed/temperature","/shed/hartbeat"
    };

    mOutsideData["/outside/temperature"] = "--.-C";// Make sure there is data.
    mOutsideData["/shed/temperature"] = "--.-C";// Make sure there is data.
    mOutsideWeather = new MQTTData("MQTT",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
            std::cout << "MQTTData " << pTopic << " " << pData << "\n";
            mOutsideData[pTopic] = pData;

            // Record when we last seen a change, if we don't see one for a while something is wrong.
            // I send an 'hartbeat' with new data that is just a value incrementing.
            // This means we get an update even if the tempareture does not change.
            if( tinytools::string::CompareNoCase(pTopic,"/outside") )
            {
                mOutsideHartbeat.lastUpdate = std::chrono::system_clock::now();
            }

            if( tinytools::string::CompareNoCase(pTopic,"/shed") )
            {
                mShedHartbeat.lastUpdate = std::chrono::system_clock::now();
            }
        });

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

    if( mOutsideWeather && mOutsideWeather->GetConnected() )
    {
        const std::string outside = mOutsideData["/outside/temperature"];
        const std::string shed = mOutsideData["/shed/temperature"];

        eui::Colour outSideColour = GetStyle().mForeground;
        eui::Colour shedColour = GetStyle().mForeground;
        if( mOutsideHartbeat.GetIsOnline() == false )
        {
            outSideColour = eui::COLOUR_RED;
        }

        if( mShedHartbeat.GetIsOnline() == false )
        {
            shedColour = eui::COLOUR_RED;
        }

        pGraphics->FontPrint(font,textRect,eui::ALIGN_LEFT_CENTER,outSideColour,outside);
        pGraphics->FontPrint(font,textRect,eui::ALIGN_RIGHT_CENTER,shedColour,shed);
    }
    else
    {
        pGraphics->FontPrint(font,textRect,eui::ALIGN_LEFT_CENTER,eui::COLOUR_DARK_GREY,"Connecting...");
    }

    return true;
}

bool Temperature::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( mOutsideWeather )
    {
        mOutsideWeather->Tick();
    }
    return true;
}
