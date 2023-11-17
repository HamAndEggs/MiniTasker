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
   
   Original code base is at https://github.com/HamAndEggs/solax   
   
   */

#ifndef TINY_SOLAX_H
#define TINY_SOLAX_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <ctime>

namespace solax{
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector<std::pair<int,std::string>> HourlyIconVector;

enum InverterStatus
{
	WaitMode = 100,
	CheckMode = 101,
	NormalMode = 102,
	FaultMode = 103,
	PermanentFaultMode = 104,
	UpdateMode = 105,
	EPSCheckMode = 106,
	EPSMode = 107,
	Self_TestMode = 108,
	IdleMode = 109,
	StandbyMode = 110,
	PvWakeUpBatMode = 111,
	GenCheckMode = 112,
	GenRunMode = 113
};

enum InverterType
{
	X1_LX = 1,
	X_Hybrid = 2,
	X1_HybiydFit = 3,
	X1_BoostAirMini = 4,
	X3_HybiydFit = 5,
	X3_20K_30K = 6,
	X3_MIC_PRO = 7,
	X1_Smart = 8,
	X1_AC = 9,
	A1_Hybrid = 10,
	A1_Fit = 11,
	A1_Grid = 12,
	J1_ESS = 13,
	X3_Hybrid_G4 = 14,
	X1_Hybrid_G4 = 15,
	X3_MIC_PRO_G2 = 16,
	X1_SPT = 17,
	X1_Boost_Mini_G4 = 18,
	A1_HYB_G2 = 19,
	A1_AC_G2 = 20,
	A1_SMT_G2 = 21,
	X3_FTH = 22,
	X3_MGA_G2 = 23,
};

struct RealtimeData
{
	bool success = false;
	struct
	{
		std::string inverter;
		std::string comms;		
	}SerialNumber;

	struct
	{
		double totalPower;			// W
		double energyOutDaily;		// KWh
		double energyTotal;			// KWh
		double meter2PowerTotal;	// W

		double EPSPowerR;	// W
		double EPSPowerS;	// W
		double EPSPowerT;	// W

		InverterStatus status;
		InverterType type;
	}Inverter;

	struct
	{
		double powerTotal;			// W
		double toGridTotal;		// KWh
		double toFromTotal;		// KWh
	}Grid;

	double BatterySOC;	// %
	std::string Uploadtime;
};


/**
 * @brief 
 */
struct TinySolaX
{

	TinySolaX(const std::string& pAPIKey);
	~TinySolaX();

	void Get(const std::string& pSerialNumber,std::function<void(const RealtimeData &pData)> pReturnFunction);

private:

	const std::string mAPIKey;

	bool Download(const std::string& pURL,std::string& rJson)const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
}; //namespace solax{

#endif //TINY_SOLAX_H
