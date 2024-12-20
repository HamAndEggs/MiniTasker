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
#include "FileDownload.h"

#include <ctime>
#include <time.h>
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

class WeatherIcon : public eui::Element
{
    eui::ElementPtr image,time,temperature;
    const bool mDayDisplay;

public:

    WeatherIcon(float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS,bool pDayDisplay):mDayDisplay(pDayDisplay)
    {
        SET_DEFAULT_ID();

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

        if( mDayDisplay )
        {
            eui::Style s;
            s.mBackground = eui::MakeColour(0,0,0,200);
            s.mRadius = RECT_RADIUS;
            SetStyle(s);
        }
        else
        {
            eui::Style s;
            s.mBorder = eui::COLOUR_DARK_GREY;
            s.mThickness = 3;
            s.mRadius = RECT_RADIUS;
            SetStyle(s);

            time->GetStyle().mForeground = eui::COLOUR_GREY;
            temperature->GetStyle().mForeground = eui::COLOUR_GREY;

        }        
    }

    void SetInfo(uint32_t pIcon,const std::string& pTime,float pTemperature,bool is_day)
    {
        if( image )
        {
            image->GetStyle().mTexture = pIcon;
            if( mDayDisplay )
            {
                if( is_day )
                {
                    GetStyle().mBackground = eui::MakeColour(50,50,150,200);
                }
                else
                {
                    GetStyle().mBackground = eui::MakeColour(0,0,0,200);
                }
            }
            time->SetText(pTime);
            temperature->SetTextF("%03.1fC",tinytools::math::RoundToPointFive(pTemperature));
        }
    }
};

DisplayWeather::DisplayWeather(eui::Graphics* graphics,const std::string& pPath,int pBigFont,int pNormalFont,int pMiniFont,float CELL_PADDING,float BORDER_SIZE,float RECT_RADIUS,bool pDayDisplay) :
    mHasWeather(false),
    mFirstFail(true),
    mFetchLimiter(0),
    mHourlyUpdates(0)
{
    SET_DEFAULT_ID();

    LoadWeatherIcons(graphics,pPath);

    this->SetPos(0,1);
    this->SetGrid(4,1);
    this->SetSpan(3,1);

    for( int n = 0 ; n < 4 ; n++ )
    {
        icons[n] = new WeatherIcon(CELL_PADDING,BORDER_SIZE,RECT_RADIUS,pDayDisplay);
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
        if( LoadWeather() )
        {
            std::clog << "Fetched weather data\n";
            mHasWeather = true;

            // It worked, do the next fetch in a days time.
            mFetchLimiter = currentTime + ONE_DAY;
            // Now round to start of day plus one minute to be safe, 00:01. The weather forcast may have changed. Also if we boot in the evening don't want all downloads at the same time every day.
            mFetchLimiter -= (mFetchLimiter%ONE_DAY);
            mFetchLimiter += ONE_MINUTE;

            // Reset this time out to force a rebuild.
            mHourlyUpdates = 0;
        }
    }

    if( mHasWeather )
    {
        // Rebuild the Next Hourly Icons vector so its always correct an hour after the last time.
        if( mHourlyUpdates < currentTime )
        {
            std::clog << "Updating weather display\n";

            try
            {
                std::time_t t = currentTime;
                for( int n = 0 ; n < 4 ; n++, t += (60*60) )
                {
                    openmeteo::Hourly Hourly;
                    if( DisplayWeather::GetForcast(t,Hourly) )
                    {
                        if( n == 0 )
                        {
                            mIsDay = Hourly.is_day;
                        }
                        // https://www.npl.co.uk/resources/q-a/is-midnight-12am-or-12pm
                        std::string hour;
                        if( Hourly.ctime.tm_hour == 0 )
                        {// Special case zero hundred, IE 12am...
                            hour = "Midnight";
                        }
                        else if( Hourly.ctime.tm_hour < 12 )
                        {
                            hour = std::to_string(Hourly.ctime.tm_hour) + "am";
                        }
                        else if( Hourly.ctime.tm_hour == 12 )
                        {// Special case 12 hundred, IE 12pm, or am, or pm.......
                            hour = "Midday";
                        }
                        else
                        {
                            hour = std::to_string(Hourly.ctime.tm_hour-12) + "pm";
                        }

                        if( WeatherIcons.find(Hourly.icon_code) != WeatherIcons.end() )
                        {
                            icons[n]->SetInfo(WeatherIcons[Hourly.icon_code],hour,Hourly.temperature_2m,Hourly.is_day);
                        }
                    }
                }
                mHourlyUpdates = NextHour(currentTime);
            }
            catch( const std::runtime_error &e)
            {
                std::cerr << "Something went wrong reading the wearth\n";
            }            
        }
    }


    return true;
}

void DisplayWeather::LoadWeatherIcons(eui::Graphics* graphics,const std::string& pPath)
{
    for( std::string f : files )
    {
        WeatherIcons[f] = graphics->TextureLoad(pPath + "icons/" + f + ".png");
    }
}

uint32_t DisplayWeather::GetRandomIcon()
{
    return WeatherIcons[files[rand()%files.size()]];
}

bool DisplayWeather::LoadWeather()
{
    const std::string openMeta =
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=51.50985954887405&"
        "longitude=-0.12022833383470222&"
        "hourly=temperature_2m,precipitation_probability,weather_code,cloud_cover,visibility,wind_speed_10m,is_day";

    const std::string weatherJson = DownloadJson(openMeta,"Weather");
    openmeteo::OpenMeteo weather(weatherJson);
    mForcast = weather.GetForcast();
    return true;
}

bool DisplayWeather::GetForcast(std::time_t theTime,openmeteo::Hourly& found)const
{
    tm myTM = *gmtime(&theTime);

    for( openmeteo::Hourly h : mForcast )
	{
		if( h.ctime.tm_mday == myTM.tm_mday &&
			h.ctime.tm_mon == myTM.tm_mon &&
			h.ctime.tm_year == myTM.tm_year &&
			h.ctime.tm_hour == myTM.tm_hour)
		{
            found = h;
			return true;
		}
	}
    return false; 
}
