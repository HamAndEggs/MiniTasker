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
#include "DisplayWeather.h"

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

static std::string ReadWeatherKey(const std::string& pPath)
{
    std::string key;
    try
    {
        key = tinytools::file::LoadFileIntoString(pPath + "weather.key");
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "Could not load weather key from current folder, trying hard coded path\n";
    }

    if( key.size() == 0 )
    {
        try
        {
            key = tinytools::file::LoadFileIntoString("/usr/share/mini-tasker/weather.key");
        }
        catch(std::runtime_error &e)
        {
            std::cerr << "Could not load weather key from hard coded path\n";
        }
    }

    if( key.size() > 0 )
    {
        key = tinytools::string::TrimWhiteSpace(key);
    }

    if( key.size() < 4 )
    {
        std::cerr << "Weather key too short, will not work\n";
    }

    return key;
}

class WeatherIcon : public eui::Element
{
    eui::ElementPtr image,time,temperature;

public:

    WeatherIcon(float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS)
    {
        SET_DEFAULT_ID();

        eui::Style s;
            s.mBackground = eui::MakeColour(0,0,0,200);
            s.mRadius = RECT_RADIUS;

        SetStyle(s);
        SetPadding(CELL_PADDING);

        image = new eui::Element;
        image->SetPadding(0.05f);
        image->GetStyle().mBackground = eui::COLOUR_WHITE;
        Attach(image);

        time = new eui::Element;
        time->SetText("XX");
        time->SetPadding(0.07f);
        time->GetStyle().mAlignment = eui::ALIGN_LEFT_TOP;
        Attach(time);

        temperature = new eui::Element;
        temperature->SetText("XX.XC");
        temperature->SetPadding(0.07f);
        temperature->GetStyle().mAlignment = eui::ALIGN_RIGHT_BOTTOM;
        Attach(temperature);
    }

    void SetInfo(uint32_t pIcon,const std::string& pTime,float pTemperature)
    {
        if( image )
        {
            image->GetStyle().mTexture = pIcon;
            time->SetText(pTime);
            temperature->SetTextF("%03.1fC",tinytools::math::RoundToPointFive(pTemperature));
        }
    }    
};

DisplayWeather::DisplayWeather(eui::Graphics* graphics,const std::string& pPath,int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS) :
    mHasWeather(false),
    mFirstFail(true),
    mFetchLimiter(0),
    mHourlyUpdates(0),
    mWeather(ReadWeatherKey(pPath))
{
    SET_DEFAULT_ID();

    LoadWeatherIcons(graphics,pPath);

    this->SetPos(0,1);
    this->SetGrid(6,1);
    this->SetSpan(3,1);
    
    eui::Style s;
    s.mBackground = eui::MakeColour(200,200,200,160);
    s.mThickness = BORDER_SIZE;
    s.mBorder = eui::COLOUR_WHITE;
    s.mRadius = RECT_RADIUS;

    for( int n = 0 ; n < 6 ; n++ )
    {
        icons[n] = new WeatherIcon(CELL_PADDING,BORDER_SIZE,RECT_RADIUS);
        icons[n]->SetPos(n,0);
        this->Attach(icons[n]);
    }
}

DisplayWeather::~DisplayWeather()
{

}

bool DisplayWeather::OnUpdate(const eui::Rectangle& pContentRect)
{
    std::time_t currentTime = std::time(nullptr);

    if( mFetchLimiter < currentTime )
    {
        mHasWeather = false;
        mWeather.Get(50.72824,-1.15244,[this,currentTime](bool pDownloadedOk,const auto &pTheWeather)
        {
            if( pDownloadedOk )
            {
                std::clog << "Fetched weather data " << pTheWeather.mCurrent.mTime.GetDate() << " " << pTheWeather.mCurrent.mTime.GetTime() << "\n";
                mHasWeather = true;

                // It worked, do the next fetch in a days time.
                mFetchLimiter = currentTime + ONE_DAY;
                // Now round to start of day plus one minute to be safe, 00:01. The weather forcast may have changed. Also if we boot in the evening don't want all downloads at the same time every day.
                mFetchLimiter -= (mFetchLimiter%ONE_DAY);
                mFetchLimiter += ONE_MINUTE;

                const tinyweather::WeatherTime nextDownload(mFetchLimiter);
                std::clog << "Next download scheduled for " << nextDownload.GetDate() << " " << nextDownload.GetTime() << "\n";

                // Reset this time out to force a rebuild.
                mHourlyUpdates = 0;
            }
            else
            {
                const tinyweather::WeatherTime now(currentTime);
                std::cerr << "Failed to fetched weather data! " << now.GetDate() << " " << now.GetTime() << "\n";
                mCurrentTemperature = "Failed";
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

    if( mHasWeather )
    {
        // Rebuild the Next Hourly Icons vector so its always correct an hour after the last time.
        if( mHourlyUpdates < currentTime )
        {
            std::stringstream log;

            if( mHourlyUpdates == 0 )
                log << "Updating icons after fetching new weather data. ";
            else
                log << "Updating icons " << GetTimeString(mHourlyUpdates) << ". ";

            try
            {
                // Update in an hour and on the hour.
                mHourlyUpdates = NextHour(currentTime);
                log << "Next icon update at " << GetTimeString(mHourlyUpdates) << ". ";

                mNextHourlyIcons = mWeather.GetHourlyIconCodes(currentTime);

                // Not building for C++20 so can't use std::format yet.... So go old school.
                const float temperature = mWeather.GetHourlyTemperature(currentTime);

                char buf[64];
                snprintf(buf,sizeof(buf),"%03.1fC",tinytools::math::RoundToPointFive(temperature));
                mCurrentTemperature = buf;
            }
            catch( const std::runtime_error &e)
            {
                std::cerr << "Something went wrong reading the wearth\n";
            }
        }
    }

    int n = 0;

    try
    {
        for( const auto& fIcon : mNextHourlyIcons )
        {
            // https://www.npl.co.uk/resources/q-a/is-midnight-12am-or-12pm
            std::string hour; 
            if( fIcon.hour == 0 )
            {// Special case zero hundred, IE 12am...
                hour = "Midnight";
            }
            else if( fIcon.hour < 12 )
            {
                hour = std::to_string(fIcon.hour) + "am";
            }
            else if( fIcon.hour == 12 )
            {// Special case 12 hundred, IE 12pm, or am, or pm.......
                hour = "Midday";
            }
            else
            {
                hour = std::to_string(fIcon.hour-12) + "pm";
            }

            if( WeatherIcons.find(fIcon.icon) != WeatherIcons.end() )
            {
                icons[n]->SetInfo(WeatherIcons[fIcon.icon],hour,fIcon.temperature);
            }

            n++;
            if( n == 6 )
            {// Sorry, this is crap. ;)
                break;
            }
        }
    }
    catch( const std::runtime_error &e)
    {
        std::cerr << "Something went wrong reading the wearth\n";
    }
    return true;
}

void DisplayWeather::LoadWeatherIcons(eui::Graphics* graphics,const std::string& pPath)
{
    for( std::string f : files )
    {
        WeatherIcons[f] = graphics->TextureLoadPNG(pPath + "icons/" + f + ".png");
    }
}

uint32_t DisplayWeather::GetRandomIcon()
{
    return WeatherIcons[files[rand()%files.size()]];
}