#include <ctime>
#include <string>
#include <array>
#include <algorithm>    // std::sort

#include <iostream>
#include <fstream>
#include <sstream>


#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include "framebuffer.h"
#include "TaskDisplay.h"

TaskDisplay::TaskDisplay(const std::string& pFontPath):
    mFont(pFontPath + "/LiberationSerif-Bold.ttf",50,true)
{
}

TaskDisplay::~TaskDisplay()
{
    // Clean up.
    for(auto t : mTheTasks )
    {
        delete t;
    }
}

bool TaskDisplay::LoadTaskList(const std::string& pFilename)
{
    std::ifstream jsonFile(pFilename);
    if( jsonFile.is_open() )
    {
        std::stringstream jsonStream;
        jsonStream << jsonFile.rdbuf();// Read the whole file in...

        rapidjson::Document jsonDocument;
        jsonDocument.Parse(jsonStream.str().c_str());

        // Have to invert result as I want true if it worked, false if it failed.
        if( jsonDocument.HasParseError() )
            return false;

        // Now parse the data.
    	const rapidjson::Value& projectRoot = jsonDocument;

        // I am not checking the format here for code simplicity. I am creating the input file.
        for( const auto& val : projectRoot["tasks"].GetArray() )
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

void TaskDisplay::Update(FBIO::FrameBuffer* pFB,int pX,int pY)
{
    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);
    const int hour = local_tm.tm_hour;
    const int minute = local_tm.tm_min;

    int tillHour,tillMinute;
    uint32_t tillColour;
    const Task* theTask = GetCurrentTask(hour,minute,tillHour,tillMinute,tillColour);
    if( theTask != NULL )
    {
        mFont.SetPenColour(theTask->fg_r,theTask->fg_g,theTask->fg_b);
        mFont.SetBackgroundColour(theTask->bg_r,theTask->bg_g,theTask->bg_b);

        // Draw box for all the text
        pFB->DrawRectangle(0,pY,pFB->GetWidth(),pFB->GetHeight(),theTask->bg_r,theTask->bg_g,theTask->bg_b,true);

        // Draw the progress line.
        const float fromTotal =  ((theTask->whenHour * 60) + theTask->whenMinute) * 60;
        const float tillTotal =  ((tillHour * 60) + tillMinute) * 60;
        const float nowTotal  = (((hour * 60) + minute) * 60) + local_tm.tm_sec;

        // "1.0f -" bit is to inverter result so it goes from left to right. :)
        float progress = 1.0f;
        if( (tillTotal - fromTotal) > 0 )
        {
            progress = 1.0f - ((tillTotal - nowTotal) / (tillTotal - fromTotal));
        }

        const int progressX = pFB->GetWidth() * progress;
        pFB->DrawRectangle(0,pY-8,progressX,pY,tillColour,true);
        pFB->DrawRectangle(progressX,pY-8,pFB->GetWidth(),pY,theTask->bg_r,theTask->bg_g,theTask->bg_b,true);
        pFB->DrawRectangle(progressX-4,pY-8,progressX+4,pY,theTask->fg_r,theTask->fg_g,theTask->fg_b,true);// the tick

        // Draw the text.
        mFont.Print(pFB,pX + 4,pFB->GetHeight() - 20,theTask->what.c_str());
        mFont.Printf(pFB,pX + 4,pY + 40,"%d:%02d to %d:%02d",theTask->whenHour,theTask->whenMinute,tillHour,tillMinute);
    }
}

const Task* TaskDisplay::GetCurrentTask(int pHour,int pMinute,int& rTillHour,int& rTillMinute,uint32_t& rTillColour)
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
                rTillColour = MAKE_PIXEL_COLOUR(previous->bg_r,previous->bg_g,previous->bg_b);
            }

            return t;
        }

        previous = t;
    }

    // Nowt found, show first one. (which is the last one) 
    return mTheTasks[0];
}