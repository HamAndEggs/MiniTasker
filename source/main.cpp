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

#include "Style.h"
#include "Graphics.h"
#include "Element.h"
#include "Application.h"


#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySolaX.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"
#include "DisplayBitcoinPrice.h"
#include "DisplayTideData.h"
#include "Temperature.h"
#include "MQTTData.h"

#include <unistd.h>
#include <filesystem>
#include <curl/curl.h> // libcurl4-openssl-dev

class MyUI : public eui::Application
{
public:
    MyUI(const std::string& path);
    virtual ~MyUI();

    virtual void OnOpen(eui::Graphics* pGraphics);
    virtual void OnClose();
    virtual void OnUpdate()
    {
        MQTT->Tick();
    }

    virtual eui::ElementPtr GetRootElement(){return mRoot;}
    virtual uint32_t GetUpdateInterval()const{return 1000;}


private:
    const float CELL_PADDING = 0.02f;
    const float RECT_RADIUS = 0.2f;
    const float BORDER_SIZE = 3.0f;
    const std::string mPath;
    eui::ElementPtr mRoot = nullptr;
    MQTTData* MQTT = nullptr;
    std::map<std::string,std::string> mMQTTData;

    Temperature *mOutSideTemp;
    DisplayBitcoinPrice* mBTC; 
    std::string myBTC = "n/a";

    DisplaySolaX* mSolar = nullptr;
    int n = 0;


    void StartMQTT();

};

MyUI::MyUI(const std::string& path):mPath(path)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

MyUI::~MyUI()
{
    delete MQTT;
	curl_global_cleanup();
}

void MyUI::OnOpen(eui::Graphics* pGraphics)
{
    std::cout << "mPath = " << mPath << "\n";
    
    mRoot = new eui::Element;

    mRoot->SetID("mainScreen");
    mRoot->SetGrid(3,3);

    StartMQTT();    

    int miniFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",25);
    int normalFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",40);
    int temperatureFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",49);
    int largeFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",55);

    int bigFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",130);

    int bitcoinFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",70);

    mRoot->SetFont(normalFont);
    mRoot->GetStyle().mTexture = pGraphics->TextureLoadPNG(mPath + "images/bg-pastal-01.png");
    mRoot->GetStyle().mBackground = eui::COLOUR_WHITE;

    eui::ElementPtr BottomPannel = new eui::Element;
        BottomPannel->SetPos(0,2);
        BottomPannel->SetGrid(3,2);
        BottomPannel->SetSpan(3,1);
        mBTC = new DisplayBitcoinPrice(bitcoinFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
        BottomPannel->Attach(mBTC);

        DisplayTideData* tide = new DisplayTideData(largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
            tide->SetPos(0,1);
            tide->SetSpan(3,1);
            BottomPannel->Attach(tide);

        mSolar = new DisplaySolaX(pGraphics,mPath,largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
        BottomPannel->Attach(mSolar);
    mRoot->Attach(BottomPannel);

    mRoot->Attach(new DisplayClock(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    mRoot->Attach(new DisplayWeather(pGraphics,mPath,bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr status = new eui::Element;
    status->SetGrid(1,2);
    status->SetPos(2,0);
    mRoot->Attach(status);
    status->Attach(new DisplaySystemStatus(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    status->Attach(new DisplayAirQuality(largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr topCentre = new eui::Element;
        topCentre->SetPos(1,0);
        topCentre->SetGrid(1,2);

        // My bitcoin investment.
        eui::ElementPtr MyInvestment = new eui::Element(mBTC->GetUpStyle());
            MyInvestment->SetPadding(0.05f);
            MyInvestment->SetText("£XXXXXX");
            MyInvestment->SetPadding(CELL_PADDING);
            MyInvestment->SetPos(0,0);
            MyInvestment->SetFont(bitcoinFont);
            MyInvestment->SetOnUpdate([this](eui::ElementPtr pElement,const eui::Rectangle& pContentRect)
            {
                pElement->SetText(myBTC);
                return true;
            });
        topCentre->Attach(MyInvestment);
        
        // The temperature outside
        mOutSideTemp = new Temperature(temperatureFont,mBTC->GetUpStyle(),CELL_PADDING);
            mOutSideTemp->SetPos(0,1);
        topCentre->Attach(mOutSideTemp);
    mRoot->Attach(topCentre);

    std::cout << "UI started\n";
}


void MyUI::OnClose()
{
    delete mRoot;
    mRoot = nullptr;
}

void MyUI::StartMQTT()
{
    // MQTT data
    const std::vector<std::string> topics =
    {
        "/outside/temperature","/outside/hartbeat",
        "/shed/temperature","/shed/hartbeat",
        "/btc/gb",
        "/btc/usd",
        "/btc/change",
        "/btc/diskused",
        "/btc/verificationprogress",
        "/btc/blockdate",
        "/btc/mine",
        "/solar/battery/total",
        "/solar/inverter/total",
        "/solar/grid/total",
        "/solar/yeld",
        "/solar/panel/front",
        "/solar/panel/rear"        
    };

    for( auto t : topics )
    {
        mMQTTData[t] = "N/A";
    }

    // Make sure there is data.
    mMQTTData["/outside/temperature"] = "--.-C";
    mMQTTData["/shed/temperature"] = "--.-C";
    mMQTTData["/shed/temperature"] = "--.-C";

    MQTT = new MQTTData("MQTT",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
//            std::cout << "MQTTData " << pTopic << " " << pData << "\n";
            mMQTTData[pTopic] = pData;

            // Record when we last seen a change, if we don't see one for a while something is wrong.
            // I send an 'hartbeat' with new data that is just a value incrementing.
            // This means we get an update even if the tempareture does not change.
            if( tinytools::string::CompareNoCase(pTopic,"/outside") && mOutSideTemp )
            {
                mOutSideTemp->NewShedOutSide(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/shed") )
            {
                mOutSideTemp->NewShedTemperature(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/btc/gb") )
            {
                mBTC->UpdateGBP(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/btc/usd") )
            {
                mBTC->UpdateUSD(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/btc/change") )
            {
                mBTC->UpdateChange(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/btc/mine") )
            {
                myBTC = pData;
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/solar/",7) )
            {
                mSolar->UpdateData(pTopic,pData);
            }
        });

}

int main(const int argc,const char *argv[])
{
#ifdef NDEBUG
    std::cout << "Release\n";
#else
    std::cout << "Debug\n";
#endif


// Crude argument list handling.
    std::string path = "./";
    if( argc == 2 && std::filesystem::directory_entry(argv[1]).exists() )
    {
        path = argv[1];
        if( path.back() != '/' )
            path += '/';
    }

    MyUI* theUI = new MyUI(path); // MyUI is your derived application class.
    eui::Application::MainLoop(theUI);
    delete theUI;

    return EXIT_SUCCESS;
}
