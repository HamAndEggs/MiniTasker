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

#include <ctime>
#include <string>
#include <array>

#include "TinyJson.h"
#include "DisplayWeather.h"

DisplayWeather::DisplayWeather(const std::string& pPath) : 
    mIconFont(pPath + "liberation_serif_font/LiberationSerif-Bold.ttf",30),
    mTemperatureFont( pPath + "liberation_serif_font/LiberationSerif-Regular.ttf",60)
{
    mTemperatureFont.SetPenColour(0,0,0);
}

void DisplayWeather::RenderWeatherForcast(tiny2d::DrawBuffer& RT,int pY,const tm& pCurrentTime,const TheWeather& pWeather,const Icons& pTheIcons)
{
     // Show next six icons.
    // I could render this to an offscreen image and only update once an hour.
    // But for now, render each time.
    const getweather::HourlyIconVector icons = pWeather.GetNextHourlyIcons();
    int n = 0;
    int x = (RT.GetWidth()/2) - ((pTheIcons.GetIconWidth()*6) / 2);
    int y = pY;
    for( const auto& icon : icons )
    {
        RT.Blend(pTheIcons.GetIconBG(),x,y);
        RT.Blend(pTheIcons.GetIcon(icon.second),x-20,y-10);

        // https://www.npl.co.uk/resources/q-a/is-midnight-12am-or-12pm
        std::string hour; 
        if( icon.first == 0 )
        {// Special case zero houndred, IE 12am...
            hour = "Midnight";
        }
        else if( icon.first < 12 )
        {
            hour = std::to_string(icon.first) + "am";
        }
        else if( icon.first == 12 )
        {// Special case 12 houndred, IE 12pm, or am, or pm.......
            hour = "Midday";
        }
        else
        {
            hour = std::to_string(icon.first-12) + "pm";
        }

        mIconFont.Print(RT,x+12,y+30,hour.c_str());

        x += pTheIcons.GetIconWidth();
        n++;

        if( n == 6 )
        {// Sorry, this is crap. ;)
            break;
        }
    }

    // Draw temperature, if we have one.
    const std::string temperature = pWeather.GetCurrentTemperature();
    if( temperature.size() > 0 )
    {
        mTemperatureFont.Print(RT,RT.GetWidth() - 200,y - 20,temperature.c_str());
    }
}
