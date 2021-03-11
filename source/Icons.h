/*
   Copyright (C) 2021, Richard e Collins.

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
   
#ifndef ICONS_H
#define ICONS_H

#include <map>
#include <string>

#include "Tiny2D.h"
#include "TinyPNG.h"

class TheWeather;

class Icons
{
public:
    Icons(const std::string& pIconFolder);

    const tiny2d::DrawBuffer& GetIcon(const std::string& pName)const;
    const tiny2d::DrawBuffer& GetIconBG()const{return mIconBG;}

    /**
     * @brief Draws a set of icons to get a brief over view of the weather coming up.
     * 
     * @param pFB 
     * @param pX 
     * @param pY 
     */
    void RenderWeatherForcast(tiny2d::DrawBuffer& RT,int pY,const tm& pCurrentTime,const TheWeather& pWeather);


private:
    tiny2d::DrawBuffer mIconBG;
    std::map<std::string,tiny2d::DrawBuffer>mIcons;
    tiny2d::FreeTypeFont mIconFont;
    tiny2d::FreeTypeFont mTemperatureFont;    

    void BuildIcon(tinypng::Loader& bg,const std::string pName);
};

#endif //#ifndef ICONS_H