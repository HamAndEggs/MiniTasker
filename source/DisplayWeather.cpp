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


DisplayWeather::DisplayWeather(tinygles::GLES& pGL,const std::string& pPath) : 
    mIconFont(pGL.FontLoad(pPath + "liberation_serif_font/LiberationSerif-Bold.ttf",30)),
    mTemperatureFont(pGL.FontLoad(pPath + "liberation_serif_font/LiberationSerif-Bold.ttf",70)),
    GL(pGL)
{
}

void DisplayWeather::RenderWeatherForcast(int pY,const tm& pCurrentTime,const TheWeather& pWeather,const Icons& pTheIcons)
{
     // Show next six icons.
    // I could render this to an offscreen image and only update once an hour.
    // But for now, render each time.
    const tinyweather::HourlyIconVector icons = pWeather.GetNextHourlyIcons();
    int n = 0;
    int x = (GL.GetWidth()/2) - ((pTheIcons.GetIconWidth()*6) / 2);
    int y = pY;
    for( const auto& icon : icons )
    {
        GL.Blit(pTheIcons.GetIconBG(),x,y,0,0,0,200);
        GL.Blit(pTheIcons.GetIcon(icon.icon),x-20,y-10);

        // https://www.npl.co.uk/resources/q-a/is-midnight-12am-or-12pm
        std::string hour; 
        if( icon.hour == 0 )
        {// Special case zero houndred, IE 12am...
            hour = "Midnight";
        }
        else if( icon.hour < 12 )
        {
            hour = std::to_string(icon.hour) + "am";
        }
        else if( icon.hour == 12 )
        {// Special case 12 houndred, IE 12pm, or am, or pm.......
            hour = "Midday";
        }
        else
        {
            hour = std::to_string(icon.hour-12) + "pm";
        }

        {
            char buf[64];
            snprintf(buf,sizeof(buf),"%03.1fC",tinytools::math::RoundToPointFive(icon.temprature));
            GL.FontPrint(mIconFont,x+20,y+154,buf);
        }

        GL.FontPrint(mIconFont,x+14,y+36,hour.c_str());

        x += pTheIcons.GetIconWidth();
        n++;

        if( n == 6 )
        {// Sorry, this is crap. ;)
            break;
        }
    }
}

int DisplayWeather::RenderTemperature(int pX,int pY,const std::string& pTemperature)
{
    const int width = 140 + ((pTemperature.size()-1)*22);
    const int x = pX - width;
    if( pTemperature.size() > 0 )
    {
        GL.RoundedRectangle(x,pY,pX-20,pY+70,12,255,255,255,130,true);
        GL.FontSetColour(mTemperatureFont,0,0,0);
        GL.FontPrint(mTemperatureFont,x + 10,pY + 60,pTemperature.c_str());
    }
    return x;
}