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
    int tillHour,tillMinute;
    const Task* theTask = GetCurrentTask(tillHour,tillMinute);
    if( theTask != NULL )
    {
        mFont.SetPenColour(theTask->fg_r,theTask->fg_g,theTask->fg_b);
        mFont.SetBackgroundColour(theTask->bg_r,theTask->bg_g,theTask->bg_b);

        pFB->DrawRectangle(0,pY,pFB->GetWidth(),pFB->GetHeight(),theTask->bg_r,theTask->bg_g,theTask->bg_b,true);

        mFont.Print(pFB,pX + 4,pFB->GetHeight() - 20,theTask->what.c_str());
        mFont.Printf(pFB,pX + 4,pY + 40,"%d:%02d to %d:%02d",theTask->whenHour,theTask->whenMinute,tillHour,tillMinute);
    }
}

const Task* TaskDisplay::GetCurrentTask(int& rTillHour,int& rTillMinute)
{
    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    rTillHour = 23;
    rTillMinute = 59;

    // Find the first to be 
    const int hour = local_tm.tm_hour;
    const int minute = local_tm.tm_min;

    const Task* previous = nullptr;

    for( auto t : mTheTasks )
    {
        if( t->whenHour <= hour && t->whenMinute < minute )
        {
            if( previous )
            {
                rTillHour = previous->whenHour;
                rTillMinute = previous->whenMinute;
            }

            return t;
        }

        previous = t;
    }

    // Nowt found, show first one. (which is the last one) 
    return mTheTasks[0];
}
