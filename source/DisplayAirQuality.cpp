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

#include "TinyJson.h"
#include "DisplayAirQuality.h"

#include <ctime>
#include <string>
#include <array>


DisplayAirQuality::DisplayAirQuality(tinygles::GLES& pGL,const std::string& pFontPath) : 
    mFont(pGL.FontLoad( pFontPath + "liberation_serif_font/LiberationSerif-Bold.ttf",30)),
    GL(pGL)
{
    auto callback = [this](int pResult,uint16_t pECO2,uint16_t pTVOC)
    {
        mResult = pResult;
        if( mResult == i2c::SGP30::READING_RESULT_VALID )
        {
            mECO2 = pECO2;
            mTVOC = pTVOC;
        }
    };

    indoorAirQuality.Start(callback);
}

void DisplayAirQuality::Update(int pX,int pY)
{
    const int width = 200;
    const int height = 100;
    const int borderWidth = 4;

    GL.FillRoundedRectangle(pX,pY,pX + width,pY + height,20,255,255,255);
    GL.FillRoundedRectangle(pX+borderWidth,pY+borderWidth,pX + width - borderWidth,pY + height - borderWidth,20,20,140,20);

    pX += 20;
    pY += 40;
    if( mResult == i2c::SGP30::READING_RESULT_WARM_UP )
    {
        GL.FontPrint(mFont,pX,pY,"Start up");
    }
    else if( mResult == i2c::SGP30::READING_RESULT_VALID )
    {
        GL.FontPrintf(mFont,pX,pY,"eCO2: %d",mECO2);
        pY += 40;
        GL.FontPrintf(mFont,pX,pY,"tVOC: %d",mTVOC);
    }
    else
    {
        GL.FontPrint(mFont,pX,pY,"Error");
    }

}