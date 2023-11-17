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
   
   Original code base is at https://github.com/HamAndEggs/solax
*/

#include <iostream>
#include <sstream>
#include <assert.h>

#include <stdio.h>
#include <curl/curl.h>

#include "TinySolaX.h"
#include "TinyJson.h"

namespace solax{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::time_t ONE_MINUTE = (60);
const std::time_t ONE_HOUR = (ONE_MINUTE * 60);
const std::time_t ONE_DAY = (ONE_HOUR*24);


static int CURLWriter(char *data, size_t size, size_t nmemb,std::string *writerData)
{
	if(writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}

TinySolaX::TinySolaX(const std::string& pAPIKey):mAPIKey(pAPIKey)
{
//	std::clog << "sizeof time_t = " << sizeof(time_t) << " sizeof uint64_t = " << sizeof(uint64_t) << '\n';
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

TinySolaX::~TinySolaX()
{
	curl_global_cleanup();
}

void TinySolaX::Get(const std::string& pSerialNumber,std::function<void(const RealtimeData &pData)> pReturnFunction)
{ 
	assert( pReturnFunction != nullptr );

	RealtimeData data;

	std::string jsonData;
	std::stringstream url;
	url << "https://www.solaxcloud.com/proxyApp/proxy/api/getRealtimeInfo.do";
	url << "?tokenId=" << mAPIKey;
	url << "&sn=" << pSerialNumber ;

	if( Download(url.str(),jsonData) )
	{
		try
		{
			tinyjson::JsonProcessor json(jsonData);
			const tinyjson::JsonValue solar = json.GetRoot();

			if( solar.GetBoolean("success") )
			{
				const tinyjson::JsonValue result = solar["result"];
				data.SerialNumber.inverter = result["inverterSN"].GetString();
				data.SerialNumber.comms = result["sn"].GetString();
				data.Inverter.totalPower = result["acpower"].GetDouble();
				data.Inverter.energyOutDaily = result["yieldtoday"].GetDouble();
				data.Inverter.energyTotal = result["yieldtotal"].GetDouble();

				data.Grid.powerTotal = result["feedinpower"].GetDouble();
				data.Grid.toGridTotal = result["feedinenergy"].GetDouble();
				data.Grid.toFromTotal = result["consumeenergy"].GetDouble();

				data.Inverter.meter2PowerTotal = result["feedinpowerM2"].GetDouble();

				data.BatterySOC = result["soc"].GetDouble();

				data.Inverter.EPSPowerR = result.GetDouble("peps1");
				data.Inverter.EPSPowerS = result.GetDouble("peps2");
				data.Inverter.EPSPowerT = result.GetDouble("peps3");

				data.Inverter.type = (InverterType)result["inverterType"].GetType();
				data.Inverter.status = (InverterStatus)result["inverterStatus"].GetType();

				data.Uploadtime = result["uploadTime"].GetString();
				data.success = true;
			}
			else
			{
				std::cerr << "Failed to download solax: " << jsonData << "\n";
			}
		}
		catch(std::runtime_error &e)
		{
			std::cerr << "Failed to download solax: " << e.what() << "\n";
		}
	}

	// Always return something. So they know if it failed or not.
	pReturnFunction(data);
}

bool TinySolaX::Download(const std::string& pURL,std::string& rJson)const
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
}; //namespace solax{

