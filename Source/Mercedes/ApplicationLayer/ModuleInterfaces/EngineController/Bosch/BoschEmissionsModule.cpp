//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for DaimlerChrysler NGC-III controller.  This controller
//  incorporates the engine controller and the transmission controller into one
//  physical module.  The transmission control portion and the engine control
//  portion have different module IDs.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/ModuleInterfaces/EngineController/Bosch/BoschEmissionsModule.cpp $
// 
// 2     6/09/05 2:00p Derickso
// Added support for CAN bus.
// 
// 1     2/22/05 11:58a Derickso
// New engine controller.
// 
#include "BoschEmissionsModule.h"
//------------------------------------------------------------------------------

template <class ProtocolFilterType>
BoschEmissionsModule<ProtocolFilterType>::BoschEmissionsModule(void) : 
GenericEmissionsModuleTemplate<ProtocolFilterType>()
{	// Nothing special to do
}

template <class ProtocolFilterType>
BoschEmissionsModule<ProtocolFilterType>::~BoschEmissionsModule() 
{	//Do Nothing
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{	// No special implementation, return the status of the base class operation
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{	// No special implementation, return the status of the base class operation
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}
template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{	// No special implementation, return the status of the base class operation
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException)
{
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, vector<UINT8>&value) throw(ModuleException)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	if (methodName == "ReadEOLStatusBytes")	  status = ReadEOLStatusBytes(value);
	else	status = GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException)
{   
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Determine if the vehicle has previously passed rolls testing
	if (methodName == "IsPreviousPass")
	{
		// CAN implementation is different than ISO-K
		if (!m_configNode->getChild("Setup/CommunicationBus")->getValue().compare("CAN"))
		{
			status = ReadModuleData("IsPreviousPass", value);
			Log(LOG_DEV_DATA, "CAN previous pass: %s", value ? "True" : "False");
		}
		else
		{
			status = IsPreviousPass(value);
			Log(LOG_DEV_DATA, "ISO-K previous pass: %s", value ? "True" : "False");
		}
	}
    else if (methodName == "ReadParkNeutralSwitchOn")
    {  // Check if the transmission is in Neutral
        status = ReadModuleData(methodName, value);
    }
	else
	{   // No special implementation, return the status of the base class operation
		status = GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
	}
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t &value) throw(ModuleException)
{	// No special implementation, return the status of the base class operation
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{   
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Read the throttle position from the module
	if (!methodName.compare("ReadThrottlePosition"))  status = ReadModuleData(methodName, value);
	// No special implementation, return the status of the base class operation
	else  status = GenericEmissionsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
	// Return the status
    return status;
}

template <class ProtocolFilterType>
inline string BoschEmissionsModule<ProtocolFilterType>::ModuleName()
{
	return "Bosch Engine Controller";
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::SetupDynamicLID(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Make sure we clear the dynamic LID first
	status = ClearDynamicLID();
	if (BEP_STATUS_SUCCESS == status)
	{
		status = CommandModule("SetupDynamicLID");
	}
	else
	{
		Log(LOG_ERRORS, "Communication failure clearing the dynamic LID - status: %s", ConvertStatusToResponse(status).c_str());
	}
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ClearDynamicLID(void)
{
	return CommandModule("ClearDynamicLID");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::UpdateEEPROMTestBit(const UINT16 address, 
																			  const UINT16 testBit,
																			  const EEPROM_TEST_RESULT testResult)
{	// Create the mask for the bit we are interested in
	UINT8 bitMask = 1 << testBit;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	vector<UINT8> eolStatusBytes;
	DataAnalysis analyze;

	// Read the status bytes from the module
	status = ReadEOLStatusBytes(eolStatusBytes);
	if (BEP_STATUS_SUCCESS == status)
	{	// Clear the appropriate bit
		UINT8 originalValue = eolStatusBytes[address];
		eolStatusBytes[address] = analyze.ConvertData(eolStatusBytes[address], bitMask, testResult == EEPROM_PASS ? CLEAR_BITS : SET_BITS);
		// Write the data back to the module
		status = WriteEOLStatusBytes(eolStatusBytes);
		Log(LOG_DEV_DATA, "Updated EOL status byte %d from %02X to %02X: %s", 
			address, originalValue, eolStatusBytes[address], ConvertStatusToResponse(status).c_str());
	}
	else
	{	// Failure reading status bytes
		Log(LOG_ERRORS, "Error reading status bytes from the module: %s", ConvertStatusToResponse(status).c_str());
	}
	// return the status
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::WriteEOLStatusBytes(const vector<UINT8> &statusBytes)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialArgs_t args;
	// Build the argument list to send to the module
	args.assign(statusBytes.begin(), statusBytes.end());
	// Write the data to the module
	status = CommandModule("WriteEOLStatusBytes", &args);
	Log(LOG_ERRORS, "Wrote EOL status bytes to the module: %s", ConvertStatusToResponse(status).c_str());
	return status;
}

template <class ProtocolFilterType>
bool BoschEmissionsModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{	// Store the type translation tables
	try
	{
		m_bodyStyleMapping.DeepCopy(configNode->getChild("Setup/BodyStyles")->getChildren());
		m_engineControllerMapping.DeepCopy(configNode->getChild("Setup/EngineControllers")->getChildren());
		m_engineTypeMapping.DeepCopy(configNode->getChild("Setup/EngineTypes")->getChildren());
		m_emissionsTypeMapping.DeepCopy(configNode->getChild("Setup/EmissionsTypes")->getChildren());       
		m_transmissionTypeMapping.DeepCopy(configNode->getChild("Setup/TransmissionTypes")->getChildren());     
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlException loading module type translation tables: %s\n", excpt.GetReason());
	}
	// Get the signal low time
	try
	{
		SetSignalLowTime(configNode->getChild("Setup/SignalLowTime")->getValue());
	}
	catch (...)
	{
		Log("ERROR: SignalLowTime not in config file, using 5\n");
		SetSignalLowTime(5);
	}
	// Get the signal high time
	try
	{
		SetSignalHighTime(configNode->getChild("Setup/SignalHighTime")->getValue());
	}
	catch (...)
	{
		Log("ERROR: SignalHighTime not in config file, using 10\n");
		SetSignalHighTime(10);
	}
	// Save the response delay time for the wake up message
	try
	{
		SetWakeUpResponseDelay(configNode->getChild("Setup/WakeUpResponseDelay")->getValue());
	}
	catch (...)
	{
		Log("ERROR: WakeUpResponseDelay not in config file, using 100ms\n");
		SetWakeUpResponseDelay(100);
	}
	// Get the number of attempts to establish comms with the module
	try
	{
		SetWakeUpAttempts(configNode->getChild("Setup/WakeUpAttempts")->getValue());
	}
	catch (...)
	{
		Log("ERROR: WakeUpAttempts not in config file, using 3\n");
		SetWakeUpAttempts(3);
	}
	// Get the security seed information
	try
	{
		StoreFirstSecuritySeedIndex(BposReadInt(configNode->getChild("Setup/SecuritySeedStartIndex")->getValue().c_str()));
		StoreNumberOfSecuritySeedBytes(BposReadInt(configNode->getChild("Setup/NumberOfSecuritySeedBytes")->getValue().c_str()));
		StoreSecurityKeyGen(BposReadInt(configNode->getChild("Setup/SecurityKeyGeneration")->getValue().c_str()));
	}
	catch (...)
	{
		Log(LOG_ERRORS, "ERROR: Byte information not specified for security seed");
		StoreFirstSecuritySeedIndex(5);
		StoreNumberOfSecuritySeedBytes(3);
		StoreSecurityKeyGen(0);
	}
	// Store the status byte information
	try
	{
		StoreNumberOfStatusBytes(BposReadInt(configNode->getChild("Setup/NumberOfEOLStatusBytes")->getValue().c_str()));
		StoreFirstStatusByteIndex(BposReadInt(configNode->getChild("Setup/EOLStatusByteStartIndex")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Byte information not specified for EOL status bytes: %s", excpt.what());
		StoreNumberOfStatusBytes(3);
		StoreFirstStatusByteIndex(2);
	}
	// Store the module communications BUS
	try
	{
		StoreCommunicationsBus(configNode->getChild("Setup/CommunicationBus")->getValue());

	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Module Config CommunicationBus tag missing. Default to NotSpecified: %s\n", excpt.GetReason());
		StoreCommunicationsBus("NotSpecified");
	}

	//Initialize the keep alive    
	m_keepAliveComm->Initialize(configNode);
	m_switchMonitorComm->Initialize(configNode);
	// Call the base classes to finish off the initialization
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ReadEOLStatusRegister(const UINT16 &eolStatusRegister, 
																				UINT16 &registerContents)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	vector<UINT8> eolStatusBytes;
	// Read the status bytes from the module
	status = ReadEOLStatusBytes(eolStatusBytes);
	if (BEP_STATUS_SUCCESS == status)
	{
		registerContents = 0x0000 | eolStatusBytes[eolStatusRegister];
		Log(LOG_DEV_DATA, "EOL StatusRegister %04X: %04X", eolStatusRegister, registerContents);
	}
	else
	{
		Log(LOG_ERRORS, "Error reading eol status bytes - status: %s", ConvertStatusToResponse(status).c_str());
	}
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ReadEOLStatusBytes(vector<UINT8> &statusBytes)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t moduleResponse;
	statusBytes.clear();
	statusBytes.reserve(GetNumberOfStatusBytes());
	// Read the status bytes from the module
	status = m_protocolFilter->GetModuleData("ReadEOLStatusBytes", moduleResponse);
	if (BEP_STATUS_SUCCESS == status)
	{	// Good read, extract the data from the response
		for (INT32 index = GetFirstStatusByteIndex(); index < (GetFirstStatusByteIndex() + GetNumberOfStatusBytes()); index++)
		{
			statusBytes.push_back(moduleResponse[index]);
		}
	}
	else
	{	// Communication failure reading the status bytes
		Log(LOG_ERRORS, "Communication failure reading EOL status bytes - status: %s", ConvertStatusToResponse(status).c_str());        
	}
	// Return the status
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::UnlockModuleSecurity()
{
	SerialString_t moduleResponse;
	SerialArgs_t securityKey;  // Security unlock key
	bool securityUnlocked = false;
	INT32 attempt = 0;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

	Log(LOG_FN_ENTRY, "Entering UnlockModuleSecurity\n");
	// lock the port Note: this also enables ISO comm
	bool portLocked = m_protocolFilter->LockPort();
	if (portLocked)
	{
		Log(LOG_DEV_DATA, "Locked The Port\n");
		// Wake up the module
		do
		{
			SerialArgs_t seed;
			if (BEP_STATUS_SUCCESS == RequestSecuritySeed(seed, "ReadSecuritySeed"))
			{	// Check if the module is already unlocked
				bool unlocked = true;
				for (UINT16 index = 0; (index < seed.size()) && unlocked; index++)
				{
					unlocked = seed[index] == 0x00;
				}
				if (!unlocked)
				{
					// Calculate the access key
					switch (GetSecurityKeyGen())
					{
					case 0:
						status = CalculateSecurityKeyGen0(seed, securityKey);
						break;
	
					case 1:
						status = CalculateSecurityKeyGen1(seed, securityKey);
						break;
	
					default:
						// No Default
						status = BEP_STATUS_SOFTWARE;
						break;
					}
					// Check the status of the key generation
					if (BEP_STATUS_SUCCESS == status)
					{	// Calculated the security key, unlock the module   
						if (BEP_STATUS_SUCCESS == CommandModule("SendAccessKey", &securityKey))
						{	// Security unlock message successful
							Log(LOG_DEV_DATA, "Module security unlocked\n");
							status = BEP_STATUS_SUCCESS;
							securityUnlocked = true;
						}
						else
						{	// Security unlock failed
							Log(LOG_ERRORS, "Failed to unlock module security (command not sent)\n");
							status = BEP_STATUS_FAILURE;
						}
					}
					else
					{	//Could not calculate key
						Log(LOG_ERRORS, "Failed to calculate the security key - status: %s\n",
							ConvertStatusToResponse(status).c_str());  
					}                   
				}
				else
				{
					Log(LOG_DEV_DATA, "Module security ALREADY unlocked!");
					status = BEP_STATUS_SUCCESS;
					securityUnlocked = true;
				}
			}
			else
			{	// Error requesting the seed from the module
				Log(LOG_ERRORS, "Error requesting seed from the module\n");
			}
			// if this was not successful, delay and attempt again
			if (!securityUnlocked)	 BposSleep(250);
		} while ((attempt++ < GetWakeUpAttempts()) && !securityUnlocked);
		// unlock the port
		if (portLocked)
		{
			if (m_protocolFilter->UnlockPort() == false)
				Log(LOG_ERRORS, "\tUnable to unlock the port\n");
			else
				Log(LOG_DEV_DATA, "\tPort Successfully Unlocked\n");
		}
	}
	// Return the result of the unlock attempt
	Log(LOG_FN_ENTRY, "Exit UnlockModuleSecurity\n");
	return status;
}
template <class ProtocolFilterType>
inline BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ReadAxleSpeed(const string &axle, float &axleSpeed)
{	// Get the axle speed from the module
	return ReadModuleData("ReadAxleSpeed" + axle, axleSpeed);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ReadSensors(INT32 &engineRPM,
																	  INT32 &boostPressure,
																	  float &vehicleSpeed,
																	  float &throttlePosition,
																	  string &cruiseMode)   
{	// Initialize the data
	engineRPM = 0;
	boostPressure = 0;
	boostPressure = 0;
	vehicleSpeed = 0.0;
	throttlePosition = 0.0;
	cruiseMode = "Unknown";
	// Send the command to read the sensors
	SerialString_t moduleResponse;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	if (GetCommunicationsBus() != "CAN")
	{	//Non Can Dynamic LID reads
		status = m_protocolFilter->GetModuleData("ReadSensors", moduleResponse);
		// Check the status of the read
		if (status == BEP_STATUS_SUCCESS)
		{	// Extract the data from the response
			engineRPM = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadEngineRPM"), moduleResponse);
			boostPressure = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadBoostPressure"), moduleResponse);
			vehicleSpeed = ParseFloatResponse(GetReplyInterpretationTag("ReadVehicleSpeed"), moduleResponse);
			throttlePosition = ParseFloatResponse(GetReplyInterpretationTag("ReadThrottlePosition"), moduleResponse);
			cruiseMode = ParseStringResponse(GetReplyInterpretationTag("ReadCruiseMode"), moduleResponse);
		}
		else
		{
			Log(LOG_ERRORS, "Communication failure in reading sensors from the module - status: %s",
				ConvertStatusToResponse(status).c_str());
		}
	}
	else
	{	//Read sensors from the 2 sensor LIDS and cruise mode form cruise LID
		SerialString_t moduleResponseLID2;
		SerialString_t moduleResponseCruise;
		status = m_protocolFilter->GetModuleData("ReadSensorLID1", moduleResponse);
		// Check the status of 1st sensor LID read
		if (status == BEP_STATUS_SUCCESS)
		{	//Read 2nd sensor LID
			status = m_protocolFilter->GetModuleData("ReadSensorLID2", moduleResponseLID2);
			//Check the status of 2nd sensor LID read
			if (status == BEP_STATUS_SUCCESS)
			{  //Read Cruise mode
				status = m_protocolFilter->GetModuleData("ReadCruiseMode", moduleResponseCruise); 
				//Check the status of Cruise mode LID read
				if (status == BEP_STATUS_SUCCESS)
				{  // Extract the data from the 1st LID
					engineRPM = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadEngineRPM"), moduleResponse);
					boostPressure = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadBoostPressure"), moduleResponse);
					vehicleSpeed = ParseFloatResponse(GetReplyInterpretationTag("ReadVehicleSpeed"), moduleResponseLID2);
					throttlePosition = ParseFloatResponse(GetReplyInterpretationTag("ReadThrottlePosition"), moduleResponse);
					cruiseMode = ParseStringResponse(GetReplyInterpretationTag("ReadCruiseMode"), moduleResponseCruise);
				}
				else
				{
					Log(LOG_ERRORS, "Communication failure in reading ReadCruiseMode message the module - status: %s",
						ConvertStatusToResponse(status).c_str());
				}
			}
			else
			{
				Log(LOG_ERRORS, "Communication failure in reading sensors ReadSensorLID1 message the module - status: %s",
					ConvertStatusToResponse(status).c_str());
			}
		}
		else
		{
			Log(LOG_ERRORS, "Communication failure in reading sensors ReadSensorLID2 message the module - status: %s",
				ConvertStatusToResponse(status).c_str());
		}
	}
	// Return the status
	Log(LOG_FN_ENTRY, "ReadSensors returning %s -- RPM: %04d, BoostPressure: %d, Speed: %5.2f, TPS: %5.2f, CruiseMode: %s\n",
		ConvertStatusToResponse(status).c_str(), engineRPM, boostPressure, vehicleSpeed, throttlePosition, cruiseMode.c_str());
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::ReadSensors(INT32 &engineRPM,
																	  INT32 &boostPressure,
																	  float &vehicleSpeed,
																	  float &throttlePosition,
																	  string &cruiseMode,
                                                                             bool &parkNeutralSwitch)
{	// Initialize the data
	engineRPM = 0;
	boostPressure = 0;
	boostPressure = 0;
	vehicleSpeed = 0.0;
	throttlePosition = 0.0;
	cruiseMode = "Unknown";
	parkNeutralSwitch = false;
    
	// Send the command to read the sensors
	SerialString_t moduleResponse;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	//Can Dynamic LID reads
    status = m_protocolFilter->GetModuleData("ReadSensors", moduleResponse);
    // Check the status of the read
    if (status == BEP_STATUS_SUCCESS)
    {	// Extract the data from the response
        engineRPM = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadEngineRPM"), moduleResponse);
        boostPressure = ParseSignedIntegerResponse(GetReplyInterpretationTag("ReadBoostPressure"), moduleResponse);
        vehicleSpeed = ParseFloatResponse(GetReplyInterpretationTag("ReadVehicleSpeed"), moduleResponse);
        throttlePosition = ParseFloatResponse(GetReplyInterpretationTag("ReadThrottlePosition"), moduleResponse);
        cruiseMode = ParseStringResponse(GetReplyInterpretationTag("ReadCruiseMode"), moduleResponse);
        parkNeutralSwitch = ParseBooleanResponse(GetReplyInterpretationTag("ReadParkNeutralSwitchOn"), moduleResponse);
    }
    else
    {
        Log(LOG_ERRORS, "Communication failure in reading sensors from the module - status: %s",
            ConvertStatusToResponse(status).c_str());
    }
	// Return the status
	Log(LOG_FN_ENTRY, "ReadSensors returning %s -- RPM: %04d, BoostPressure: %d, Speed: %5.2f, TPS: %5.2f, CruiseMode: %s, ParkNeutral: %d\n",
		ConvertStatusToResponse(status).c_str(), engineRPM, boostPressure, vehicleSpeed, throttlePosition, cruiseMode.c_str(), parkNeutralSwitch);
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::SendModuleWakeUpMessage(void)
{	// Timing is critical so need to eliminate any possible latency items
	Log(LOG_FN_ENTRY, "Entering SendModuleWakeUpMessage\n");
	string messageTag = "ModuleWakeUp";
	string asciiMessage;
	SerialString_t xmtMessage;
	bool initSequenceSuccess = false;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

	// lock the port Note: this also enables ISO comm
	bool portLocked = m_protocolFilter->LockPort();
	if (portLocked)
	{
		Log(LOG_DEV_DATA, "Locked The Port\n");
		try
		{	// send the message
			Log(LOG_DEV_DATA, "Sending %s message\n", messageTag.c_str());
			if (m_protocolFilter->GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
			{	// Convert the message to binary
				Log(LOG_DEV_DATA, "Converting %s message to binary\n", asciiMessage.c_str());
				m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
				// Add the checksum to the message
				m_protocolFilter->AddChecksumToMessage(xmtMessage);

				INT32 attempt = 0;
				do
				{	// Look for the response
					// Assert a break so the line will be driven low for the required amount of time
					Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims\n", GetSignalLowTime(), GetSignalHighTime());
					int iCommError;
					if ((iCommError = m_protocolFilter->EstablishComms(GetSignalLowTime(), GetSignalHighTime(), xmtMessage.c_str(), xmtMessage.length())) == EOK)
					{	// Get the module response -- could take up to 500ms
						Log(LOG_DEV_DATA, "Wake up delay %ims\n", GetWakeUpResponseDelay());
						BposSleep(GetWakeUpResponseDelay());
						SerialString_t moduleResponse;
						errno = EOK;
						Log(LOG_DEV_DATA, "Getting %s message response\n", messageTag.c_str());
						if (BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse(messageTag, moduleResponse))
						{
							initSequenceSuccess = true;
							status = BEP_STATUS_SUCCESS;
						}
						else
						{
							Log(LOG_DEV_DATA, "Waiting for module response: attempt: %d sleeping:%dms\n", attempt, GetWakeUpResponseDelay());
							BposSleep(GetWakeUpResponseDelay());
						}
					}
					else
					{
						Log(LOG_ERRORS, "Error asserting break: %s\n", strerror(iCommError));
					}
				} while (!initSequenceSuccess && (attempt++ < GetWakeUpAttempts()));
			}
			else
			{	// No message provided for this tag
				Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
			}
		}
		catch (BepException &e)
		{	// ensure the port is unlocked
			if (portLocked)
			{
				if (m_protocolFilter->UnlockPort() == false)
					Log(LOG_ERRORS, "\tUnable to unlock the port\n");
				else
					Log(LOG_DEV_DATA, "\tPort Successfully Unlocked\n");
			}
			throw;
		}

		// unlock the port
		if (portLocked)
		{
			if (m_protocolFilter->UnlockPort() == false)
				Log(LOG_ERRORS, "\tUnable to unlock the port\n");
			else
				Log(LOG_DEV_DATA, "\tPort Successfully Unlocked\n");
		}
	}
	// Return the result of the slow baud init
	Log(LOG_FN_ENTRY, "Exit SendModuleWakeUpMessage\n");
	return status;
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::RequestSecuritySeed(SerialArgs_t &seed, const std::string &messageTag)
{
	SerialString_t moduleResponse;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string seedMessage;
	char buff[64];
	// Attempt to read the security see from the module
	status = m_protocolFilter->GetModuleData(messageTag, moduleResponse);
	if (BEP_STATUS_SUCCESS == status)
	{	// Extract the seed from the response
		for (INT32 index = GetFirstSecuritySeedIndex(); index < (GetFirstSecuritySeedIndex() + GetNumberOfSecuritySeedBytes()) ; index++)
		{
			seed.push_back(moduleResponse[index]);
			seedMessage += CreateMessage(buff, sizeof(buff), " $%02X", moduleResponse[index]);
		}
		Log(LOG_DEV_DATA, "Received security seed: %s", seedMessage.c_str());
	}
	else
	{
		Log(LOG_ERRORS, "Error reading security seed from the module - status: %s\n", ConvertStatusToResponse(status).c_str());
	}
	// Return the status
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::CalculateSecurityKeyGen0(const SerialArgs_t &seed, SerialArgs_t &key)
{	// Calculate the key 
	unsigned char keyMaskTable[8][3] = 
	{
		{0x9e, 0x5c, 0x63},
		{0x22, 0x12, 0x82},
		{0x62, 0x44, 0x84},
		{0x92, 0x5b, 0xb1},
		{0x14, 0xe5, 0x76},
		{0xf2, 0x54, 0x26},
		{0x54, 0x2a, 0xb2},
		{0x36, 0xe9, 0x03}
	};

	// Calculate the index for mask selection
	Log(LOG_DEV_DATA, "\tMasking seed\n");
	unsigned keyIndexMask = 0;
	if (seed[1] & 0x20)
		keyIndexMask |= 0x01;
	if (seed[0] & seed[2] & 0x80)
		keyIndexMask |= 0x02;
	if (seed[0] & seed[2] & 0x04)
		keyIndexMask |= 0x04;

	// Calculate the key
	// Note: Push these on in reverse order
	UINT32 index;
	for (index = 0; index < 3; index++)
	{
		Log(LOG_DEV_DATA, "\tPushing byte into key\n");
		key.push_back((UINT8) (seed[index] ^ keyMaskTable[keyIndexMask][index]));
	}
	Log(LOG_DEV_DATA, "Calculated security key: $%02X $%02X $%02X\n", key[0], key[1], key[2]);

	// Return success
	return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::CalculateSecurityKeyGen1(const SerialArgs_t &seed, SerialArgs_t &key)
{
	UINT8 lookupTable[8][4];
	UINT8 tempKey[4];
	UINT8 tableRow = 0;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Load the lookup table
	if ((status = LoadLookupTable(lookupTable)) == BEP_STATUS_SUCCESS)
	{	// Determine which row in the table to use
		if (seed[2] & 0x40)				 tableRow = 1;
		if ((seed[1] ^ seed[0]) & 0x04)	 tableRow += 2;
		if ((seed[1] ^ seed[0]) & 0x20)	 tableRow += 4;
		// Set the first round of the security key
		tempKey[0] = seed[0] ^ lookupTable[tableRow][0];
		tempKey[1] = seed[1] ^ lookupTable[tableRow][1];
		tempKey[2] = seed[2] ^ lookupTable[tableRow][2];
		tempKey[3] = seed[3] ^ lookupTable[tableRow][3];
		// Determine the next row of the table to use
		tableRow = 0;
		if (seed[2] & 0x02)				 tableRow = 1;
		if (tempKey[2]  & 0x20)			 tableRow += 2;
		if ((seed[1] ^ seed[0]) & 0x80)	 tableRow += 4;
		// Finish calculating the security key
		tempKey[0] = tempKey[0] ^ lookupTable[tableRow][3];
		tempKey[1] = tempKey[1] ^ lookupTable[tableRow][0];
		tempKey[2] = tempKey[2] ^ lookupTable[tableRow][1];
		tempKey[3] = tempKey[3] ^ lookupTable[tableRow][2];
		// Load the key into the return variable
		key.push_back(tempKey[0]);
		key.push_back(tempKey[1]);
		key.push_back(tempKey[2]);
		key.push_back(tempKey[3]);
		Log(LOG_DEV_DATA, "Calculated Security Key:   %02X %02X %02X %02X\n", key[0], key[1], key[2], key[3]);
	}
	else
	{
		Log(LOG_ERRORS, "Could not load security table - status: %s", ConvertStatusToResponse(status).c_str());
	}
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::LoadLookupTable(UINT8 lookupTable[8][4])
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	FILE *tableFile;
	// Open the lookup table file
	string fileName = string(getenv("USR_ROOT")) + "/Configuration/Components/Emissions/" + m_configNode->getChild("Setup/SecurityLookupTable")->getValue();
	if ((tableFile = fopen(fileName.c_str(), "r")) != NULL)
	{	// Read each line from the file and store into our table
		UINT32 temp1, temp2, temp3, temp4;
		char tableBuffer[128];
		UINT8 row = 0;
		while ((fgets(tableBuffer, sizeof(tableBuffer), tableFile) != NULL) && (row < 8))
		{
			sscanf(tableBuffer, "%X, %X, %X, %X", &temp1, &temp2, &temp3, &temp4);
			lookupTable[row][0] = temp1;
			lookupTable[row][1] = temp2;
			lookupTable[row][2] = temp3;
			lookupTable[row][3] = temp4;
			Log(LOG_DEV_DATA, "Lookup Table Row %d: %02X %02X %02X %02X", 
				row, lookupTable[row][0], lookupTable[row][1], lookupTable[row][2], lookupTable[row][3]);
			row++;
		}
		// Done reading in file, close it
		fclose(tableFile);
		status = BEP_STATUS_SUCCESS;
	}
	else
	{
		Log(LOG_ERRORS, "Could not open security lookup table: %s - error: %s", fileName.c_str(), strerror(errno));
		status = BEP_STATUS_SOFTWARE;
	}
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::StartSwitchMonitor(const bool &isAutoTrans) throw(ModuleException)
{	// do not call the base emissions monitor but instead the base module monitor
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	Log(LOG_DEV_DATA, "Starting switch monitor - trans type: %s", isAutoTrans ? "Automatic" : "Manual");
	SetTransmissionType(isAutoTrans);
	moduleStatus = GenericModuleTemplate<ProtocolFilterType>::StartSwitchMonitor();
	// Return the status
	return moduleStatus;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE BoschEmissionsModule<ProtocolFilterType>::StartSwitchMonitor(void) throw(ModuleException)
{	// Just call the base class
	return GenericEmissionsModuleTemplate<ProtocolFilterType>::StartSwitchMonitor();
}

template <class ProtocolFilterType>
inline const INT32& BoschEmissionsModule<ProtocolFilterType>::GetNumberOfSecuritySeedBytes(void)
{
	return m_numberOfSeedBytes;
}

template <class ProtocolFilterType>
inline const INT32& BoschEmissionsModule<ProtocolFilterType>::GetFirstSecuritySeedIndex(void)
{
	return m_seedStartIndex;
}

template <class ProtocolFilterType>
inline const INT32& BoschEmissionsModule<ProtocolFilterType>::GetNumberOfStatusBytes(void)
{
	return m_numberOfStatusBytes;
}

template <class ProtocolFilterType>
inline const INT32& BoschEmissionsModule<ProtocolFilterType>::GetFirstStatusByteIndex(void)
{
	return m_statusByteStartIndex;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreNumberOfSecuritySeedBytes(const INT32 &numberOfBytes)
{
	m_numberOfSeedBytes = numberOfBytes;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreFirstSecuritySeedIndex(const INT32 &index)
{
	m_seedStartIndex = index;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreSecurityKeyGen(const INT32 &keyGen)
{
	m_securityKeyGen = keyGen;
}

template <class ProtocolFilterType>
inline const INT32& BoschEmissionsModule<ProtocolFilterType>::GetSecurityKeyGen(void)
{
	return m_securityKeyGen;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreNumberOfStatusBytes(const INT32 &numberOfBytes)
{
	m_numberOfStatusBytes = numberOfBytes;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreFirstStatusByteIndex(const INT32 &index)
{
	m_statusByteStartIndex = index;
}

template <class ProtocolFilterType>
inline void BoschEmissionsModule<ProtocolFilterType>::StoreCommunicationsBus(const string &communicationsBus)
{
	m_moduleCommunicationsBus = communicationsBus;
}   
