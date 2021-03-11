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

#include <cstdlib>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <cstdarg>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Tiny2D.h"
#include "ClockDisplay.h"
#include "TaskDisplay.h"
#include "TheWeather.h"
#include "Icons.h"

#include "TinyPNG.h"

/**
 * @brief Gets the uptime of the system
 * 
 * @return true If it found all the data.
 * @return false Something is missing.
 */
static bool GetUptime(uint64_t& rUpDays,uint64_t& rUpHours,uint64_t& rUpMinutes)
{
    rUpDays = 0;
    rUpHours = 0;
    rUpMinutes = 0;

    std::ifstream upTimeFile("/proc/uptime");
    if( upTimeFile.is_open() )
    {
        rUpDays = 999;
        char buf[256];
        buf[255] = 0;
        if( upTimeFile.getline(buf,255,' ') )
        {
            const uint64_t secondsInADay = 60 * 60 * 24;
            const uint64_t secondsInAnHour = 60 * 60;
            const uint64_t secondsInAMinute = 60;

            const uint64_t totalSeconds = std::stoull(buf);
            rUpDays = totalSeconds / secondsInADay;
            rUpHours = (totalSeconds - (rUpDays*secondsInADay)) / secondsInAnHour;
            rUpMinutes = (totalSeconds - (rUpDays*secondsInADay) - (rUpHours * secondsInAnHour) ) / secondsInAMinute;
            return true;
        }
    }

    return true;
}

static bool DirectoryExists(const char* pDirname)
{
// Had and issue where a path had an odd char at the end of it. So do this to make sure it's clean.
    struct stat dir_info;
    if( stat(pDirname, &dir_info) == 0 )
    {
        return S_ISDIR(dir_info.st_mode);
    }
    return false;
}

int main(int argc, char *argv[])
{
// Display the constants defined by app build. \n";
    std::cout << "Application Version " << APP_VERSION << '\n';
    std::cout << "Build date and time " << APP_BUILD_DATE_TIME << '\n';
    std::cout << "Build date " << APP_BUILD_DATE << '\n';
    std::cout << "Build time " << APP_BUILD_TIME << '\n';

    // Crude argument list handling.
    std::string path = "./";
    std::string taskFile = "example.json";
    if( (argc == 2 || argc == 3) && DirectoryExists(argv[1]) )
    {
        path = argv[1];
        if( path.back() != '/' )
            path += '/';
    }

    if( argc == 3 )
    {
        taskFile = argv[2];
    }

    std::time_t result = std::time(nullptr);
    tm currentTime = *localtime(&result);

    TaskDisplay theTasks(path + "liberation_serif_font/");
    if( theTasks.LoadTaskList(path + taskFile) == false )
    {
        std::cerr << "Failed to read task file\n";
       	return EXIT_FAILURE;
    }

    tiny2d::FrameBuffer* FB = tiny2d::FrameBuffer::Open();
	if( !FB )
    	return EXIT_FAILURE;

    tiny2d::DrawBuffer RT(FB);

    // Load a background, later I'll make this configurable.
    tiny2d::DrawBuffer Background;

    tinypng::Loader bg;
    if( bg.LoadFromFile(path + "images/bg-pastal-01.png") )
    {
        Background.Resize(bg.GetWidth(),bg.GetHeight());
        bg.GetRGB(Background.mPixels);
    }
    else
    {
        // Do something default.
        Background.Resize(RT.GetWidth(),RT.GetHeight());
        uint8_t* pixels = Background.mPixels.data();
        for(size_t n = 0 ; n < Background.mPixels.size() ; n += 3, pixels += 3 )
        {
            pixels[0] = 100;
            pixels[1] = 100;
            pixels[2] = 100;
        }
    }

    Icons WeatherIcons(path);
    
    tiny2d::FreeTypeFont StatsFont(path + "liberation_serif_font/LiberationSerif-Bold.ttf");

    ClockDisplay theClock(path + "liberation_serif_font/");
    theClock.SetForground(255,255,255);
    theClock.SetBackground(0,0,0);

    TheWeather weather(theTasks.GetWeatherApiKey());

    while( FB->GetKeepGoing() )
    {
        // See if day has changed.
        std::time_t theTimeUTC = std::time(nullptr);
        const tm *now = localtime(&theTimeUTC);
        if( now != nullptr )
        {
            currentTime = *now;
        }

        RT.Blit(Background,0,0);

        weather.Update(theTimeUTC);
        theClock.Update(RT,20,20,currentTime);
        theTasks.Update(RT,20,450,currentTime);

        // Render the weather icons.
        WeatherIcons.RenderWeatherForcast(RT,280,currentTime,weather);

        // Render the uptime
        uint64_t upDays,upHours,upMinutes;
        if( GetUptime(upDays,upHours,upMinutes) )
        {
            RT.FillRoundedRectangle(650,2,RT.GetWidth()-2,80,20,255,255,255);
            RT.FillRoundedRectangle(654,6,RT.GetWidth()-6,76,18,20,30,180);
            StatsFont.Printf(RT,700,50,"Uptime: %lld:%02lld:%02lld",upDays,upHours,upMinutes);
        }

        FB->Present(RT);
        sleep(1);
    }

    delete FB;

    return EXIT_SUCCESS;
}
