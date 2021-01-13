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

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <cstdarg>
#include <string.h>


#include "framebuffer.h"
#include "ClockDisplay.h"
#include "TaskDisplay.h"

bool KeepGoing = true;
static void CtrlHandler(int SigNum)
{
	static int numTimesAskedToExit = 0;
	std::cout << std::endl << "Asked to quit, please wait" << std::endl;
	if( numTimesAskedToExit > 2 )
	{
		std::cout << "Asked to quit to many times, forcing exit in bad way" << std::endl;
		exit(1);
	}
	KeepGoing = false;
}

int main(int argc, char *argv[])
{
// Display the constants defined by app build. \n";
    std::cout << "Application Version " << APP_VERSION << '\n';
    std::cout << "Build date and time " << APP_BUILD_DATE_TIME << '\n';
    std::cout << "Build date " << APP_BUILD_DATE << '\n';
    std::cout << "Build time " << APP_BUILD_TIME << '\n';

    FBIO::FrameBuffer* FB = FBIO::FrameBuffer::Open(true);
	if( FB )
    {
        signal (SIGINT,CtrlHandler);

        FB->ClearScreen(0,0,0);

        TaskDisplay theTasks;
        if( theTasks.LoadTaskList("./my-tasks.json") )
        {
            std::cerr << "Failed to load task list\n";

            ClockDisplay theClock;
            theClock.SetForground(255,255,255);
            theClock.SetBackground(0,0,0);

            while( KeepGoing )
            {
                theClock.Update(FB,20,20);
                theTasks.Update(FB,20,400);
                sleep(1);
            }

            return EXIT_SUCCESS;
        }
        else
        {
            std::cerr << "Failed to read task file\n";
        }
        
    }

	return EXIT_FAILURE;
}
