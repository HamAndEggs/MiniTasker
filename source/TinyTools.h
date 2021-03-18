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

#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <cstring>
#include <iostream>

#include <assert.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


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
		mTimeout = (CLOCKS_PER_SEC * pMilliseconds) / 1000;
		mTrigger = clock() + mTimeout;
	}

	/**
	 * @brief Returns true if trigger ticks is less than now
	 */
    bool Tick(){return Tick(clock());}
    bool Tick(const clock_t pNow)
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
    void Tick(std::function<void()> pCallback){Tick(clock(),pCallback);}
    void Tick(const clock_t pNow,std::function<void()> pCallback )
	{
		assert( pCallback != nullptr );
		if( mTrigger < pNow )
		{
			mTrigger += mTimeout;
			pCallback();
		}
	}


private:
    clock_t mTimeout;
    clock_t mTrigger;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace tiny2d
	
#endif //TINY_2D_H
