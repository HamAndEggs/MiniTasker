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

TaskDisplay::TaskDisplay():
    mFont("./liberation_serif_font/LiberationSerif-Bold.ttf",50,true)
{
    mCurrentTask = NULL;
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
    const Task* newTask = FindNextTask();
    if( newTask != NULL && mCurrentTask != newTask )
    {
        mCurrentTask = newTask;

        mFont.SetPenColour(mCurrentTask->fg_r,mCurrentTask->fg_g,mCurrentTask->fg_b);
        mFont.SetBackgroundColour(mCurrentTask->bg_r,mCurrentTask->bg_g,mCurrentTask->bg_b);

        pFB->DrawRectangle(pX,pY,pX+800,pY+140,mCurrentTask->bg_r,mCurrentTask->bg_g,mCurrentTask->bg_b,true);

        mFont.Print(pFB,pX + 4,pY + 120,mCurrentTask->what.c_str());
        mFont.Printf(pFB,pX + 4,pY + 40,"%d:%02d",mCurrentTask->whenHour,mCurrentTask->whenMinute);
    }
}

const Task* TaskDisplay::FindNextTask()
{
    std::time_t result = std::time(nullptr);
    tm local_tm = *localtime(&result);

    // Find the first to be 

    const int hour = local_tm.tm_hour;
    const int minute = local_tm.tm_min;

    for( auto t : mTheTasks )
    {
        if( t->whenHour <= hour && t->whenMinute < minute )
            return t;
    }

    // Nowt found, show first one. (which is the last one) 
    return mTheTasks[0];
}
