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
#include "DisplayTask.h"

#include <ctime>
#include <string>
#include <array>
#include <algorithm>    // std::sort
#include <iostream>
#include <fstream>
#include <sstream>


DisplayTask::DisplayTask(tinygles::GLES& GL,const std::string& pFontPath):
    mFont(GL.FontLoad(pFontPath + "/LiberationSerif-Bold.ttf",50))
{
}

DisplayTask::~DisplayTask()
{
    // Clean up.
    for(auto t : mTheTasks )
    {
        delete t;
    }
}

bool DisplayTask::LoadTaskList(const std::string& pFilename)
{
// Allows me to have my own personal file with personal data in it not in the repo.
// Defined in the x11 project settings.
#ifdef DEVELOPMENT_TASK_FILE
    const std::string filename = DEVELOPMENT_TASK_FILE;
    std::cout << "Using project overide for task file, " << filename << "\n";
#else
    const std::string filename = pFilename;
#endif

    std::ifstream jsonFile(filename);
    if( jsonFile.is_open() )
    {
        std::stringstream jsonStream;
        jsonStream << jsonFile.rdbuf();// Read the whole file in...

   		tinyjson::JsonProcessor json(jsonStream.str(),true);
		const tinyjson::JsonValue projectRoot = json.GetRoot();

        mWeatherApiKey = projectRoot.GetString("weather-api-key");

        // I am not checking the format here for code simplicity. I am creating the input file.
        for( const auto& val : projectRoot["tasks"].mArray )
        {
            const std::string what = val["what"].GetString();
            const std::string when = val["when"].GetString();
            const u_int8_t fg_r = val["fg_colour"][0].GetInt();
            const u_int8_t fg_b = val["fg_colour"][1].GetInt();
            const u_int8_t fg_g = val["fg_colour"][2].GetInt();
            const u_int8_t bg_r = val["bg_colour"][0].GetInt();
            const u_int8_t bg_b = val["bg_colour"][1].GetInt();
            const u_int8_t bg_g = val["bg_colour"][2].GetInt();

            int hour,minute;
            sscanf(when.c_str(),"%d:%d",&hour,&minute);

            mTheTasks.push_back(new Task{what,hour,minute,fg_r,fg_g,fg_b,bg_r,bg_g,bg_b});

            // Now sort in reverse order, latest first. Makes finding the current task way easier.
            std::sort(mTheTasks.begin(),mTheTasks.end(),[](const Task *a,const Task *b)
            {
                if( a->whenHour == b->whenHour )
                    return a->whenMinute > b->whenMinute;

                return a->whenHour > b->whenHour;
            });
        }
        return mTheTasks.size() > 0;
    }

    return false;
}

void DisplayTask::Update(tinygles::GLES& GL,int pX,int pY,const tm& pCurrentTime)
{
    const int hour = pCurrentTime.tm_hour;
    const int minute = pCurrentTime.tm_min;
    const int Xpadding = 20;

    int tillHour,tillMinute;
    uint8_t rTillR,rTillG,rTillB;
    const Task* theTask = GetCurrentTask(hour,minute,tillHour,tillMinute,rTillR,rTillG,rTillB);
    if( theTask != NULL )
    {
//        mFont.SetPenColour(theTask->fg_r,theTask->fg_g,theTask->fg_b);
//        mFont.SetBackgroundColour(theTask->bg_r,theTask->bg_g,theTask->bg_b);

        // Draw box for all the text
        GL.FillRoundedRectangle(Xpadding,pY,GL.GetWidth()-1-Xpadding,GL.GetHeight()+40,35,theTask->bg_r,theTask->bg_g,theTask->bg_b);

        // Draw the progress line.
        const float fromTotal =  ((theTask->whenHour * 60) + theTask->whenMinute) * 60;
        const float tillTotal =  ((tillHour * 60) + tillMinute) * 60;
        const float nowTotal  = (((hour * 60) + minute) * 60) + pCurrentTime.tm_sec;

        // "1.0f -" bit is to inverter result so it goes from left to right. :)
        float progress = 0.0f;
        if( (tillTotal - fromTotal) > 0 )
        {
            progress = 1.0f - ((tillTotal - nowTotal) / (tillTotal - fromTotal));
        }

        const int progressX = Xpadding + 4 + ((GL.GetWidth()-1-Xpadding-Xpadding-8) * progress);
        const int progressY = GL.GetHeight();
        GL.FillRectangle(Xpadding,progressY-8,progressX,progressY,rTillR,rTillG,rTillB);
        GL.FillRectangle(progressX,progressY-8,GL.GetWidth()-1-Xpadding,progressY,theTask->bg_r,theTask->bg_g,theTask->bg_b);
        GL.FillRectangle(progressX-4,progressY-8,progressX+4,progressY,theTask->fg_r,theTask->fg_g,theTask->fg_b);// the tick

        // Draw the text.
        GL.FontPrint(mFont,pX + 4 + Xpadding,GL.GetHeight() - 28,theTask->what.c_str());
        GL.FontPrintf(mFont,pX + 4 + Xpadding,pY + 50,"%d:%02d to %d:%02d",theTask->whenHour,theTask->whenMinute,tillHour,tillMinute);
    }
}

const Task* DisplayTask::GetCurrentTask(int pHour,int pMinute,int& rTillHour,int& rTillMinute,uint8_t& rTillR,uint8_t& rTillG,uint8_t& rTillB)
{
    rTillHour = 23;
    rTillMinute = 59;

    const Task* previous = nullptr;

    for( auto t : mTheTasks )
    {
        if( t->whenHour <= pHour && t->whenMinute <= pMinute )
        {
            if( previous )
            {
                rTillHour = previous->whenHour;
                rTillMinute = previous->whenMinute;
                rTillR = previous->bg_r;
                rTillG = previous->bg_g;
                rTillB = previous->bg_b;
            }

            return t;
        }

        previous = t;
    }

    // Nowt found, show first one. (which is the last one) 
    return mTheTasks[0];
}
