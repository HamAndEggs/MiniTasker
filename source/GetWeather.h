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

#ifndef GET_WEATHER_H
#define GET_WEATHER_H

#include <string>
#include <vector>
#include <functional>

namespace getweather{
///////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Temperature
{
	void Set(float pKelvin)
	{
		Morning.Set(pKelvin);
		Day.Set(pKelvin);
		Evening.Set(pKelvin);
		Night.Set(pKelvin);

		UpdateMinMax();
	}

	void Set(float pMorning,float pDay,float pEvening,float pNight,float pMin,float pMax)
	{
		Morning.Set(pMorning);
		Day.Set(pDay);
		Evening.Set(pEvening);
		Night.Set(pNight);

		if( pMin > 0.0f && pMax > 0.0f )
		{
			Min.Set(pMin);
			Max.Set(pMax);
		}
		else
		{
			UpdateMinMax();
		}
	}

	void UpdateMinMax()
	{
		float minK = Morning.k;
		float maxK = Morning.k;

		minK = std::min(minK,Day.k);
		minK = std::min(minK,Evening.k);
		minK = std::min(minK,Night.k);

		maxK = std::max(minK,Day.k);
		maxK = std::max(minK,Evening.k);
		maxK = std::max(minK,Night.k);

		Min.Set(minK);
		Max.Set(maxK);
	}

	struct
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

	}Morning,Day,Evening,Night,Min,Max;
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
	uint64_t mTime;				//!< Current time, Unix, UTC
	uint64_t mSunrise;			//!< Sunrise time, Unix, UTC
	uint64_t mSunset;			//!< Sunset time, Unix, UTC
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
	
	uint64_t mTime;				//!< Current time, Unix, UTC
	uint64_t mSunrise;			//!< Sunrise time, Unix, UTC
	uint64_t mSunset;			//!< Sunset time, Unix, UTC
	Temperature mTemperature;	//!< Temperature. Units - default: kelvin, metric: Celsius, imperial: Fahrenheit. How to change units used
	Temperature mFeelsLike;		//!< This temperature parameter accounts for the human perception of weather. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
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
	uint64_t mTime; //!< Time of the forecasted data, unix, UTC
	uint32_t mPrecipitation; //!< Precipitation volume, mm
};

/**
 * @brief Contains all the weather information downloaded.
 * 
 */
struct TheWeather
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
};

/**
 * @brief Builds a tree of data that you can read that represents the weather for your area.
 * uses OpenWeather one-call-api https://openweathermap.org/api/one-call-api
 * You will need to make a free account and get an API key
 */
class GetWeather
{
public:
	GetWeather(const std::string& pAPIKey);
	~GetWeather();

	void Get(double pLatitude,double pLongitude,std::function<void(const TheWeather& pWeather)> pReturnFunction);

private:

	const std::string mAPIKey;

	bool DownloadWeatherReport(const std::string& pURL,std::string& rJson)const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
} //namespace getweather{

#endif //GET_WEATHER_H
