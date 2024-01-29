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

#ifndef DISPLAY_SOLAX_H
#define DISPLAY_SOLAX_H

#include "Graphics.h"
#include "Element.h"

#include <ctime>
#include <string>
#include <map>

#include <vector>

class DisplaySolaX : public eui::Element
{
public:

    DisplaySolaX(eui::Graphics* graphics,const std::string& pPath,int pFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS);
    ~DisplaySolaX();

    void UpdateData(const std::string& pTopic,const std::string& pData);

private:

    eui::ElementPtr mBatterySOC,mYeld,mInverter,mFeedIn,mFrontPanels,mBackPanels;
    eui::Style ImportStyle,ExportStyle;

};

#endif //#ifndef DISPLAY_WEATHER_H
