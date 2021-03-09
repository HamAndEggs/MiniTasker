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

class Icons
{
public:
    Icons(const std::string& pIconFolder);

    const tiny2d::DrawBuffer& GetIcon(const std::string& pName)const;
    const tiny2d::DrawBuffer& GetIconBG()const{return mIconBG;}

private:
    tiny2d::DrawBuffer mIconBG;
    std::map<std::string,tiny2d::DrawBuffer>mIcons;

    void BuildIcon(tinypng::Loader& bg,const std::string pName);
};

#endif //#ifndef ICONS_H