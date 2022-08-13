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

#include "DisplayAirQuality.h"

DisplayAirQuality::DisplayAirQuality(int pFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
{

    eui::Style s;
    s.mBackground = eui::COLOUR_DARK_GREEN;
    s.mBorderSize = BORDER_SIZE;
    s.mBorder = eui::COLOUR_WHITE;
    s.mRadius = RECT_RADIUS;
    SetStyle(s);

    this->SetID("environment status");
    this->SetPadding(CELL_PADDING);
    this->SetPos(0,1);

    eCO2 = new eui::Element;
        eCO2->SetPadding(0.05f);
        eCO2->GetStyle().mAlignment = eui::ALIGN_CENTER_TOP;
        eCO2->SetFont(pFont);
        eCO2->SetText("UP: XX:XX:XX");
    this->Attach(eCO2);
    
    tOC = new eui::Element;
        tOC->SetPadding(0.05f);
        tOC->GetStyle().mAlignment = eui::ALIGN_CENTER_BOTTOM;
        tOC->SetFont(pFont);
        tOC->SetText("XX.XX.XX.XX");
    this->Attach(tOC);

    indoorAirQuality.Start([this](int pResult,uint16_t pECO2,uint16_t pTVOC)
    {
        mResult = pResult;
        if( pResult == i2c::SGP30::READING_RESULT_VALID )
        {
            mECO2 = pECO2;
            mTVOC = pTVOC;
        }
    });
}

DisplayAirQuality::~DisplayAirQuality()
{
    indoorAirQuality.Stop();
}

bool DisplayAirQuality::OnUpdate()
{
    switch (mResult)
    {
    case i2c::SGP30::READING_RESULT_WARM_UP:
        eCO2->SetText("Start up");
        tOC->SetText("Please Wait");
        break;

    case i2c::SGP30::READING_RESULT_VALID:
        eCO2->SetTextF("eCO2: %d",mECO2);
        tOC->SetTextF("tVOC: %d",mTVOC);
        break;

    default:
        eCO2->SetText("Error");
        tOC->SetText("Disconnected");
        break;
    }
    return true;
}

