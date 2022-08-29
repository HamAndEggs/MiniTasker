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

Temperature::Temperature(int pFont,const eui::Style &pStyle,float CELL_PADDING) : eui::Element(pStyle)
{
    // MQTT data
    const std::vector<std::string> topics =
    {
        "/outside/temperature","/outside/hartbeat",
        "/shed/temperature","/shed/hartbeat"
    };

    mOutsideData["/outside/temperature"] = "Waiting";// Make sure there is data.
    mOutsideData["/shed/temperature"] = "Waiting";// Make sure there is data.
    mOutsideWeather = new MQTTData("server",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
            std::cout << "MQTTData " << pTopic << " " << pData << "\n";
            mOutsideData[pTopic] = pData;
            mOutsideTemperatureUpdateSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - mOutsideTemperatureDelivered).count();
            mOutsideTemperatureDelivered = std::chrono::system_clock::now();
        });

    SetPadding(0.05f);
    SetText("100.0C");
    SetPadding(CELL_PADDING);
    SetFont(pFont);
}

bool Temperature::OnDraw(eui::Graphics* pGraphics,const eui::Rectangle& pContentRect)
{
    pGraphics->DrawRectangle(pContentRect,GetStyle());

    const eui::Rectangle textRect = pContentRect.GetScaled(0.8f);

    const int font = GetFont();

//    const std::string outside = "Front: " + mOutsideData["/outside/temperature"];
//    const std::string shed = "Shed: " + mOutsideData["/shed/temperature"];
//
//    pGraphics->FontPrint(font,textRect,eui::ALIGN_CENTER_TOP,GetStyle().mForeground,outside);
//    pGraphics->FontPrint(font,textRect,eui::ALIGN_CENTER_BOTTOM,GetStyle().mForeground,shed);

    const std::string outside = mOutsideData["/outside/temperature"];
    const std::string shed = mOutsideData["/shed/temperature"];

    pGraphics->FontPrint(font,textRect,eui::ALIGN_LEFT_CENTER,GetStyle().mForeground,outside);
    pGraphics->FontPrint(font,textRect,eui::ALIGN_RIGHT_CENTER,GetStyle().mForeground,shed);

    return true;
}