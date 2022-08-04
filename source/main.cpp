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

#include "Graphics.h"
#include "Element.h"


#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"
#include "DisplayBitcoinPrice.h"

#include <unistd.h>
#include <filesystem>

class MyUI : public eui::Element
{
public:
    MyUI(const std::string& path,eui::Graphics* pGraphics);
    virtual ~MyUI(){};

private:
    const float CELL_PADDING = 0.02f;
    const float RECT_RADIUS = 0.2f;
    const float BORDER_SIZE = 3.0f;
    std::map<std::string,std::string> mOutsideData;
    std::time_t mOutsideTemperatureDelivered = 0;

    double ReadMyBTCInvestment(const std::string& pPath);

};

MyUI::MyUI(const std::string& path,eui::Graphics* pGraphics) : Element()
{
    const double myBTC = ReadMyBTCInvestment(path);

    SetID("mainScreen");
    SetGrid(3,3);

    int miniFont = pGraphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Regular.ttf",25);
    int normalFont = pGraphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Regular.ttf",40);
    int largeFont = pGraphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Bold.ttf",55);

    int bigFont = pGraphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Bold.ttf",130);

    int bitcoinFont = pGraphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Bold.ttf",70);

    SetFont(normalFont);
    GetStyle().mTexture = pGraphics->TextureLoadPNG(path + "images/bg-pastal-01.png");
    GetStyle().mBackground = eui::COLOUR_WHITE;

    DisplayBitcoinPrice* btc = new DisplayBitcoinPrice(bitcoinFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
    Attach(btc);

    Attach(new DisplayClock(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    Attach(new DisplayWeather(pGraphics,path,bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr status = eui::Element::Create();
    status->SetGrid(1,2);
    status->SetPos(2,0);
    Attach(status);
    status->Attach(new DisplaySystemStatus(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    status->Attach(new DisplayAirQuality(largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    // MQTT data
    const std::vector<std::string> topics = {"/outside/temperature","/outside/hartbeat"};
    mOutsideData["/outside/temperature"] = "Waiting";// Make sure there is data.
    MQTTData OutsideWeather("server",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
//            std::cout << pData << "\n";
            mOutsideData[pTopic] = pData;
            mOutsideTemperatureDelivered = std::time(nullptr);
        });

    eui::ElementPtr topCentre = eui::Element::Create();
        topCentre->SetPos(1,0);
        topCentre->SetGrid(1,2);

        // My bitcoin investment.
        eui::ElementPtr MyInvestment = eui::Element::Create(btc->GetUpStyle());
            MyInvestment->SetPadding(0.05f);
            MyInvestment->SetText("£XXXXXX");
            MyInvestment->SetPadding(CELL_PADDING);
            MyInvestment->SetPos(0,0);
            MyInvestment->SetFont(bitcoinFont);
            MyInvestment->SetOnUpdate([btc,myBTC](eui::ElementPtr pElement)
            {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << (btc->GetPriceGBP() * myBTC);

                pElement->SetText(ss.str());
                return true;
            });
        topCentre->Attach(MyInvestment);
        // The temperature outside
        eui::ElementPtr outSideTemp = eui::Element::Create(btc->GetUpStyle());
            outSideTemp->SetPadding(0.05f);
            outSideTemp->SetText("100.0C");
            outSideTemp->SetPadding(CELL_PADDING);
            outSideTemp->SetPos(0,1);
            outSideTemp->SetFont(bitcoinFont);
            outSideTemp->SetOnUpdate([this](eui::ElementPtr pElement)
            {
                pElement->SetText(mOutsideData["/outside/temperature"]);
                return true;
            });
        topCentre->Attach(outSideTemp);
    Attach(topCentre);
}

double MyUI::ReadMyBTCInvestment(const std::string& pPath)
{
    std::string btc;
    try
    {
        btc = tinytools::file::LoadFileIntoString(pPath + "my.btc");
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "Could not load weather key from current folder, trying hard coded path\n";
    }

    try
    {
        btc = tinytools::file::LoadFileIntoString("/usr/share/mini-tasker/my.btc");
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "Could not load weather key from hard coded path\n";
    }

    double myBTC = 1.0;
    if( btc.size() > 0 )
    {
        try
        {
            btc = tinytools::string::TrimWhiteSpace(btc);
            if( btc.size() > 0 )
            {
                myBTC = std::stod(btc);
            }
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "BTC string not a double\n";
        }
    }

    return myBTC;
}

eui::ElementPtr eui::Element::AllocateUI(const int argc,const char *argv[],eui::Graphics* pGraphics)
{
// Crude argument list handling.
    std::string path = "./";
    if( argc == 2 && std::filesystem::directory_entry(argv[1]).exists() )
    {
        path = argv[1];
        if( path.back() != '/' )
            path += '/';
    }
    pGraphics->SetUpdateFrequency(1000);
    return new MyUI(path,pGraphics); // MyUI is your derived class.
}
