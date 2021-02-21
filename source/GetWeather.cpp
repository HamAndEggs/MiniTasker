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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
   Original code base is at https://github.com/HamAndEggs/GetWeather
*/

#include <iostream>
#include <sstream>
#include <assert.h>

#include <stdio.h>
#include <curl/curl.h>

#include "GetWeather.h"
#include "TinyJson.h"

namespace getweather{
///////////////////////////////////////////////////////////////////////////////////////////////////////////


static int CURLWriter(char *data, size_t size, size_t nmemb,std::string *writerData)
{
	if(writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}

static void ReadWeatherData(const tinyjson::JsonValue pJson,WeatherData& rWeather)
{

	rWeather.mTime = pJson.GetUInt64("dt");					//!< Current time, Unix, UTC
	rWeather.mSunrise = pJson.GetUInt64("sunrise");			//!< Sunrise time, Unix, UTC
	rWeather.mSunset = pJson.GetUInt64("sunset");				//!< Sunset time, Unix, UTC
	rWeather.mTemperature.Set(pJson.GetFloat("temp"));			//!< Temperature. Units - default: kelvin, metric: Celsius, imperial: Fahrenheit. How to change units used
	rWeather.mFeelsLike.Set(pJson.GetFloat("feels_like"));		//!< This temperature parameter accounts for the human perception of weather. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
	rWeather.mPressure = pJson.GetUInt64("pressure");			//!< Atmospheric pressure on the sea level, hPa
	rWeather.mHumidity = pJson.GetUInt64("humidity");			//!< Humidity, %
	rWeather.mDewPoint = pJson.GetFloat("dew_point");			//!< Atmospheric temperature (varying according to pressure and humidity) below which water droplets begin to condense and dew can form. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
	rWeather.mClouds = pJson.GetUInt32("clouds");				//!< Cloudiness, %
	rWeather.mUVIndex = pJson.GetUInt32("uvi");				//!< Current UV index
	rWeather.mVisibility = pJson.GetUInt32("visibility");		//!< Average visibility, metres
	rWeather.mWindSpeed = pJson.GetFloat("wind_speed");		//!< Wind speed. Wind speed. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	rWeather.mWindGusts = pJson.GetFloat("wind_deg");			//!< defaults to 0 if not found. (where available) Wind gust. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	rWeather.mWindDirection = pJson.GetUInt32("wind_deg");	//!< Wind direction, degrees (meteorological)

	if( pJson.GetArraySize("weather") > 0 )
	{
		const tinyjson::JsonValue& weather = pJson["weather"][0];
		rWeather.mDisplay.mID = weather.GetUInt32("id");
		rWeather.mDisplay.mTitle = weather.GetString("main");
		rWeather.mDisplay.mDescription = weather.GetString("description");
		rWeather.mDisplay.mIcon = weather.GetString("icon");
	}
}

static void ReadDailyWeatherData(const tinyjson::JsonValue pJson,DailyWeatherData& rDaily)
{

	rDaily.mTime = pJson.GetUInt64("dt");					//!< Current time, Unix, UTC
	rDaily.mSunrise = pJson.GetUInt64("sunrise");			//!< Sunrise time, Unix, UTC
	rDaily.mSunset = pJson.GetUInt64("sunset");				//!< Sunset time, Unix, UTC
	rDaily.mPressure = pJson.GetUInt64("pressure");			//!< Atmospheric pressure on the sea level, hPa
	rDaily.mHumidity = pJson.GetUInt64("humidity");			//!< Humidity, %
	rDaily.mDewPoint = pJson.GetFloat("dew_point");			//!< Atmospheric temperature (varying according to pressure and humidity) below which water droplets begin to condense and dew can form. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
	rDaily.mClouds = pJson.GetUInt32("clouds");				//!< Cloudiness, %
	rDaily.mUVIndex = pJson.GetUInt32("uvi");				//!< Current UV index

	rDaily.mWindSpeed = pJson.GetFloat("wind_speed");		//!< Wind speed. Wind speed. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	rDaily.mWindGusts = pJson.GetFloat("wind_deg");			//!< defaults to 0 if not found. (where available) Wind gust. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	rDaily.mWindDirection = pJson.GetUInt32("wind_deg");	//!< Wind direction, degrees (meteorological)

	rDaily.mPrecipitationProbability = pJson.GetFloat("prop");	//!< Probability of precipitation
	rDaily.mRain = pJson.GetFloat("rain");						//!< (where available) Precipitation volume, mm
	rDaily.mSnow = pJson.GetFloat("snow");						//!< (where available) Snow volume, mm

	if( pJson.GetType("temp") == tinyjson::JTYPE_OBJECT  )
	{
		const tinyjson::JsonValue& temp = pJson["temp"];
		rDaily.mTemperature.Set
		(
			temp.GetFloat("morn"),
			temp.GetFloat("day"),
			temp.GetFloat("eve"),
			temp.GetFloat("night"),
			temp.GetFloat("min"),
			temp.GetFloat("max")
		);
	}
	else if( pJson.GetType("temp") == tinyjson::JTYPE_NUMBER  )
	{
		rDaily.mTemperature.Set(pJson.GetFloat("temp"));
	}

	if( pJson.GetType("feels_like") == tinyjson::JTYPE_OBJECT  )
	{
		const tinyjson::JsonValue& feels_like = pJson["feels_like"];
		rDaily.mFeelsLike.Set
		(
			feels_like.GetFloat("morn"),
			feels_like.GetFloat("day"),
			feels_like.GetFloat("eve"),
			feels_like.GetFloat("night"),
			feels_like.GetFloat("min"),
			feels_like.GetFloat("max")
		);
	}
	else if( pJson.GetType("feels_like") == tinyjson::JTYPE_NUMBER  )
	{
		rDaily.mFeelsLike.Set(pJson.GetFloat("feels_like"));
	}

	if( pJson.GetArraySize("weather") > 0 )
	{
		const tinyjson::JsonValue& weather = pJson["weather"][0];
		rDaily.mDisplay.mID = weather.GetUInt32("id");
		rDaily.mDisplay.mTitle = weather.GetString("main");
		rDaily.mDisplay.mDescription = weather.GetString("description");
		rDaily.mDisplay.mIcon = weather.GetString("icon");
	}
}


GetWeather::GetWeather(const std::string& pAPIKey):mAPIKey(pAPIKey)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

GetWeather::~GetWeather()
{
	curl_global_cleanup();
}

void GetWeather::Get(double pLatitude,double pLongitude,std::function<void(bool pDownloadedOk,const TheWeather& pWeather)> pReturnFunction)
{ 
	assert( pReturnFunction != nullptr );

	TheWeather weatherData;
	bool downloadedOk = false;

	std::string jsonData;
	std::stringstream url;
	url << "http://api.openweathermap.org/data/2.5/onecall?";
	url << "lat=" << pLatitude << "&";
	url << "lon=" << pLongitude << "&";
	url << "appid=" << mAPIKey;

	if( DownloadWeatherReport(url.str(),jsonData) )
	{
		// We got it, now we need to build the weather object from the json.
		// I would have used rapid json but that is a lot of files to add to this project.
		// My intention is for someone to beable to drop these two files into their project and continue.
		// And so I will make my own json reader, it's easy but not the best solution.
		tinyjson::JsonProcessor json(jsonData);
		const tinyjson::JsonValue weather = json.GetRoot();

		// Lets build up the weather data.
		if( weather.HasValue("current") )
		{
			downloadedOk = true;
			ReadWeatherData(weather["current"],weatherData.mCurrent);
		}

		if( weather.GetArraySize("hourly") > 0 )
		{
			downloadedOk = true;
			const tinyjson::JsonValue& hourly = weather["hourly"];
			for( const auto& weather : hourly.mArray )
			{
				// Looks odd, but is the easiest / optimal way to reduce memory reallocations using c++14 features.
                weatherData.mHourly.resize(weatherData.mHourly.size()+1);
				ReadWeatherData(weather,weatherData.mHourly.back());
			}
		}

		if( weather.GetArraySize("daily") > 0 )
		{
			downloadedOk = true;
			const tinyjson::JsonValue& daily = weather["daily"];
			for( const auto& weather : daily.mArray )
			{
                weatherData.mDaily.resize(weatherData.mDaily.size()+1);
				ReadDailyWeatherData(weather,weatherData.mDaily.back());
			}

		}
	}

	// Always return something. So they know if it failed or not.
	pReturnFunction(downloadedOk,weatherData);
}

bool GetWeather::DownloadWeatherReport(const std::string& pURL,std::string& rJson)const
{
	bool result = false;
	CURL *curl = curl_easy_init();
	if(curl)
	{
		char errorBuffer[CURL_ERROR_SIZE];
		errorBuffer[0] = 0;

		const char* funcName = "CURLOPT_ERRORBUFFER";
		if( curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer) == CURLE_OK )
		{
			funcName = "CURLOPT_URL";
			if( curl_easy_setopt(curl, CURLOPT_URL, pURL.c_str()) == CURLE_OK )
			{
				funcName = "CURLOPT_WRITEFUNCTION";
				if( curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriter) == CURLE_OK )
				{
					funcName = "CURLOPT_WRITEDATA";
					if( curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rJson) == CURLE_OK )
					{
						funcName = "curl_easy_perform";
						if( curl_easy_perform(curl) == CURLE_OK )
						{
							result = true;
						}
					}
				}
			}
		}

		if( result  == false )
		{
			std::cerr << "Lib curl " << funcName << " failed, [" << errorBuffer << "]\n";
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
};// namespace getweather{

