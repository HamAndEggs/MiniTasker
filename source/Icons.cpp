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


#include "Icons.h"
#include "TheWeather.h"


Icons::Icons(const std::string& pPath)
{    
    const int sizeY = 160;
    const int rounded = 15;
    const int padding = 2;
    mIconBG.Resize(GetIconWidth(),sizeY,true);
    mIconBG.Clear(0,0,0,0);
    mIconBG.FillRoundedRectangle(padding,padding,GetIconWidth()-padding-1,sizeY-padding-1,rounded,0,0,0,200);
    mIconBG.DrawRoundedRectangle(padding,padding,GetIconWidth()-padding-1,sizeY-padding-1,rounded,255,255,255);
    

    // Load the images.
    const std::vector<std::string> files =
    {
        "01d",
        "01n",
        "02d",
        "02n",
        "03d",
        "03n",
        "04d",
        "04n",
        "09d",
        "09n",
        "10d",
        "10n",
        "11d",
        "11n",
        "13d",
        "13n",
        "50d",
        "50n",
    }; 

    tinypng::Loader bg;
    for( std::string f : files )
    {
        if( bg.LoadFromFile(pPath + "icons/" + f + ".png") )
        {
            BuildIcon(bg,f);
        }
    }
}


const tiny2d::DrawBuffer& Icons::GetIcon(const std::string& pName)const
{
    auto found = mIcons.find(pName);
    if( found != mIcons.end() )
        return found->second;

    return mIconBG;
}

void Icons::BuildIcon(tinypng::Loader& bg,const std::string pName)
{
    tiny2d::DrawBuffer& icon = mIcons[pName];
    icon.Resize(bg.GetWidth(),bg.GetHeight(),bg.GetHasAlpha());

    std::vector<uint8_t> pixels;

    if( bg.GetHasAlpha() )
    {
        bg.GetRGBA(pixels);
        icon.BlitRGBA(pixels.data(),0,0,bg.GetWidth(),bg.GetHeight());
        icon.PreMultiplyAlpha();
    }
    else
    {
        bg.GetRGB(pixels);
        icon.BlitRGB(pixels.data(),0,0,bg.GetWidth(),bg.GetHeight());
    }
}
