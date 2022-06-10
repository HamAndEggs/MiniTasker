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

#include "Graphics.h"
#include "Element.h"


#include "DisplayClock.h"
#include "DisplayWeather.h"
#include "DisplaySystemStatus.h"
#include "DisplayAirQuality.h"
#include "DisplayBitcoinPrice.h"

#include <unistd.h>
#include <filesystem>

const float CELL_PADDING = 0.02f;
const float RECT_RADIUS = 0.2f;
const float BORDER_SIZE = 3.0f;

int main(int argc, char *argv[])
{
// Crude argument list handling.
    std::string path = "./";

    std::string taskFile = "task-file.json";
    if( argc == 2 && std::filesystem::directory_entry(argv[1]).exists() )
    {
        path = argv[1];
        if( path.back() != '/' )
            path += '/';
    }


    eui::Graphics* graphics = eui::Graphics::Open();

    eui::ElementPtr mainScreen = eui::Element::Create();
    mainScreen->SetID("mainScreen");
    mainScreen->SetGrid(3,3);

    int miniFont = graphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Regular.ttf",20);
    int normalFont = graphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Regular.ttf",35);
    int bigFont = graphics->FontLoad(path + "liberation_serif_font/LiberationSerif-Bold.ttf",130);
    mainScreen->SetFont(normalFont);
    mainScreen->GetStyle().mTexture = graphics->TextureLoadPNG(path + "images/bg-pastal-01.png");
    mainScreen->GetStyle().mBackground = eui::COLOUR_WHITE;

    mainScreen->Attach(new DisplayClock(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    mainScreen->Attach(new DisplayBitcoinPrice(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    mainScreen->Attach(new DisplayWeather(graphics,path,bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    eui::ElementPtr status = eui::Element::Create();
    status->SetGrid(1,2);
    status->SetPos(2,0);
    mainScreen->Attach(status);
    status->Attach(new DisplaySystemStatus(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));
    status->Attach(new DisplayAirQuality(bigFont,normalFont,miniFont,CELL_PADDING,BORDER_SIZE,RECT_RADIUS));

    // Use dependency injection to pass events onto the controls.
    // This means that we don't need a circular header dependency that can make it hard to port code.
    // I do not want graphics.h including element.h as element.h already includes graphics.h
    auto touchEventHandler = [mainScreen](int32_t pX,int32_t pY,bool pTouched)
    {
        return mainScreen->TouchEvent(pX,pY,pTouched);
    };

    while( graphics->ProcessSystemEvents(touchEventHandler) )
    {
        mainScreen->Update();

        graphics->BeginFrame();

        mainScreen->Draw(graphics);

        graphics->EndFrame();

        // Check again in a second. Not doing big wait here as I need to be able to quit in a timely fashion.
        // Also OS could correct display. But one second means system not pegged 100% rendering as fast as possible.
        sleep(1);
    }

    delete mainScreen;
    eui::Graphics::Close();

    return EXIT_SUCCESS;
}
