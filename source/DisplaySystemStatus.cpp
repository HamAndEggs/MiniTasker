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
#include "style.h"

DisplaySystemStatus::DisplaySystemStatus(int pBigFont,int pNormalFont,int pMiniFont)
{
    this->SetID("system status");
    this->SetPos(2,0);
    this->SetGrid(1,4);

    uptime = new eui::Element;
        uptime->SetPadding(0.05f);
        uptime->GetStyle().mFont = (pNormalFont);
        uptime->SetText("UP: XX:XX:XX");
        uptime->SetPos(0,0);
    this->Attach(uptime);

    localIP = new eui::Element;
        localIP->SetPadding(0.05f);
        localIP->GetStyle().mFont = (pMiniFont);
        localIP->SetText("XX.XX.XX.XX");
        localIP->SetPos(0,1);
    this->Attach(localIP);

    hostName = new eui::Element;
        hostName->SetPadding(0.05f);
        hostName->GetStyle().mFont = (pMiniFont);
        hostName->SetText("--------");
        hostName->SetPos(0,2);
    this->Attach(hostName);

    cpuLoad = new eui::Element;
        cpuLoad->SetPadding(0.05f);
        cpuLoad->GetStyle().mAlignment = eui::ALIGN_LEFT_CENTER;
        cpuLoad->GetStyle().mFont = (pMiniFont);
        cpuLoad->SetText("XX.XX.XX.XX");
        cpuLoad->SetPos(0,3);
    this->Attach(cpuLoad);

    ramUsed = new eui::Element;
        ramUsed->SetPadding(0.05f);
        ramUsed->GetStyle().mAlignment = eui::ALIGN_RIGHT_CENTER;
        ramUsed->GetStyle().mFont = (pMiniFont);
        ramUsed->SetText("--------");
        ramUsed->SetPos(0,3);
    this->Attach(ramUsed);

    this->SetPadding(CELL_PADDING);

    std::map<int,int> CPULoads;
    int totalSystemLoad;
    tinytools::system::GetCPULoad(trackingData,totalSystemLoad,CPULoads);
}

bool DisplaySystemStatus::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( dayDisplay )
    {
        eui::Style s;
        s.mBackground = eui::COLOUR_BLUE;
        s.mThickness = BORDER_SIZE;
        s.mBorder = eui::COLOUR_WHITE;
        s.mRadius = RECT_RADIUS;

        uptime->GetStyle().mForeground = eui::COLOUR_WHITE;
        localIP->GetStyle().mForeground = eui::COLOUR_WHITE;
        hostName->GetStyle().mForeground = eui::COLOUR_WHITE;
        cpuLoad->GetStyle().mForeground = eui::COLOUR_WHITE;
        ramUsed->GetStyle().mForeground = eui::COLOUR_WHITE;

        this->SetStyle(s);
    }
    else
    {
        eui::Style s;
        s.mForeground = eui::COLOUR_GREY;
        s.mThickness = BORDER_SIZE;
        s.mBorder = eui::COLOUR_DARK_GREY;
        s.mRadius = RECT_RADIUS;
        this->SetStyle(s);

        uptime->GetStyle().mForeground = eui::COLOUR_GREY;
        localIP->GetStyle().mForeground = eui::COLOUR_GREY;
        hostName->GetStyle().mForeground = eui::COLOUR_GREY;
        cpuLoad->GetStyle().mForeground = eui::COLOUR_GREY;
        ramUsed->GetStyle().mForeground = eui::COLOUR_GREY;

    }    
// Render the uptime
    uint64_t upDays,upHours,upMinutes;
    tinytools::system::GetUptime(upDays,upHours,upMinutes);

    uptime->SetTextF("UP: %lld:%02lld:%02lld",upDays,upHours,upMinutes);

    localIP->SetText(tinytools::network::GetLocalIP());

    hostName->SetText(tinytools::network::GetHostName());

    std::map<int,int> CPULoads;
    int totalSystemLoad;
    tinytools::system::GetCPULoad(trackingData,totalSystemLoad,CPULoads);
    if(CPULoads.size() > 0)
    {
        cpuLoad->SetTextF("CPU:%d%%",totalSystemLoad);
    }
    else
    {
        cpuLoad->SetText("CPU:--%%");
    }

    size_t memoryUsedKB,memAvailableKB,memTotalKB,swapUsedKB;
    if( tinytools::system::GetMemoryUsage(memoryUsedKB,memAvailableKB,memTotalKB,swapUsedKB) )
    {
        const std::string memory = "Mem:" + std::to_string(memoryUsedKB * 100 / memTotalKB) + "%"; 
        ramUsed->SetText(memory);
    }
    return true;
}
