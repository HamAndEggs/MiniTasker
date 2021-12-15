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
#include <iostream>

Icons::Icons(tinygles::GLES& GL,const std::string& pPath) :
    mIconFolder(pPath + "icons/")
{    
    tinypng::Loader loader;

    mIconBG = LoadIconTexture(GL,loader,"square-rounded");

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

    for( std::string f : files )
    {
        mIcons[f] = LoadIconTexture(GL,loader,f);
    }
}


const uint32_t Icons::GetIcon(const std::string& pName)const
{
    auto found = mIcons.find(pName);
    if( found != mIcons.end() )
        return found->second;

    return mIconBG;
}

uint32_t Icons::LoadIconTexture(tinygles::GLES& GL,tinypng::Loader& pLoader,const std::string pName)
{
    if( pLoader.LoadFromFile(mIconFolder + pName + ".png") )
    {
        std::vector<uint8_t> pixels;
        pLoader.GetRGBA(pixels);
        return GL.CreateTexture(pLoader.GetWidth(),pLoader.GetHeight(),pixels.data(),tinygles::TextureFormat::FORMAT_RGBA);
    }
    else
    {
        std::cerr << "Failed to load icon " << pName << "\n";
    }

    return GL.GetDiagnosticsTexture();
}

