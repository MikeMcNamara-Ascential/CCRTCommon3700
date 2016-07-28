//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for the Isuzu Mimamori module.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MimamoriModule.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
MimamoriModule<ProtocolFilterType>::MimamoriModule() : GenericModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
MimamoriModule<ProtocolFilterType>::~MimamoriModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::EnterDiagnosticMode(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "MimamoriModule::EnterDiagnosticMode() - Enter");
	// Get the current time and create arguments to send to the module
	time_t currentTime = time(NULL);
	struct tm *timeStruct = localtime(&currentTime);
	SerialArgs_t timeArgs;
	timeArgs.push_back((((timeStruct->tm_year-100) / 10) << 4) | ((timeStruct->tm_year-100) % 10));
	timeArgs.push_back((((timeStruct->tm_mon+1) / 10) << 4) | ((timeStruct->tm_mon+1) % 10));
	timeArgs.push_back((((timeStruct->tm_mday) / 10) << 4) | ((timeStruct->tm_mday) % 10));  
	timeArgs.push_back((((timeStruct->tm_hour) / 10) << 4) | ((timeStruct->tm_hour) % 10));  
	timeArgs.push_back((((timeStruct->tm_min) / 10) << 4) | ((timeStruct->tm_min) % 10));    
	timeArgs.push_back((((timeStruct->tm_sec) / 10) << 4) | ((timeStruct->tm_sec) % 10));    
	if((status = CommandModule("ConnectingDeviceInfo", &timeArgs)) == BEP_STATUS_SUCCESS)
	{
		string strTemp;
		SerialString_t moduleResponse;
		if((status = ReadModuleData("CurrentTimeResponse", strTemp, NULL, &moduleResponse, NULL)) == BEP_STATUS_SUCCESS)
		{
			m_vehicleInfo.engineModel = strTemp;
			m_vehicleInfo.tmModel = ParseStringResponse("TransmissionType", moduleResponse);
			m_vehicleInfo.tireRadius = ParseFloatResponse("TireRadius", moduleResponse);
			m_vehicleInfo.finalGearRatio = ParseFloatResponse("FinalGearRatio", moduleResponse);
			m_vehicleInfo.dealerArea = ParseStringResponse("DealerArea", moduleResponse);
			m_vehicleInfo.vin = ParseStringResponse("Vin", moduleResponse);
			if((status = CommandModule("VehicleAttributeResponse")) == BEP_STATUS_SUCCESS)
			{
				status = CommandModule("VehicleNumberResponse");
			}
			else
			{
				Log(LOG_ERRORS, "Error sending vehicle attribute response to Mimamori Module");
			}
		}
		else
		{
			Log(LOG_ERRORS, "Error sending current time response to Mimamori Module");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Error sending connecting device info to Mimamori Module");
	}
	Log(LOG_FN_ENTRY, "MimamoriModule::EnterDiagnosticMode() - Exit");
	return status;
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
string MimamoriModule<ProtocolFilterType>::ModuleName(void)
{
	return string("Mimamori Module");
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
	Log(LOG_FN_ENTRY, "MimamoriModule::ReadFaults() - Enter");
	SerialString_t response(4096, 0);
	BEP_STATUS_TYPE status = m_protocolFilter->GetModuleData("ReadFaults", response);
	if(BEP_STATUS_SUCCESS == status)
	{   // Check the status if each DTC
		int currentDtcIndex = 16;
		for(UINT16 index = 0; index < 27; index++)
		{   
			char buff[8];
			string dtc = CreateMessage(buff, sizeof(buff), "%02X%02X", 
									   response[currentDtcIndex], response[currentDtcIndex+1]);
			string dtcStatus = "";
			bool report = true;
			switch(response[currentDtcIndex+2])
			{
			case 0x80:
				dtcStatus = "clear";
				report = false;
				break;

			case 0xA0:
				dtcStatus = "past failure";
				break;

			case 0xE0:
				dtcStatus = "present failure";
				break;

			default:
				dtcStatus = "unknown";
				break;
			}
			Log(LOG_DEV_DATA, "DTC Status: %s = %s", dtc.c_str(), dtcStatus.c_str());
			if(report)
			{
				faultCodes.push_back(dtc);
			}
			currentDtcIndex += 4;
		}
	}
	else
	{
		Log(LOG_ERRORS, "Failure reading DTC data from Mimamori module");
	}
	Log(LOG_FN_ENTRY, "MimamoriModule::ReadFaults() - Exit");
	return status;
}
