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
    virtual eui::ElementPtr GetRootElement(){return mRoot;}
    virtual uint32_t GetUpdateInterval()const{return 1000;}


private:
    const float CELL_PADDING = 0.02f;
    const float RECT_RADIUS = 0.2f;
    const float BORDER_SIZE = 3.0f;
    const std::string mPath;
    eui::ElementPtr mRoot = nullptr;

    double ReadMyBTCInvestment(const std::string& pPath);

};

MyUI::MyUI(const std::string& path):mPath(path)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

MyUI::~MyUI()
{
	curl_global_cleanup();
}

void MyUI::OnOpen(eui::Graphics* pGraphics)
{
    std::cout << "mPath = " << mPath << "\n";
    
    mRoot = new eui::Element;
    const double myBTC = ReadMyBTCInvestment(mPath);

    mRoot->SetID("mainScreen");
    mRoot->SetGrid(3,3);

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
        DisplayBitcoinPrice* btc = new DisplayBitcoinPrice(bitcoinFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
            BottomPannel->Attach(btc);

        DisplayTideData* tide = new DisplayTideData(bitcoinFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
            tide->SetPos(0,1);
            tide->SetSpan(3,1);
            BottomPannel->Attach(tide);

        BottomPannel->Attach(new DisplaySolaX(pGraphics,mPath,largeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
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
        eui::ElementPtr MyInvestment = new eui::Element(btc->GetUpStyle());
            MyInvestment->SetPadding(0.05f);
            MyInvestment->SetText("£XXXXXX");
            MyInvestment->SetPadding(CELL_PADDING);
            MyInvestment->SetPos(0,0);
            MyInvestment->SetFont(bitcoinFont);
            MyInvestment->SetOnUpdate([btc,myBTC](eui::ElementPtr pElement,const eui::Rectangle& pContentRect)
            {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << (btc->GetPriceGBP() * myBTC);

                pElement->SetText(ss.str());
                return true;
            });
        topCentre->Attach(MyInvestment);
        
        // The temperature outside
        eui::ElementPtr outSideTemp = new Temperature(temperatureFont,btc->GetUpStyle(),CELL_PADDING);
            outSideTemp->SetPos(0,1);
        topCentre->Attach(outSideTemp);
    mRoot->Attach(topCentre);
}


void MyUI::OnClose()
{
    delete mRoot;
    mRoot = nullptr;
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
