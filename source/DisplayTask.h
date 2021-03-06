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

#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "TinyGLES.h"

#include <vector>

struct Task
{
    const std::string what;
    const int whenHour,whenMinute;
    const u_int8_t fg_r,fg_b,fg_g;
    const u_int8_t bg_r,bg_b,bg_g;
};

class DisplayTask
{
public:
    DisplayTask(tinygles::GLES& pGL,const std::string& pFontPath);
    ~DisplayTask();

    /**
     * @brief Loads in a JSON file that describes the tasks that need to be done.
     * 
     * @param pFilename 
     * @return true 
     * @return false 
     */
    bool LoadTaskList(const std::string& pFilename);

    /**
     * @brief Updates the current task based on the time and if it has changed will redraw it.
     * 
     * @param pFB 
     * @param pX 
     * @param pY 
     */
    void Update(int pX,int pY,const tm& pCurrentTime);

    const std::string& GetWeatherApiKey()const{return mWeatherApiKey;}

private:

    const Task* GetCurrentTask(int pHour,int pMinute,int& rTillHour,int& rTillMinute,uint8_t& rTillR,uint8_t& rTillG,uint8_t& rTillB);

    const uint32_t mFont = 0;
    tinygles::GLES& GL;


    std::vector<const Task*> mTheTasks;

    std::string mWeatherApiKey;
};

#endif //#ifndef DISPLAY_TASK_H
