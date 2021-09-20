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

#include "TinyGLES.h"
#include "TinyTools.h"

#include "DisplayClock.h"
#include "DisplayTask.h"
#include "DisplayWeather.h"
#include "DisplaySystemStatus.h"

#include "TheWeather.h"
#include "Icons.h"

#include "TinyPNG.h"

#include "MQTTData.h"

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

    tinygles::GLES GL(true);
    GL.FontSetMaximumAllowedGlyph(256);

    DisplayTask theTasks(GL,path + "liberation_serif_font/");
    
    if( theTasks.LoadTaskList(path + taskFile) == false )
    {
        std::cerr << "Failed to read task file\n";
       	return EXIT_FAILURE;
    }

    // Load a background, later I'll make this configurable.
    uint32_t background = 0;

    tinypng::Loader bg;
    if( bg.LoadFromFile(path + "images/bg-pastal-01.png") )
    {
        std::vector<uint8_t> RGB;
        bg.GetRGB(RGB);
        background = GL.CreateTexture(bg.GetWidth(),bg.GetHeight(),RGB.data(),tinygles::TextureFormat::FORMAT_RGB);
    }
    else
    {
        background = GL.GetDiagnosticsTexture(); // used default development texture
    }

    Icons someIcons(GL,path);
    DisplayWeather theWeather(GL,path);
    DisplaySystemStatus systemStatus(GL,path);
    

    DisplayClock theClock(GL,path);
    theClock.SetForground(255,255,255);

    TheWeather weatherData(theTasks.GetWeatherApiKey());

    // Get our MQTT data stream so we can collect stuff from other systems.
    std::string outsideTemperature = "Unknown";
    std::time_t outsideTemperatureDelivered = 0;
    const std::vector<std::string>& topics =
    {
        "/outside/temperature"
    };

    MQTTData MQTT("server",1883,topics,[&outsideTemperature,&outsideTemperatureDelivered](const std::string &pTopic,const std::string &pData)
    {
        if( pTopic == "/outside/temperature" )
        {
            outsideTemperature = pData;
            outsideTemperatureDelivered = std::time(nullptr);
        }
    });

    if( MQTT.GetOK() == false )
    {
        std::cerr << "Failed to start MQTT\n";
       	return EXIT_FAILURE;
    }

    while( GL.BeginFrame() )
    {
        std::time_t theTimeUTC = std::time(nullptr);
        const tm *now = localtime(&theTimeUTC);
        if( now != nullptr )
        {
            currentTime = *now;
        }

        GL.Clear(background);

        weatherData.Update(theTimeUTC);
        theClock.Update(20,20,currentTime);
        theTasks.Update(20,450,currentTime);

        // If been an hour since last update put an marker at end of string..
        std::string outTemp = outsideTemperature + "C";
        if( outsideTemperatureDelivered + (60*60) < theTimeUTC )
        {
            outTemp += "*";
        }

        // Render the weather forcast.
        theWeather.RenderWeatherForcast(280,currentTime,weatherData,someIcons,outsideTemperature);

        // Draw some intresting system status stuff.
        systemStatus.Render(650,2);

        // Always redraw FB, something on the OS may have don't something I can't stop.
        // Also handles user input.
        GL.EndFrame();

        // Check again in a second. Not doing big wait here as I need to be able to quit in a timely fashion.
        // Also OS could correct display. But one second means system not pegged 100% rendering as fast as possible.
        sleep(1);
    }

    return EXIT_SUCCESS;
}
