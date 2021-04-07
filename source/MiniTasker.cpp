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

#include "Tiny2D.h"
#include "TinyTools.h"

#include "DisplayClock.h"
#include "DisplayTask.h"
#include "DisplayWeather.h"

#include "TheWeather.h"
#include "Icons.h"

#include "TinyPNG.h"

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
#include <filesystem>

int main(int argc, char *argv[])
{
// Display the constants defined by app build. \n";
    std::cout << "Application Version " << APP_VERSION << '\n';
    std::cout << "Build date and time " << APP_BUILD_DATE_TIME << '\n';
    std::cout << "Build date " << APP_BUILD_DATE << '\n';
    std::cout << "Build time " << APP_BUILD_TIME << '\n';

    // Crude argument list handling.
    std::string path = "./";

    std::string taskFile = "task-file.json";
    if( (argc == 2 || argc == 3) && std::filesystem::directory_entry(argv[1]).exists() )
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

    DisplayTask theTasks(path + "liberation_serif_font/");
    
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

        std::vector<uint8_t> RGB;
        bg.GetRGB(RGB);

        Background.BlitRGB(RGB.data(),0,0,bg.GetWidth(),bg.GetHeight());
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

    Icons someIcons(path);
    DisplayWeather theWeather(path);
    
    tiny2d::FreeTypeFont StatsFont(path + "liberation_serif_font/LiberationSerif-Bold.ttf");
    tiny2d::FreeTypeFont StatsFontSmall(path + "liberation_serif_font/LiberationSerif-Bold.ttf",20);

    DisplayClock theClock(path + "liberation_serif_font/");
    theClock.SetForground(255,255,255);
    theClock.SetBackground(0,0,0);

    TheWeather weatherData(theTasks.GetWeatherApiKey());
    int lastMinute = -1;// Only redraw once a minute.

    while( FB->GetKeepGoing() )
    {
        // See if day has changed.
        std::time_t theTimeUTC = std::time(nullptr);
        const tm *now = localtime(&theTimeUTC);
        if( now != nullptr )
        {
            currentTime = *now;
        }

        // I redraw on the minute, not every minute. That is at 00 seconds. Important for time looking right!
        // I will need to change this to draw when something changes. Like the minute or net work status.
        if( lastMinute != currentTime.tm_min )
        {
            // Redraw the off screen display buffer
            lastMinute = currentTime.tm_min;
            RT.Blit(Background,0,0);

            weatherData.Update(theTimeUTC);
            theClock.Update(RT,20,20,currentTime);
            theTasks.Update(RT,20,450,currentTime);

            // Render the weather forcast.
            theWeather.RenderWeatherForcast(RT,280,currentTime,weatherData,someIcons);

            // Render the uptime
            uint64_t upDays,upHours,upMinutes;
            if( tinytools::system::GetUptime(upDays,upHours,upMinutes) )
            {
                const int y = 2;
                const int border = 4;
                const int Height = 100;
                RT.FillRoundedRectangle(650,y,RT.GetWidth()-2,y + Height,20,255,255,255);
                RT.FillRoundedRectangle(654,y+border,RT.GetWidth()-6,y + Height - border,18,20,30,180);
                StatsFont.Printf(RT,680,40,"Uptime: %lld:%02lld:%02lld",upDays,upHours,upMinutes);

                StatsFontSmall.Print(RT,680,70,tinytools::network::GetLocalIP());
                StatsFontSmall.Print(RT,680,90,tinytools::network::GetHostName());
            }
        }

        // Always redraw FB, something on the OS may have don't something I can't stop.
        // Also handles user input.
        FB->Present(RT);

        // Check again in a second. Do doing big wait here as I need to be able to quit in a timely fashion.
        sleep(1);
    }

    delete FB;

    return EXIT_SUCCESS;
}
