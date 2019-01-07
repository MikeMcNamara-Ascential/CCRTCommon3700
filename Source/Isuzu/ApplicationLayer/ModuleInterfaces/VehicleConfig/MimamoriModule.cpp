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
template <class ProtocolFilterType>
bool MimamoriModule<ProtocolFilterType>::InitializeHook (const XmlNode *configNode)
{
    bool status = false;
    // Call the base class to complete the initialization
    status = GenericModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
    
    Log(LOG_DEV_DATA, "MimamoriModule::InitializeHook: Getting Dtc Info from Config\n");
    UINT8 byte = 0x00;
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcStartIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 0;
        Log(LOG_ERRORS, "DTC Start index not defined, using 0");
    }
    Log(LOG_DEV_DATA, "Setting DTC STart Index to %d", byte);
    DtcStartIndex(&byte);

    Log(LOG_FN_ENTRY, "Exit MimamoriModule::InitializeHook(), status=%d\n", status);

    return(status);

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
	SerialString_t moduleResponse;
	m_protocolFilter->ResetConnection();
	status = m_protocolFilter->GetResponse("ConnectingDevBcast", moduleResponse);
    //if unsuccessful try again
    if (BEP_STATUS_SUCCESS != status) {
        status = m_protocolFilter->GetResponse("ConnectingDevBcast", moduleResponse);
    }
	if(BEP_STATUS_SUCCESS == status)
	{
		status = m_protocolFilter->CheckForValidResponse(moduleResponse);
		if(BEP_STATUS_SUCCESS == status)
		{
			m_protocolFilter->ExtractModuleData(moduleResponse);
			if((status = CommandModule("ConnectingDeviceInfo", &timeArgs)) == BEP_STATUS_SUCCESS)
			{
				string strTemp;
				if((status = ReadModuleData("CurrentTimeResponse", strTemp, NULL, &moduleResponse, NULL)) == BEP_STATUS_SUCCESS)
				{
					m_vehicleInfo.engineModel = strTemp;
					m_vehicleInfo.tmModel = ParseStringResponse("TransmissionType", moduleResponse);
					m_vehicleInfo.tireRadius = ParseFloatResponse("TireRadius", moduleResponse);
					m_vehicleInfo.finalGearRatio = ParseFloatResponse("FinalGearRatio", moduleResponse);
					m_vehicleInfo.dealerArea = ParseStringResponse("DealerArea", moduleResponse);
					m_vehicleInfo.vin = ParseStringResponse("Vin", moduleResponse);
					Log(LOG_DEV_DATA, "Engine Model    : %s", m_vehicleInfo.engineModel.c_str());
					Log(LOG_DEV_DATA, "Transmission    : %s", m_vehicleInfo.tmModel.c_str());
					Log(LOG_DEV_DATA, "Tire Radius     : %.2f", m_vehicleInfo.tireRadius);
					Log(LOG_DEV_DATA, "Final Gear Ratio: %.2f", m_vehicleInfo.finalGearRatio);
					Log(LOG_DEV_DATA, "VIN             : %s", m_vehicleInfo.vin.c_str());
					if((status = CommandModule("VehicleAttributeResponse")) == BEP_STATUS_SUCCESS)
					{
						status = CommandModule("VehicleNumberResponse");
//						status = m_protocolFilter->SendMessage("VehicleNumberResponse");
						//Commcheckout();
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
		}
		else
		{
			Log(LOG_ERRORS, "Invalid Mimamori broadcast message, No Comms for You!");
		}
	}
	else
	{
		Log(LOG_ERRORS, "No Mimamori broadcast message detected, no comms available to be sent");
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
    CommandModule("ReadTriggerResponse");
	BEP_STATUS_TYPE status = m_protocolFilter->GetModuleData("ReadFaults", response);
	if(BEP_STATUS_SUCCESS == status)
	{   // Check the status if each DTC
		int currentDtcIndex = DtcStartIndex();
		for(UINT16 index = 0; index < 24; index++)
		{   
			char buff[8];
			string dtc = CreateMessage(buff, sizeof(buff), "%02X%02X%02X", 
									   response[currentDtcIndex], response[currentDtcIndex+1],response[currentDtcIndex+2]);
			string dtcStatus = "";
			bool report = true;
			
			switch(response[currentDtcIndex+3])
			{
			case 0x00:
				dtcStatus = "clear";
				report = false;
				break;

			case 0xA0:
				dtcStatus = "past failure";
				break;

			case 0x01:
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
        CommandModule("ReadTriggerResponse");
   	}
	else
	{
		Log(LOG_ERRORS, "Failure reading DTC data from Mimamori module");
	}
	Log(LOG_FN_ENTRY, "MimamoriModule::ReadFaults() - Exit");
	return status;
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GatherMimamoriData()
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "MimamoriModule::GatherMimamoriData() - Enter");

	Log(LOG_FN_ENTRY, "MimamoriModule::GatherMimamoriData() - Exit");
	return status;
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
void MimamoriModule<ProtocolFilterType>::Commcheckout()
{
	Log(LOG_FN_ENTRY, "MimamoriModule::Commcheckout() - Enter");
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	SerialString_t moduleResponse;
	for(int msgCnt = 0; (msgCnt < 3); msgCnt++)
	{
		m_protocolFilter->ResetConnection();
		status = m_protocolFilter->GetResponse(moduleResponse);
		if(BEP_STATUS_SUCCESS == status)
		{
			status = m_protocolFilter->CheckForValidResponse(moduleResponse);
			if(BEP_STATUS_SUCCESS == status)
			{
				m_protocolFilter->ExtractModuleData(moduleResponse);
				// Determine which response to send back
				if((moduleResponse[9] == 0x31) && (moduleResponse[10] == 0x04))
				{
					status = m_protocolFilter->SendMessage("VehicleNumberResponse");
				}
				else
				{
					Log(LOG_DEV_DATA, "STOP!! - Got a different message back from Mimamori!! msgCount:%d", msgCnt);
					status = m_protocolFilter->SendMessage("TriggerSetup");
				}
			}
		}
	}
	Log(LOG_FN_ENTRY, "MimamoriModule::Commcheckout() - Exit");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, string & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "ReadDrmEngine"
       || methodName == "ReadDrmNees2"
       || methodName == "ReadDrmAirsus"
       || methodName == "ReadDrmIss" 
       || methodName == "ReadDrmAt"
       || methodName == "ReadDrmScr" 
       || methodName == "ReadDrmBrake"
       || methodName == "ReadDrmHsa" 
       || methodName == "ReadDrmBls"
       || methodName == "ReadDrmSrs" 
       || methodName == "ReadDrmVcEcu"
       || methodName == "ReadDrmBcm" 
       || methodName == "ReadEdrPistonOpenTemp"
       || methodName == "ReadEdrEsc" 
       || methodName == "ReadEdrAebs" 
       || methodName == "ReadEdrSrs"
        )
    {

        status = ReadModuleData(methodName, value);
    }
    else
    {
        status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    }
      /*  switch (methodName)
        {
        case "ReadDrmEngine" :
        case "ReadDrmNees2"  :
        case "ReadDrmAirsus" :
        case "ReadDrmIss"    :
        case "ReadDrmAt"     :
        case "ReadDrmScr"    :
        case "ReadDrmBrake"  :
        case "ReadDrmHsa"    :
        case "ReadDrmBls"    :
        case "ReadDrmSrs"    :
        case "ReadDrmVcEcu"  :
        case "ReadDrmBcm"    :
        case "ReadEdrPistonOpenTemp"    :
        case "ReadEdrEsc"    :
        case "ReadEdrAebs"   :
        case "ReadEdrSrs"    :
        status = ReadModuleData(methodName, value);
        break;
        default:
        status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
        }
    }*/
   // if (methodName == "ReadDrmEngine") status = ReadModuleData(methodName, value);
    
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, float &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "ReadFuelEconomy") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadTotalMileage") status = ReadModuleData(methodName,value);
    // No special implementation, try the base class
    else  status = GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, int & value)
throw(ModuleException)
{
    // No special implementation, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, UINT8 & value)
throw(ModuleException)
{
    // No special implementation, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, UINT16 & value)
throw(ModuleException)
{
    // No special implementation, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}
template <class ProtocolFilterType>
BEP_STATUS_TYPE MimamoriModule<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{
    // No special implementation, try the base class
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& MimamoriModule<ProtocolFilterType>::DtcStartIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcStartIndex = *byte;
    return m_dtcStartIndex;
}


