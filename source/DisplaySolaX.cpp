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

#include <ctime>
#include <string>
#include <array>


const std::time_t ONE_MINUTE = (60);
const std::time_t ONE_HOUR = (ONE_MINUTE * 60);
const std::time_t ONE_DAY = (ONE_HOUR*24);

static const std::time_t RoundToHour(const std::time_t pTime)
{
    return pTime - (pTime%ONE_HOUR);
}

static const std::time_t NextHour(const std::time_t pTime)
{
    return RoundToHour(pTime) + ONE_HOUR;
}

// Mainly for debugging.
static std::string GetTimeString(const std::time_t pTime)
{
    tm currentTime = *localtime((time_t*)&pTime);
	return std::to_string(currentTime.tm_hour) + ":" + std::to_string(currentTime.tm_min) + ":" + std::to_string(currentTime.tm_sec);
}

static std::string ReadSolaXKeyFile(const std::string& pPath)
{ 
    std::cout << "pPath" << pPath << "\n";
    std::cout << tinytools::file::GetCurrentWorkingDirectory() << "\n";

    std::string keyFile;
    try
    {
        keyFile = tinytools::file::LoadFileIntoString(pPath + "solax.key");
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "Could not load solaX key from current folder, trying hard coded path\n";
    }

    if( keyFile.size() == 0 )
    {
        try
        {
            keyFile = tinytools::file::LoadFileIntoString("/usr/share/mini-tasker/solax.key");
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Could not load solaX key from hard coded path\n";
            return "";
        }
    }
    return keyFile;
}

static std::string ReadSolaXKey(const std::string& keyFile)
{

    tinyjson::JsonProcessor json(keyFile);

    const std::string key = json.GetRoot()["key"].GetString();

    if( key.size() < 4 )
    {
        std::cerr << "Solax key too short, will not work\n";
    }

    return key;
}

static std::string ReadSolaXSerialNumber(const std::string& keyFile)
{

    tinyjson::JsonProcessor json(keyFile);

    const std::string SN = json.GetRoot()["sn"].GetString();

    if( SN.size() < 4 )
    {
        std::cerr << "Solax SN too short, will not work\n";
    }

    return SN;
}

DisplaySolaX::DisplaySolaX(eui::Graphics* graphics,const std::string& pPath,int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS) :
    mSerialNumber(ReadSolaXSerialNumber(ReadSolaXKeyFile(pPath))),
    mHasData(false),
    mFirstFail(true),
    mFetchLimiter(0),
    mSolaX(ReadSolaXKey(ReadSolaXKeyFile(pPath)))
{
    SET_DEFAULT_ID();

    this->SetID("solaX");
    this->SetPos(2,0);
    this->SetGrid(2,1);

    eui::Style SOCStyle;
    SOCStyle.mBackground = eui::MakeColour(100,255,100);
    SOCStyle.mThickness = BORDER_SIZE;
    SOCStyle.mBorder = eui::COLOUR_WHITE;
    SOCStyle.mRadius = RECT_RADIUS;
    SOCStyle.mForeground = eui::COLOUR_BLACK;


    mBatterySOC = new eui::Element;
    mBatterySOC->SetPadding(0.05f);
    mBatterySOC->SetText("Fetching");
    mBatterySOC->SetPadding(CELL_PADDING);
    mBatterySOC->SetPos(0,0);
    mBatterySOC->SetStyle(SOCStyle);
    this->Attach(mBatterySOC);

//    this->SetPos(0,0);

//    this->SetPos(0,1);
//    this->SetGrid(6,1);
//    this->SetSpan(3,1);
    

}

DisplaySolaX::~DisplaySolaX()
{

}

bool DisplaySolaX::OnUpdate(const eui::Rectangle& pContentRect)
{
    std::time_t currentTime = std::time(nullptr);

    if( mFetchLimiter < currentTime )
    {
        mHasData = false;
        mSolaX.Get(mSerialNumber,[this,currentTime](const solax::RealtimeData &pData)
        {
            if( pData.success )
            {
                mHasData = true;

                // It worked, do the next fetch in five minutes time
                mFetchLimiter = currentTime + (ONE_MINUTE * 5);

                mBatterySOC->SetTextF("%d%%",(int)pData.BatterySOC);
                mYieldToday = pData.Inverter.energyOutDaily;

                std::cout << "Uploaded " << pData.Uploadtime << '\n';
                std::cout << "State of Charge: " << pData.BatterySOC << "%\n";
                std::cout << "Daily out: " << pData.Inverter.energyOutDaily << "KWh\n";
                std::cout << "Total out: " << pData.Inverter.energyTotal << "KWh\n";

                std::cout << "\n";

            }
            else
            {
                if( mFirstFail )
                {
                    mFirstFail = false;
                    mFetchLimiter = currentTime + 60;// Just booting up, try again in a minute.
                }
                else
                {
                    mFetchLimiter = currentTime + (60*60);// If it fails, this will make the next attempt an hour later.
                }
            }
        });
    }

    if( mHasData )
    {

    }

    return true;
}

