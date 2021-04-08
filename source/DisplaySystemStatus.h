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
   
#ifndef DISPLAY_SYSTEM_STATUS_H
#define DISPLAY_SYSTEM_STATUS_H

#include "TinyGLES.h"

#include "TinyTools.h"


#include <string>
#include <time.h>

class DisplaySystemStatus
{
public:
    DisplaySystemStatus(tinygles::GLES& pGL,const std::string& pFontPath);
    ~DisplaySystemStatus();

    void Render(int pX,int pY);

private:

    const uint32_t mFont = 0;
    const uint32_t mSmallFont = 0;

    tinygles::GLES& GL;
    std::map<int,tinytools::system::CPULoadTracking> mTrackingData;

};

#endif //#ifndef DISPLAY_SYSTEM_STATUS_H