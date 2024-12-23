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
#include "TinyTools.h"
#include "DisplaySolaX.h"
#include "style.h"

#include <ctime>
#include <string>
#include <array>

DisplaySolaX::DisplaySolaX(eui::Graphics* graphics,const std::string& pPath,int pFont)
{
    SET_DEFAULT_ID();

    this->SetID("solaX");
    this->SetPos(0,0);
    this->SetGrid(6,1);
    this->GetStyle().mFont = (pFont);
    this->SetSpan(6,1);

mYeld = nullptr;
//    mYeld = new eui::Element;
//    mYeld->SetPadding(0.05f);
//    mYeld->SetText("Fetching");
//    mYeld->SetPadding(CELL_PADDING);
//    mYeld->SetPos(1,0);
//    mYeld->SetStyle(SOCStyle);
//    this->Attach(mYeld);

    mBatterySOC = new eui::Element;
    mBatterySOC->SetPadding(0.05f);
    mBatterySOC->SetText("Fetching");
    mBatterySOC->SetPadding(CELL_PADDING);
    mBatterySOC->SetPos(0,0);

    this->Attach(mBatterySOC);
    
    mInverter = new eui::Element;
    mInverter->SetPadding(0.05f);
    mInverter->SetText("Fetching");
    mInverter->SetPadding(CELL_PADDING);
    mInverter->SetPos(1,0);
    this->Attach(mInverter);

    mFeedIn = new eui::Element;
    mFeedIn->SetPadding(0.05f);
    mFeedIn->SetText("Fetching");
    mFeedIn->SetPadding(CELL_PADDING);
    mFeedIn->SetPos(2,0);

    this->Attach(mFeedIn);

    eui::ElementPtr pannels = new eui::Element;
        pannels->SetPos(4,0);
        pannels->SetSpan(2,1);
        pannels->SetGrid(2,1);
        mFrontPanels = new eui::Element;
            mFrontPanels->SetPadding(0.05f);
            mFrontPanels->SetText("...");
            mFrontPanels->SetPadding(CELL_PADDING);
            mFrontPanels->SetPos(0,0);
        pannels->Attach(mFrontPanels);

        mBackPanels = new eui::Element;
            mBackPanels->SetPadding(0.05f);
            mBackPanels->SetText("...");
            mBackPanels->SetPadding(CELL_PADDING);
            mBackPanels->SetPos(1,0);
        pannels->Attach(mBackPanels);
    this->Attach(pannels);
}

DisplaySolaX::~DisplaySolaX()
{

}

bool DisplaySolaX::OnUpdate(const eui::Rectangle& pContentRect)
{
    if( dayDisplay )
    {
        ExportStyle.mBackground = eui::MakeColour(100,255,100);
        ExportStyle.mThickness = BORDER_SIZE;
        ExportStyle.mBorder = eui::COLOUR_WHITE;
        ExportStyle.mRadius = RECT_RADIUS;
        ExportStyle.mForeground = eui::COLOUR_BLACK;

        ImportStyle.mBackground = eui::MakeColour(255,100,100);
        ImportStyle.mThickness = BORDER_SIZE;
        ImportStyle.mBorder = eui::COLOUR_WHITE;
        ImportStyle.mRadius = RECT_RADIUS;
        ImportStyle.mForeground = eui::COLOUR_BLACK;
    }
    else
    {
        ExportStyle.mForeground = eui::COLOUR_DARK_GREEN;
        ImportStyle.mForeground = eui::COLOUR_DARK_RED;
    }

    eui::Style SOCStyle;
    if( dayDisplay )
    {
        SOCStyle.mBackground = eui::MakeColour(100,255,100);
        SOCStyle.mThickness = BORDER_SIZE;
        SOCStyle.mBorder = eui::COLOUR_WHITE;
        SOCStyle.mRadius = RECT_RADIUS;
        SOCStyle.mForeground = eui::COLOUR_BLACK;
    }
    else
    {
        SOCStyle.mThickness = BORDER_SIZE;
        SOCStyle.mBorder = eui::COLOUR_DARK_GREY;
        SOCStyle.mRadius = RECT_RADIUS;
        SOCStyle.mForeground = eui::COLOUR_GREY;
    }

    mBatterySOC->SetStyle(SOCStyle);
    mInverter->SetStyle(SOCStyle);
    mFeedIn->SetStyle(SOCStyle);
    mFrontPanels->SetStyle(SOCStyle);
    mBackPanels->SetStyle(SOCStyle);

    return true;
}

void DisplaySolaX::UpdateData(const std::string& pTopic,const std::string& pData)
{
    if( tinytools::string::CompareNoCase(pTopic,"/solar/battery/total") )
    {
        mBatterySOC->SetTextF("%d%%",std::stoi(pData));
    }
    else if( mYeld && tinytools::string::CompareNoCase(pTopic,"/solar/yeld") )
    {
        mYeld->SetTextF("%2.2f",std::stof(pData));
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/inverter/total") )
    {
        mInverter->SetTextF("%d",std::stoi(pData));
    }
    else if( mFeedIn && tinytools::string::CompareNoCase(pTopic,"/solar/grid/total") )
    {
        const int total = std::stoi(pData);
        if( total < 0 )
        {
            mFeedIn->SetStyle(ImportStyle);
            mFeedIn->SetTextF("%d",-total);
        }
        else
        {
            mFeedIn->SetStyle(ExportStyle);
            mFeedIn->SetTextF("%d",total);
        }
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/panel/front") )
    {
        mFrontPanels->SetTextF("%d",std::stoi(pData));
    }
    else if( tinytools::string::CompareNoCase(pTopic,"/solar/panel/rear") )
    {
        mBackPanels->SetTextF("%d",std::stoi(pData));
    }
}

