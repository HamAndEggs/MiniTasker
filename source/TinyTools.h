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

#ifndef TINY_TOOLS_H
#define TINY_TOOLS_H

#include <getopt.h>
#include <assert.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <condition_variable>

/**
 * @brief Adds line and source file. There is a c++20 way now that is better. I need to look at that.
 */
#define TINYTOOLS_THROW(THE_MESSAGE__)	{throw std::runtime_error("At: " + std::to_string(__LINE__) + " In " + std::string(__FILE__) + " : " + std::string(THE_MESSAGE__));}


namespace tinytools{	// Using a namespace to try to prevent name clashes as my class name is kind of obvious. :)
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace math
{
	inline float GetFractional(float pValue)
	{
		return std::fmod(pValue,1.0f);
	}

	inline float GetInteger(float pValue)
	{
		return pValue - GetFractional(pValue);
	}

	/**
	 * @brief Rounds a floating point value into multiplies of 0.5
	 * -1.2 -> -1.0
	 * -1.0 -> -1.0
	 * -0.8 -> -1.0
	 * 2.3 -> 2.5
	 * 2.8 -> 3.0
	 */
	inline float RoundToPointFive(float pValue)
	{
		const float integer = GetInteger(pValue);
		const float frac = std::round(GetFractional(pValue)*2.0f) / 2.0f;
		return integer + frac;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace string{

inline std::vector<std::string> SplitString(const std::string& pString, const char* pSeperator)
{
	std::vector<std::string> res;
	for (size_t p = 0, q = 0; p != pString.npos; p = q)
	{
		const std::string part(pString.substr(p + (p != 0), (q = pString.find(pSeperator, p + 1)) - p - (p != 0)));
		if( part.size() > 0 )
		{
			res.push_back(part);
		}
	}
	return res;
}

// If pNumChars == 0 then full length is used.
// assert(cppmake::CompareNoCase("onetwo","one",3) == true);
// assert(cppmake::CompareNoCase("onetwo","ONE",3) == true);
// assert(cppmake::CompareNoCase("OneTwo","one",3) == true);
// assert(cppmake::CompareNoCase("onetwo","oneX",3) == true);
// assert(cppmake::CompareNoCase("OnE","oNe") == true);
// assert(cppmake::CompareNoCase("onetwo","one") == true);	// Does it start with 'one'
// assert(cppmake::CompareNoCase("onetwo","onetwothree",6) == true);
// assert(cppmake::CompareNoCase("onetwo","onetwothreeX",6) == true);
// assert(cppmake::CompareNoCase("onetwo","onetwothree") == false); // sorry, but we're searching for more than there is... false...
// assert(cppmake::CompareNoCase("onetwo","onetwo") == true);
/**
 * @brief Does an ascii case insensitive test within the full string or a limited start of the string.
 * 
 * @param pA 
 * @param pB 
 * @param pLength If == 0 then length of second string is used. If first is shorter, will always return false.
 * @return true 
 * @return false 
 */
inline bool CompareNoCase(const char* pA,const char* pB,size_t pLength = 0)
{
    assert( pA != nullptr || pB != nullptr );// Note only goes pop if both are null.
// If either or both NULL, then say no. A bit like a divide by zero as null strings are not strings.
    if( pA == nullptr || pB == nullptr )
        return false;

// If same memory then yes they match, doh!
    if( pA == pB )
        return true;

    if( pLength == 0 )
        pLength = strlen(pB);

    while( (*pA != 0 || *pB != 0) && pLength > 0 )
    {
        // Get here are one of the strings has hit a null then not the same.
        // The while loop condition would not allow us to get here if both are null.
        if( *pA == 0 || *pB == 0 )
        {// Check my assertion above that should not get here if both are null. Note only goes pop if both are null.
            assert( pA != NULL || pB != NULL );
            return false;
        }

        if( tolower(*pA) != tolower(*pB) )
            return false;

        pA++;
        pB++;
        pLength--;
    };

    // Get here, they are the same.
    return true;
}

inline bool CompareNoCase(const std::string& pA,const std::string& pB,size_t pLength = 0)
{
	return CompareNoCase(pA.c_str(),pB.c_str(),pLength);
}


};//namespace string{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace timers{
class MillisecondTicker
{
public:
	MillisecondTicker() = default;
    MillisecondTicker(int pMilliseconds)
	{
		SetTimeout(pMilliseconds);
	}


	/**
	 * @brief Sets the new timeout interval, resets internal counter.
	 * 
	 * @param pMilliseconds 
	 */
	void SetTimeout(int pMilliseconds)
	{
		assert(pMilliseconds > 0 );
		mTimeout = std::chrono::milliseconds(pMilliseconds);
		mTrigger = std::chrono::system_clock::now() + mTimeout;
	}

	/**
	 * @brief Returns true if trigger ticks is less than now
	 */
    bool Tick(){return Tick(std::chrono::system_clock::now());}
    bool Tick(const std::chrono::system_clock::time_point pNow)
	{
		if( mTrigger < pNow )
		{
			mTrigger += mTimeout;
			return true;
		}
		return false;
	}

	/**
	 * @brief Calls the function if trigger ticks is less than now. 
	 */
    void Tick(std::function<void()> pCallback){Tick(std::chrono::system_clock::now(),pCallback);}
    void Tick(const std::chrono::system_clock::time_point pNow,std::function<void()> pCallback )
	{
		assert( pCallback != nullptr );
		if( mTrigger < pNow )
		{
			mTrigger += mTimeout;
			pCallback();
		}
	}


private:
    std::chrono::milliseconds mTimeout;
    std::chrono::system_clock::time_point mTrigger;
};

};//namespace timers{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace network{

/**
 * @brief 
 * Used the excellent answer by SpectreVert from the topic below.
 * https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c
 * A little restructure to minimise typo bugs. (e.g fogetting close on socket)
 * @return std::string 
 */
inline std::string GetLocalIP()
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    sockaddr_in loopback;
 
	std::string ip = "Could not socket";
    if( sock > 0 )
	{
		std::memset(&loopback, 0, sizeof(loopback));
		loopback.sin_family = AF_INET;
		loopback.sin_addr.s_addr = 1;   // can be any IP address. Odd, but works. :/
		loopback.sin_port = htons(9);   // using debug port

		if( connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == 0 )
		{
			socklen_t addrlen = sizeof(loopback);
			if( getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == 0 )
			{
				char buf[INET_ADDRSTRLEN];
				if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0)
				{
					ip = "Could not inet_ntop";
				}
				else
				{
					ip = buf;
				}
			}
			else
			{
				ip = "Could not getsockname";
			}
		}
		else
		{
			ip = "Could not connect";
		}

		// All paths that happen after opening sock will come past here. Less chance of accidentally leaving it open.
		close(sock);
	}

	return ip;
}

/**
 * @brief Get Host Name, handy little wrapper.
 */
inline std::string GetHostName()
{
    char buf[256];
    ::gethostname(buf,256);
	return std::string(buf);
}
};// namespace network

///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace system{

/**
 * @brief Returns seconds since 1970, the epoch. I put this is as I can never rememeber the correct construction using c++ :D
 */
inline int64_t SecondsSinceEpoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief Fetches the system uptime in a more human readable format
 */
inline bool GetUptime(uint64_t& rUpDays,uint64_t& rUpHours,uint64_t& rUpMinutes)
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
    return false;
}

/**
 * @brief Return the uptime as a nice human readable string.
 */
inline std::string GetUptime()
{
	uint64_t upDays,upHours,upMinutes;
	GetUptime(upDays,upHours,upMinutes);
	std::string upTime;
	std::string space;
	if( upDays > 0 )
	{
		if( upDays == 1 )
		{
			upTime = "1 Day";
		}
		else
		{
			upTime = std::to_string(upDays) + " Days";
		}
		space = " ";
	}

	if( upHours > 0 )
	{
		if( upHours == 1 )
		{
			upTime += space + "1 Hour";
		}
		else
		{
			upTime += space + std::to_string(upHours) + " Hours";
		}
		space = " ";
	}

	if( upMinutes == 1 )
	{
		upTime += "1 Minute";
	}
	else
	{
		upTime += space + std::to_string(upMinutes) + " Minutes";
	}

	return upTime;
}

/**
 * @brief USed to track the deltas from last time function is called. Needed to be done like this to beable to see instantaneous CPU load. Method copied from htop source.
 * https://www.linuxhowtos.org/manpages/5/proc.htm
 * https://github.com/htop-dev/htop/
 */
struct CPULoadTracking
{
	uint64_t mUserTime;		// <! Time spent in user mode and hosting virtual machines.
	uint64_t mTotalTime;	// <! Total time, used to create the percentage.
};

/**
 * @brief Will return N + 1 entries for each hardware thread, +1 for total avarage.
 * First call expects pTrackingData to be zero in length, and will initialise the vector. Don't mess with the vector as you're break it all. ;)
 * Index of map is core ID, The ID of the CPU core (including HW threading, so a 8 core 16 thread system will have 17 entries, 16 for threads and one for total)
 * The load is the load of user space and virtual guests on the system. Does not include system time load.
 */
inline bool GetCPULoad(std::map<int,CPULoadTracking>& pTrackingData,int& rTotalSystemLoad,std::map<int,int>& rCoreLoads)
{
	// If pTrackingData is empty then we're initalising the state so lets build our starting point.
	const bool initalising = pTrackingData.size() ==  0;

	std::ifstream statFile("/proc/stat");
	if( statFile.is_open() )
	{
		// Written so that it'll read the lines in any order. Easy to do and safe.
		while( statFile.eof() == false )
		{
			std::string what;
			statFile >> what;
			if( string::CompareNoCase(what,"cpu") )
			{
				// The the rest of the bits we want.
				std::string usertimeSTR, nicetimeSTR, systemtimeSTR, idletimeSTR, ioWaitSTR, irqSTR, softIrqSTR, stealSTR, guestSTR, guestniceSTR;
				statFile >> usertimeSTR >>
								 nicetimeSTR >>
								 systemtimeSTR >>
								 idletimeSTR >>
								 ioWaitSTR >>
								 irqSTR >>
								 softIrqSTR >>
								 stealSTR >>
								 guestSTR >>
								 guestniceSTR;
				// Read rest of line.
				std::string eol;
				std::getline(statFile,eol,'\n');

				const uint64_t userTime = std::stoull(usertimeSTR);	
				const uint64_t niceTime = std::stoull(nicetimeSTR);
				const uint64_t idleTime = std::stoull(idletimeSTR);
				const uint64_t stealTime = std::stoull(stealSTR);
				
				const uint64_t systemAllTime = std::stoull(systemtimeSTR) + std::stoull(irqSTR) + std::stoull(softIrqSTR);
//				const uint64_t virtualTime = std::stoull(guestSTR) + std::stoull(guestniceSTR); // According to a comment in hot, guest and guest nice are already incorerated in user time.
				const uint64_t totalTime = userTime + niceTime + systemAllTime + idleTime + stealTime; // No need to add virtualTime, I want that to be part of user time. If not will not see load on CPU from virtual guest.

				int cpuID = -1; // This is the total system load.
				if( what.size() > 3 )
				{
					sscanf(what.data(),"cpu%d",&cpuID);
					if( cpuID < 0 || cpuID > 128 )
					{
						TINYTOOLS_THROW("Error in GetCPULoad, cpu Id read from /proc/stat has a massive index, not going ot happen mate... cpuID == " + std::to_string(cpuID));
					}
				}

				if( initalising )
				{
					if( pTrackingData.find(cpuID) != pTrackingData.end() )
					{
						TINYTOOLS_THROW("Error in GetCPULoad, trying to initalising and found a duplicate CPU id, something went wrong. cpuID == " + std::to_string(cpuID));
					}

					CPULoadTracking info;
					info.mUserTime		= userTime;
					info.mTotalTime		= totalTime;
					pTrackingData.emplace(cpuID,info);
				}
				else
				{
					// Workout the deltas to get cpu load in percentage.
					auto core = pTrackingData.find(cpuID);
					if( core == pTrackingData.end() )
					{
						TINYTOOLS_THROW("Error in GetCPULoad, trying to workout load but found a new CPU id, something went wrong. cpuID == " + std::to_string(cpuID));
					}

					// Copied from htop!
					// Since we do a subtraction (usertime - guest) and cputime64_to_clock_t()
					// used in /proc/stat rounds down numbers, it can lead to a case where the
					// integer overflow.
					#define WRAP_SUBTRACT(a,b) (a > b) ? a - b : 0					
					const uint64_t deltaUser = WRAP_SUBTRACT(userTime,core->second.mUserTime);
					const uint64_t deltaTotal = WRAP_SUBTRACT(totalTime,core->second.mTotalTime);
					#undef WRAP_SUBTRACT
					core->second.mUserTime = userTime;
					core->second.mTotalTime = totalTime;

					if( deltaTotal > 0 )
					{
						const uint64_t percentage = deltaUser * 100 / deltaTotal;
						if( cpuID == -1 )
						{
							rTotalSystemLoad = (int)percentage;
						}
						else
						{
							rCoreLoads[cpuID] = (int)percentage;
						}
					}
					else
					{
						if( cpuID == -1 )
						{
							rTotalSystemLoad = 0;
						}
						else
						{
							rCoreLoads[cpuID] = 0;
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

/**
 * @brief Get the Memory Usage, all values passed back in 1K units because that is what the OS sends back.
 * Used https://gitlab.com/procps-ng/procps as reference as it's not as simple as reading the file. :-? Thanks Linus.....
 */
inline bool GetMemoryUsage(size_t& rMemoryUsedKB,size_t& rMemAvailableKB,size_t& rMemTotalKB,size_t& rSwapUsedKB)
{
	std::ifstream memFile("/proc/meminfo");
	if( memFile.is_open() )
	{
		// We'll build a map of the data lines that have key: value combination.
		std::map<std::string,size_t> keyValues;
		while( memFile.eof() == false )
		{
			std::string memLine;
			std::getline(memFile,memLine);
			if( memLine.size() > 0 )
			{
				std::vector<std::string> memParts = string::SplitString(memLine," ");// Have to do like this as maybe two or three parts. key: value [kb]
				if( memParts.size() != 2 && memParts.size() != 3 )
				{
					TINYTOOLS_THROW("Failed to correctly read a line from /proc/meminfo did the format change? Found line \"" + memLine + "\" and split it into " + std::to_string(memParts.size()) + " parts");
				}

				if( memParts[0].back() == ':' )
				{
					memParts[0].pop_back();
					keyValues[memParts[0]] = std::stoull(memParts[1]);
				}
			}
		}

		try
		{
			const size_t Buffers = keyValues.at("Buffers");
			const size_t Cached = keyValues["Cached"];
			const size_t SReclaimable = keyValues["SReclaimable"];
			const size_t MemAvailable = keyValues["MemAvailable"];
			const size_t MemTotal = keyValues["MemTotal"];
			const size_t MemFree = keyValues["MemFree"];
			const size_t SwapFree = keyValues["SwapFree"];
			const size_t SwapTotal = keyValues["SwapTotal"];

			const size_t mainCached = Cached + SReclaimable;
			// if kb_main_available is greater than kb_main_total or our calculation of
			// mem_used overflows, that's symptomatic of running within a lxc container
			// where such values will be dramatically distorted over those of the host.
			if (MemAvailable > MemTotal)
			{
				rMemAvailableKB = MemFree;
			}
			else
			{
				rMemAvailableKB = MemAvailable;
			}

			rMemoryUsedKB = MemTotal - MemFree - mainCached - Buffers;
			if (rMemoryUsedKB < 0)
			{
				rMemoryUsedKB = MemTotal - MemFree;
			}

			rMemTotalKB = MemTotal;
			rSwapUsedKB = SwapTotal - SwapFree;

			return true;
		}
		catch( std::out_of_range& e ){std::cerr << " failed to read all the data needed from /proc/meminfo " << e.what();}// We'll return false so they know something went wrong.
	}
	return false;
}

};//namespace system{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace threading{

/**
 * @brief This class encapsulates a thread that can sleep for any amount of time but wake up when the ownering, main thread, needs to exit the application.
 */
class SleepableThread
{
public:

	/**
	 * @brief This will start the thread and return, pTheWork will be called 'ticked' by the thread after each pause, and exit when asked to.
	 * pPauseInterval is seconds.
	 */
	void Tick(int pPauseInterval,std::function<void()> pTheWork)
	{
		if( pTheWork == nullptr )
		{
			TINYTOOLS_THROW("SleepableThread passed nullpoint for the work to do...");
		}

		mWorkerThread = std::thread([this,pPauseInterval,pTheWork]()
		{
			while(mKeepGoing)
			{
				pTheWork();
				std::unique_lock<std::mutex> lk(mSleeperMutex);
				mSleeper.wait_for(lk,std::chrono::seconds(pPauseInterval));
			};
		});
	}

	/**
	 * @brief Called from another thread, will ask it to exit and then wait for it to do so.
	 * 
	 */
	void TellThreadToExitAndWait()
	{
		mKeepGoing = false;
	    mSleeper.notify_one();
		if( mWorkerThread.joinable() )
		{
			mWorkerThread.join();
		}
	}

private:
    bool mKeepGoing;                  	//!< A boolean that will be used to signal the worker thread that it should exit.
	std::thread mWorkerThread;			//!< The thread that will do your work and then sleep for a bit. 
    std::condition_variable mSleeper;   //!< Used to sleep for how long asked for but also wake up if we need to exit.
    std::mutex mSleeperMutex;			//!< This is used to correctly use the condition variable.
};

};//namespace threading{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CommandLineOptions
{
public:
	CommandLineOptions(const std::string& pUsageHelp):mUsageHelp(pUsageHelp)
	{// Always add this, everyone does. BE rude not to. ;)
		AddArgument('h',"help","Display this help and exit");
	}

	void AddArgument(char pArg,const std::string& pLongArg,const std::string& pHelp,int pArgumentOption = no_argument,std::function<void(const std::string& pOptionalArgument)> pCallback = nullptr)
	{
		if( mArguments.find(pArg) != mArguments.end() )
		{
			TINYTOOLS_THROW(std::string("class CommandLineOptions: Argument ") + pArg + " has already been registered, can not contine");
		}
		
		mArguments.emplace(pArg,Argument(pLongArg,pHelp,pArgumentOption,pCallback));
	}

	bool Process(int argc, char *argv[])
	{
		std::vector<struct option> longOptions;
		std::string shortOptions;

		// Build the data for the getopt_long function that will do all the work for us.
		for( auto& opt : mArguments)
		{
			shortOptions += opt.first;
			if( opt.second.mArgumentOption == required_argument )
			{
				shortOptions += ":";
			}
			// Bit of messing about because mixing c code with c++
			struct option newOpt = {opt.second.mLongArgument.c_str(),opt.first,nullptr,opt.first};
			longOptions.emplace_back(newOpt);
		}
		struct option emptyOpt = {NULL, 0, NULL, 0};
		longOptions.emplace_back(emptyOpt);

		int c,oi;
		while( (c = getopt_long(argc,argv,shortOptions.c_str(),longOptions.data(),&oi)) != -1 )
		{
			auto arg = mArguments.find(c);
			if( arg == mArguments.end() )
			{// Unknow option, print help and bail.
				std::cout << "Unknown option \'" << c << "\' found.\n";
				PrintHelp();
				return false;
			}
			else
			{
				arg->second.mIsSet = true;
				std::string optionalArgument;
				if( optarg )
				{// optarg is defined in getopt_code.h 
					optionalArgument = optarg;
				}

				if( arg->second.mCallback != nullptr )
				{
					arg->second.mCallback(optionalArgument);
				}
			}
		};

		// See if help was asked for.
		if( IsSet('h') )
		{
			PrintHelp();
			return false;
		}

		return true;
	}

	bool IsSet(char pShortOption)const
	{
		return mArguments.at(pShortOption).mIsSet;
	}

	bool IsSet(const std::string& pLongOption)const
	{
		for( auto& opt : mArguments)
		{
			if( string::CompareNoCase(opt.second.mLongArgument,pLongOption) )
			{
				return opt.second.mIsSet;
			}
		}

		return false;
	}

	void PrintHelp()const
	{
		std::cout << mUsageHelp << "\n";

		std::vector<char> shortArgs;
		std::vector<std::string> longArgs;
		std::vector<std::string> descriptions;

		for( auto& opt : mArguments)
		{
			shortArgs.push_back(opt.first);
			descriptions.push_back(opt.second.mHelp);
			if( opt.second.mArgumentOption == required_argument )
			{
				longArgs.push_back("--" + opt.second.mLongArgument + "=arg");
			}
			else if( opt.second.mArgumentOption == optional_argument )
			{
				longArgs.push_back("--" + opt.second.mLongArgument + "[=arg]");
			}
			else
			{
				longArgs.push_back("--" + opt.second.mLongArgument);
			}
		}

		// Now do a load of formatting of the output.
		size_t DescMaxSpace = 0;
		for(auto lg : longArgs)
		{
			size_t l = 5 + lg.size(); // 5 == 2 spaces + -X + 1 for space for short arg.
			if( DescMaxSpace < l )
				DescMaxSpace = l;
		}

		DescMaxSpace += 4; // Add 4 spaces for formatting.
		for(size_t n=0;n<shortArgs.size();n++)
		{
			std::string line = "  -";
			line += shortArgs[n];
			line += " ";
			line += longArgs[n];
			line += " ";
			std::cout << line;

			size_t space = DescMaxSpace - line.size();
			const std::vector<std::string> lines = string::SplitString(descriptions[n],"\n");
			for(auto line : lines)
			{
				std::cout << std::string(space,' ') << line << '\n';
				space = DescMaxSpace + 2;// For subsequent lines.
			}
		}
	}

private:
	struct Argument
	{
		Argument(const std::string& pLongArgument,const std::string& pHelp,const int pArgumentOption,std::function<void(const std::string& pOptionalArgument)> pCallback):
			mLongArgument(pLongArgument),
			mHelp(pHelp),
			mArgumentOption(pArgumentOption),
			mCallback(pCallback),
			mIsSet(false)
		{

		}

		const std::string mLongArgument;
		const std::string mHelp;
		const int mArgumentOption;
		std::function<void(const std::string& pOptionalArgument)> mCallback;
		bool mIsSet;	//!< This will be true if the option was part of the commandline. Handy for when you just want to know true or false.
	};
	
	const std::string mUsageHelp;
	std::map<char,Argument> mArguments;
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace tinytools
	
#endif //TINY_2D_H
