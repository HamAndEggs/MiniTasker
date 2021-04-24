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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
   Original code base is at https://github.com/HamAndEggs/TinyWeather   
   
   */

#ifndef TINY_WEATHER_H
#define TINY_WEATHER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <ctime>

namespace tinyweather{
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector<std::pair<int,std::string>> HourlyIconVector;

struct WeatherTime
{
	std::time_t mUTC;

	WeatherTime() = default;
	WeatherTime(std::time_t pUTC)
	{
		Set(pUTC);
	}
	
	void Set(std::time_t pTime)
	{
		mUTC = pTime;

		tm currentTime = *localtime((time_t*)&pTime);

		mYear = currentTime.tm_year + 1900;
		mMonth = currentTime.tm_mon + 1; // (1 - 12) tm.tm_mon is zero based index 0 - 11 but day in month is not, no consistency!
		mDay = currentTime.tm_mday;   // (1 - 31) or 30 or 29 or 28..... tm.tm_mday is 1 based index.
		mHour = currentTime.tm_hour;
		mMinute = currentTime.tm_min;
	}

	int mYear;
	int mMonth; // (1 - 12)
	int mDay;   // (1 - 31) or 30 or 29 or 28.....
	int mHour;
	int mMinute;

	std::string GetDate()const{return std::to_string(mDay) + "/" + std::to_string(mMonth) + "/" + std::to_string(mYear);}
	std::string GetTime()const{return std::to_string(mHour) + ":" + std::to_string(mMinute);}

	inline bool operator < (const WeatherTime& pOther)const{return mUTC < pOther.mUTC;}
	inline bool operator == (const WeatherTime& pOther)const{return mUTC == pOther.mUTC;}
};

struct Temperature
{
	float k;	// Kelvin
	float c;	// Celsius
	float f;	// Fahrenheit

	void Set(float pKelvin)
	{
		k = pKelvin;
		c = k - 273.15f;
		f = k * 9.0f/5.0f - 459.670f;
	}
};

struct DisplayData
{
	uint32_t mID;				//!< Weather condition id
	std::string mTitle;			//!< Group of weather parameters (Rain, Snow, Extreme etc.)
	std::string mDescription;	//!< Weather condition within the group (full list of weather conditions). Get the output in your language
	std::string mIcon;			//!< Weather icon id. How to get icons
};


/**
 * @brief This data contains the current state of the weather for the time.
 * It is used for the current weather and the hourley forcast weather for the same day as current.
 */
struct WeatherData
{
	WeatherTime mTime;				//!< Current time, Unix, UTC
	WeatherTime mSunrise;			//!< Sunrise time, Unix, UTC
	WeatherTime mSunset;			//!< Sunset time, Unix, UTC
	Temperature mTemperature;	//!< Temperature. Units - default: kelvin, metric: Celsius, imperial: Fahrenheit. How to change units used
	Temperature mFeelsLike;		//!< This temperature parameter accounts for the human perception of weather. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
	uint32_t mPressure;			//!< Atmospheric pressure on the sea level, hPa
	uint32_t mHumidity;			//!< Humidity, %
	float mDewPoint;			//!< Atmospheric temperature (varying according to pressure and humidity) below which water droplets begin to condense and dew can form. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
	uint32_t mClouds;			//!< Cloudiness, %
	uint32_t mUVIndex;			//!< Current UV index
	uint32_t mVisibility;		//!< Average visibility, metres
	float mWindSpeed;			//!< Wind speed. Wind speed. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	float mWindGusts;			//!< defaults to 0 if not found. (where available) Wind gust. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	uint32_t mWindDirection;	//!< Wind direction, degrees (meteorological)
	DisplayData mDisplay;
};

struct DailyWeatherData
{
	
	WeatherTime mTime;				//!< Current time, Unix, UTC
	WeatherTime mSunrise;			//!< Sunrise time, Unix, UTC
	WeatherTime mSunset;			//!< Sunset time, Unix, UTC

	struct
	{
		void Set(float pMorning,float pDay,float pEvening,float pNight,float pMin,float pMax)
		{
			Morning.Set(pMorning);
			Day.Set(pDay);
			Evening.Set(pEvening);
			Night.Set(pNight);
			Min.Set(pNight);
			Max.Set(pNight);
		}
		Temperature Morning,Day,Evening,Night,Min,Max;
	}mTemperature;	//!< Temperature. Units - default: kelvin, metric: Celsius, imperial: Fahrenheit. How to change units used

	struct
	{
		void Set(float pMorning,float pDay,float pEvening,float pNight)
		{
			Morning.Set(pMorning);
			Day.Set(pDay);
			Evening.Set(pEvening);
			Night.Set(pNight);
		}
		Temperature Morning,Day,Evening,Night;
	}mFeelsLike;		//!< This temperature parameter accounts for the human perception of weather. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.

	uint32_t mPressure;			//!< Atmospheric pressure on the sea level, hPa
	uint32_t mHumidity;			//!< Humidity, %
	float mDewPoint;			//!< Atmospheric temperature (varying according to pressure and humidity) below which water droplets begin to condense and dew can form. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.

	uint32_t mClouds;			//!< Cloudiness, %
	uint32_t mUVIndex;			//!< Current UV index

	float mWindSpeed;			//!< Wind speed. Wind speed. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	float mWindGusts;			//!< defaults to 0 if not found. (where available) Wind gust. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour. How to change units used
	uint32_t mWindDirection;	//!< Wind direction, degrees (meteorological)

	float mPrecipitationProbability;	//!< Probability of precipitation
	float mRain;						//!< (where available) Precipitation volume, mm
	float mSnow;						//!< (where available) Snow volume, mm
	DisplayData mDisplay;
};

/**
 * @brief The weather data for a miniute interval. 
 * Included as in tests was sent, but only had the time and expected rain volume. A bit odd?
 */
struct MinutelyForecast
{
	WeatherTime mTime; //!< Time of the forecasted data, unix, UTC
	uint32_t mPrecipitation; //!< Precipitation volume, mm
};

/**
 * @brief Contains all the weather information downloaded.
 * When you call get it will build a tree of data that you can read that represents the weather for your area.
 * uses OpenWeather one-call-api https://openweathermap.org/api/one-call-api
 * You will need to make a free account and get an API key
 */
struct OpenWeatherMap
{
	double mLatitude;			//!< Geographical coordinates of the location (latitude)
	double mLongitude;			//!< Geographical coordinates of the location (longitude)
	std::string mTimeZone;		//!< timezone Timezone name for the requested location
	uint32_t mTimezoneOffset;	//!< timezone_offset Shift in seconds from UTC
	WeatherData mCurrent; 		//<! Current weather data API response

	std::vector<MinutelyForecast>mMinutely; //!< Minute forecast weather data API response
	std::vector<WeatherData>mHourly;		//!< Hourly forecast weather data API response
	std::vector<DailyWeatherData>mDaily;	//!< Daily forecast weather data API response



	//!< alerts National weather alerts data from major national weather warning systems
	//!< alerts.sender_name Name of the alert source. Please read here the full list of alert sources
	//!< alerts.event Alert event name
	//!< alerts.start Date and time of the start of the alert, Unix, UTC
	//!< alerts.end Date and time of the end of the alert, Unix, UTC
	//!< alerts.description Description of the alert

	OpenWeatherMap(const std::string& pAPIKey);
	~OpenWeatherMap();

	void Get(double pLatitude,double pLongitude,std::function<void(bool pDownloadedOk,const OpenWeatherMap& pWeather)> pReturnFunction);

	/**
	 * @brief Get the current temperature forcast from the hourly forcast data.
	 * @param pNowUTC When the forcast is for.
	 */
	const WeatherData* GetHourlyForcast(std::time_t pNowUTC)const;

	/**
	 * @brief Get the hourly temperature forcast.
	 * @param pNowUTC When the forcast is for.
	 */
	float GetHourlyTemperature(std::time_t pNowUTC)const;

	/**
	 * @brief For the day passed in UTC time you'll get a map of weather icon names for each hour.
	 * I send back the hour as that helps when you do the display, you can mark it as 6am for example.
	 * @param pNowUTC 
	 * @return const HourlyIconVector The map is <24h,name>. EG the icon for 7pm is 'icon name == map[19]'
	 */
	HourlyIconVector GetTodaysHourlyIconCodes(std::time_t pNowUTC)const;

	/**
	 * @brief For the UTC time you'll get a map of weather icon names for each hour into the future spanning many days.
	 * First is the current hour.
	 * I send back the hour as that helps when you do the display, you can mark it as 6am for example.
	 * @param pNowUTC 
	 * @return const HourlyIconVector In hour order. The map is <24h,name>. EG the icon for 7pm is 'icon name == map[19]'
	 */
	HourlyIconVector GetHourlyIconCodes(std::time_t pNowUTC)const;

private:

	const std::string mAPIKey;

	bool DownloadWeatherReport(const std::string& pURL,std::string& rJson)const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
}; //namespace tinyweather{

#endif //TINY_WEATHER_H
