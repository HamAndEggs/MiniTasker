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
#include "FileDownload.h"


#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySolaX.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"
#include "DisplayBitcoinPrice.h"
#include "DisplayTideData.h"
#include "Temperature.h"
#include "MQTTData.h"
#include "../OpenMeteoFetch/open-meteo.h"


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
    virtual void OnUpdate();

    virtual eui::ElementPtr GetRootElement()
    {
        if( GetIsDay() )
        {
            return mDayTime;
        }

        return mNightTime;
    }
    virtual uint32_t GetUpdateInterval()const{return 1000;}

    virtual int GetEmulatedWidth()const{return 720;}
    virtual int GetEmulatedHeight()const{return 720;}

//    virtual int GetEmulatedWidth()const{return 1280;}
//    virtual int GetEmulatedHeight()const{return 720;}

private:
    const float CELL_PADDING = 0.02f;
    const float RECT_RADIUS = 0.2f;
    const float BORDER_SIZE = 3.0f;
    const std::string mPath;
    eui::ElementPtr mDayTime = nullptr;
    eui::ElementPtr mNightTime = nullptr;

    std::time_t mFetchLimiter;

    MQTTData* MQTT = nullptr;
    std::map<std::string,std::string> mMQTTData;
    std::vector<openmeteo::Hourly> mForcast;

    int mMiniFont = 0;
    int mNormalFont = 0;
    int mLargeFont = 0;
    int mBigFont = 0;

    struct
    {
        Temperature *mOutSideTemp = nullptr;
        DisplaySolaX *mSolar = nullptr;
        DisplayWeather *mWeather = nullptr;

        void Update(const std::string &pTopic,const std::string &pData)
        {
            if( tinytools::string::CompareNoCase(pTopic,"/loft/temperature") && mOutSideTemp )
            {
                mOutSideTemp->NewLoftTemperature(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/outside/temperature") && mOutSideTemp )
            {
                mOutSideTemp->NewOutSideTemperature(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/shed/temperature") && mOutSideTemp)
            {
                mOutSideTemp->NewShedTemperature(pData);
            }
            else if( tinytools::string::CompareNoCase(pTopic,"/solar/",7) && mSolar )
            {
                mSolar->UpdateData(pTopic,pData);
            }            
        }
    }day,night;

    void StartMQTT();
    eui::ElementPtr MakeDayTimeDisplay(eui::Graphics* pGraphics);
    eui::ElementPtr MakeNightTimeDisplay(eui::Graphics* pGraphics);

    std::vector<openmeteo::Hourly> LoadWeather();
    bool GetIsDay()const;
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
    
    StartMQTT();

    if( pGraphics->GetDisplayWidth() > 720 )
    {
        mMiniFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",35);
        mNormalFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",70);
        mLargeFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",70);
        mBigFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",130);
    }
    else
    {
        mMiniFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",25);
        mNormalFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Regular.ttf",42);
        mLargeFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",42);
        mBigFont = pGraphics->FontLoad(mPath + "liberation_serif_font/LiberationSerif-Bold.ttf",130);
    }


    mDayTime = MakeDayTimeDisplay(pGraphics);
    mNightTime = MakeNightTimeDisplay(pGraphics);

    std::cout << "UI started\n";
}


void MyUI::OnClose()
{
    delete mDayTime;
    delete mNightTime;

    mDayTime = nullptr;
    mNightTime = nullptr;
}

void MyUI::OnUpdate()
{
    MQTT->Tick();

    std::time_t currentTime = std::time(nullptr);
    if( mFetchLimiter < currentTime )
    {
        mForcast = LoadWeather();
        if( mForcast.size() > 0 )
        {
            std::clog << "Fetched weather data\n";

            if( day.mWeather )
            {
                day.mWeather->OnNewForcast(mForcast);
            }

            if( night.mWeather )
            {
                night.mWeather->OnNewForcast(mForcast);
            }

            // It worked, do the next fetch in a days time.
            mFetchLimiter = currentTime + ONE_DAY;
            // Now round to start of day plus one minute to be safe, 00:01. The weather forcast may have changed. Also if we boot in the evening don't want all downloads at the same time every day.
            mFetchLimiter -= (mFetchLimiter%ONE_DAY);
            mFetchLimiter += ONE_MINUTE;
        }
    }
}

void MyUI::StartMQTT()
{
    // MQTT data
    const std::vector<std::string> topics =
    {
        "/outside/temperature","/outside/hartbeat",
        "/shed/temperature","/shed/hartbeat",
        "/loft/temperature","/loft/hartbeat",
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
    mMQTTData["/loft/temperature"] = "--.-C";

    MQTT = new MQTTData("MQTT",1883,topics,
        [this](const std::string &pTopic,const std::string &pData)
        {
//            std::cout << "MQTTData " << pTopic << " " << pData << "\n";
            mMQTTData[pTopic] = pData;

            // Record when we last seen a change, if we don't see one for a while something is wrong.
            // I send an 'hartbeat' with new data that is just a value incrementing.
            // This means we get an update even if the tempareture does not change.
            day.Update(pTopic,pData);
            night.Update(pTopic,pData);

        });

}

eui::ElementPtr MyUI::MakeDayTimeDisplay(eui::Graphics* pGraphics)
{
    eui::ElementPtr root = new eui::Element;

    root->SetID("Day time root");
    root->SetGrid(3,3);

    root->GetStyle().mFont = (mNormalFont);
    root->GetStyle().mTexture = pGraphics->TextureLoad(mPath + "images/bg-pastal-01.png");
    root->GetStyle().mBackground = eui::COLOUR_WHITE;

    eui::Style UpStyle;
    UpStyle.mBackground = eui::MakeColour(100,255,100);
    UpStyle.mThickness = BORDER_SIZE;
    UpStyle.mBorder = eui::COLOUR_WHITE;
    UpStyle.mRadius = RECT_RADIUS;
    UpStyle.mForeground = eui::COLOUR_BLACK;

    eui::ElementPtr BottomPannel = new eui::Element;
        BottomPannel->SetPos(0,2);
        BottomPannel->SetGrid(6,2);
        BottomPannel->SetSpan(3,1);
            day.mSolar = new DisplaySolaX(pGraphics,mPath,mLargeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,true);
            day.mOutSideTemp = new Temperature(mLargeFont,mMiniFont,UpStyle,CELL_PADDING,true);
            day.mOutSideTemp->SetPos(0,1);
            day.mOutSideTemp->SetSpan(4,1);
            BottomPannel->Attach(day.mSolar);
        BottomPannel->Attach(day.mOutSideTemp);
    root->Attach(BottomPannel);

    root->Attach(new DisplayClock(mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,true));
    root->Attach(new DisplaySystemStatus(mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,0.1f,true));

    // need to seperate the weather collection from the weather display.
    day.mWeather = new DisplayWeather(pGraphics,mPath,mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,true);
    root->Attach(day.mWeather);

    return root;
}

eui::ElementPtr MyUI::MakeNightTimeDisplay(eui::Graphics* pGraphics)
{
    eui::ElementPtr root = new eui::Element;

    root->SetID("Night time root");
    root->SetGrid(3,3);

    root->GetStyle().mFont = (mNormalFont);
    root->GetStyle().mBackground = eui::COLOUR_BLACK;
    root->GetStyle().mForeground = eui::COLOUR_BLACK;

    eui::Style temperatureStyle;
    temperatureStyle.mBorder = eui::COLOUR_DARK_GREY;
    temperatureStyle.mThickness = 3;
    temperatureStyle.mRadius = RECT_RADIUS;


    eui::ElementPtr BottomPannel = new eui::Element;
        BottomPannel->SetPos(0,2);
        BottomPannel->SetGrid(6,2);
        BottomPannel->SetSpan(3,1);

            night.mSolar = new DisplaySolaX(pGraphics,mPath,mLargeFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,false);
            night.mOutSideTemp = new Temperature(mLargeFont,mMiniFont,temperatureStyle,CELL_PADDING,false);
            night.mOutSideTemp->SetPos(0,1);
            night.mOutSideTemp->SetSpan(4,1);
            night.mOutSideTemp->GetStyle().mForeground = eui::COLOUR_GREY;
            BottomPannel->Attach(night.mSolar);
        BottomPannel->Attach(night.mOutSideTemp);
    root->Attach(BottomPannel);

    root->Attach(new DisplayClock(mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,false));
    root->Attach(new DisplaySystemStatus(mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,0.1f,false));
    night.mWeather = new DisplayWeather(pGraphics,mPath,mBigFont,mNormalFont,mMiniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS,false);
    root->Attach(night.mWeather);

    return root;
}

std::vector<openmeteo::Hourly> MyUI::LoadWeather()
{
    const std::string openMeta =
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=51.50985954887405&"
        "longitude=-0.12022833383470222&"
        "hourly=temperature_2m,precipitation_probability,weather_code,cloud_cover,visibility,wind_speed_10m,is_day";

    const std::string weatherJson = DownloadJson(openMeta,"Weather");
    openmeteo::OpenMeteo weather(weatherJson);
    return weather.GetForcast();
}

bool MyUI::GetIsDay()const
{
    std::time_t currentTime = std::time(nullptr);
    tm myTM = *gmtime(&currentTime);

    for( openmeteo::Hourly h : mForcast )
	{
		if( h.ctime.tm_mday == myTM.tm_mday &&
			h.ctime.tm_mon == myTM.tm_mon &&
			h.ctime.tm_year == myTM.tm_year &&
			h.ctime.tm_hour == myTM.tm_hour)
		{
			return h.is_day;
		}
	}
    return true;
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
