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

#include "DisplaySystemStatus.h"

#include <string>
#include <array>

DisplaySystemStatus::DisplaySystemStatus(tinygles::GLES& pGL,const std::string& pFontPath):
    mFont( pGL.FontLoad( pFontPath + "liberation_serif_font/LiberationSerif-Bold.ttf")),
    mSmallFont( pGL.FontLoad( pFontPath + "liberation_serif_font/LiberationSerif-Bold.ttf",20)),
    GL(pGL)
{
    std::map<int,int> CPULoads;
    int totalSystemLoad;
    tinytools::system::GetCPULoad(mTrackingData,totalSystemLoad,CPULoads);
}

DisplaySystemStatus::~DisplaySystemStatus()
{

}

void DisplaySystemStatus::Render(int pX,int pY)
{
    // Render the uptime
    uint64_t upDays,upHours,upMinutes;
    tinytools::system::GetUptime(upDays,upHours,upMinutes);

    const int border = 4;
    const int fontBorder = 30;
    const int Height = 100;
    const int Width = 340;
    GL.FillRoundedRectangle(pX,pY,pX + Width,pY + Height,20,255,255,255);
    GL.FillRoundedRectangle(pX+border,pY+border,pX + Width - border,pY + Height - border,18,20,30,180);
    GL.FontPrintf(mFont,pX + fontBorder,pY + 40,"Uptime: %lld:%02lld:%02lld",upDays,upHours,upMinutes);

    GL.FontPrint(mSmallFont,pX + fontBorder,pY + 70,tinytools::network::GetLocalIP());

    const std::string hostName = tinytools::network::GetHostName();
    if( hostName.size() > 0 )
    {
        const int textWidth = GL.FontGetPrintWidth(mSmallFont,hostName);

        GL.FontPrint(mSmallFont,pX + Width - textWidth - fontBorder,pY + 70,hostName);
    }

    std::map<int,int> CPULoads;
    int totalSystemLoad;
    tinytools::system::GetCPULoad(mTrackingData,totalSystemLoad,CPULoads);
    if(CPULoads.size() > 0)
    {
        GL.FontPrintf(mSmallFont,pX + fontBorder,pY + 90,"CPU:%d%%",totalSystemLoad);
    }
    else
    {
        GL.FontPrint(mSmallFont,pX + fontBorder,pY + 90,"CPU:--%%");
    }

    size_t memoryUsedKB,memAvailableKB,memTotalKB,swapUsedKB;
    if( tinytools::system::GetMemoryUsage(memoryUsedKB,memAvailableKB,memTotalKB,swapUsedKB) )
    {
        const std::string memory = "Mem:" + std::to_string(memoryUsedKB * 100 / memTotalKB) + "%"; 
        const int textWidth = GL.FontGetPrintWidth(mSmallFont,memory);

        GL.FontPrint(mSmallFont,pX + Width - textWidth - fontBorder,pY + 90,memory);
    }
}
