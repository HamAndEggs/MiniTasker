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

#include "TinyGLES.h"
#include "TinyPNG.h"

#include <map>
#include <string>

class TheWeather;

class Icons
{
public:
    Icons(tinygles::GLES& GL,const std::string& pIconFolder);

    const uint32_t GetIcon(const std::string& pName)const;
    const uint32_t GetIconBG()const{return mIconBG;}
    int GetIconWidth()const{return 166;}

private:
    const std::string mIconFolder;
    uint32_t mIconBG;
    std::map<std::string,uint32_t>mIcons;

    uint32_t LoadIconTexture(tinygles::GLES& GL,tinypng::Loader& pLoader,const std::string pName);
};

#endif //#ifndef ICONS_H