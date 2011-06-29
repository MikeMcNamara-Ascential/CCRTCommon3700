//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/ModuleInterfaces/Brake/Teves/KnorrModule.cpp 2
//
// FILE DESCRIPTION:
//	Module Interface for Knorr controller.
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
//    $Log: /CCRT/Common/ApplicationLayer/ModuleInterfaces/Brake/Teves/KnorrModule.cpp $
// 
// 
//*************************************************************************
#include "KnorrModule.h"
//*************************************************************************


template <class ProtocolFilterType>
KnorrModule<ProtocolFilterType>::KnorrModule(void) : GenericABSModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

template <class ProtocolFilterType>
KnorrModule<ProtocolFilterType>::~KnorrModule()
{
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special implementation, use the base class
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Try using the base class to get the data
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{   // Return the status of the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{   // No special method available, try using the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special implementation, use the base class
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special method, try the base class
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No specific method, use the base class
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException)
{   // No special implementation, try the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(method, value);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{   // No special method, just try the base clss
    return GenericModuleTemplate<ProtocolFilterType>::GetInfo(methodName);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::HoldValve(const string &wheel)
{
    return CommandModule("Hold" + wheel);
}

template <class ProtocolFilterType>
bool KnorrModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Initialize all the comm objects we created
    m_switchMonitorComm->Initialize(configNode);
    m_keepAliveComm->Initialize(configNode);


	// Get the signal low time
	try
	{
		SetSignalLowTime(configNode->getChild("Setup/SignalLowTime")->getValue());
	}
	catch (...)
	{
		Log("ERROR: SignalLowTime not in config file, using 5\n");
		SetSignalLowTime(25);
	}
	// Get the signal high time
	try
	{
		SetSignalHighTime(configNode->getChild("Setup/SignalHighTime")->getValue());
	}
	catch (...)
	{
		Log("ERROR: SignalHighTime not in config file, using 10\n");
		SetSignalHighTime(25);
	}
	// Get the gateway init high time and low time
	try
	{
		SetGatewayInitHighTime(atoi(configNode->getChild("Setup/GatewayInitHighTime")->getValue().c_str()));
		SetGatewayInitLowTime(atoi(configNode->getChild("Setup/GatewayInitLowTime")->getValue().c_str()));
	}
	catch (...)
	{
		Log("ERROR: GatewayInitHighTime/GatewayInitLowTime not in config file, using 1800/0\n");
		SetGatewayInitHighTime(1800);
		SetGatewayInitLowTime(0);
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

    // Call the base classes to complete initialization
    return GenericABSModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::ReadSensorSpeeds(WheelSpeeds_t &speeds, INT16 &numberOfChannels)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // report status of method
    float wheelSpeed = 0.0;  // hold the wheel speed for each module read
    // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ReadSensorSpeeds\n", ModuleName().c_str());
    // Read wheel speed from module
    status = ReadModuleData("ReadLFSensorSpeed", wheelSpeed);
    if (status == BEP_STATUS_SUCCESS)
    {   // Log and store wheel speed	
        speeds[LFWHEEL] = wheelSpeed;
        Log(LOG_DEV_DATA, "Read Sensor Speeds: LF = %g\n", speeds[LFWHEEL]);
        // Read wheel speed from module
        status = ReadModuleData("ReadRFSensorSpeed", wheelSpeed);
        if (status == BEP_STATUS_SUCCESS)
        {   // Log and store wheel speed	
            speeds[RFWHEEL] = wheelSpeed;
            Log(LOG_DEV_DATA, "Read Sensor Speeds: RF = %g\n", speeds[RFWHEEL]);
            // Read wheel speed from module
            status = ReadModuleData("ReadLRSensorSpeed", wheelSpeed);
            if (status == BEP_STATUS_SUCCESS)
            {   // Log and store wheel speed	
                speeds[LRWHEEL] = wheelSpeed;
                Log(LOG_DEV_DATA, "Read Sensor Speeds: LR = %g\n", speeds[LRWHEEL]);
                // Read wheel speed from module
                status = ReadModuleData("ReadRRSensorSpeed", wheelSpeed);
                if (status == BEP_STATUS_SUCCESS)
                {   // Log and store wheel speed	
                    speeds[RRWHEEL] = wheelSpeed;
                    Log(LOG_DEV_DATA, "Read Sensor Speeds: RR = %g\n", speeds[RRWHEEL]);                    
                    //DCBR added for tandem
                    if (numberOfChannels == 6)  //DCBR  added as function input for 3, 4, or 6 channel
                    {
                        if (status == BEP_STATUS_SUCCESS)
                        {   // Log and store wheel speed	
                            speeds[LTWHEEL] = wheelSpeed;
                            Log(LOG_DEV_DATA, "Read Sensor Speeds: LT = %g\n", speeds[LTWHEEL]);
                            status = ReadModuleData("ReadLTSensorSpeed", wheelSpeed);
                            if (status == BEP_STATUS_SUCCESS)
                            {   // Log and store wheel speed	
                                speeds[RTWHEEL] = wheelSpeed;
                                Log(LOG_DEV_DATA, "Read Sensor Speeds: RT = %g\n", speeds[RTWHEEL]);
                                status = BEP_STATUS_SUCCESS;
                            }
                            else
                            {   // Module Read failed
                                Log(LOG_DEV_DATA, "Error Reading RT Speed Sensor, Status = %s\n",
                                    ConvertStatusToResponse(status).c_str());
                            }
                        }
                        else
                        {   // Module Read failed
                            Log(LOG_DEV_DATA, "Error Reading LT Speed Sensor, Status = %s\n",
                                ConvertStatusToResponse(status).c_str());
                        }
                    }
                    else
                    {
                        status = BEP_STATUS_SUCCESS;
                    }
                }
                else
                {   // Module Read failed
                    Log(LOG_DEV_DATA, "Error Reading RR Speed Sensor, Status = %s\n",
                        ConvertStatusToResponse(status).c_str());
                }
            }
            else
            {   // Module Read failed
                Log(LOG_DEV_DATA, "Error Reading LR Speed Sensor, Status = %s\n",
                    ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {   // Module Read failed
            Log(LOG_DEV_DATA, "Error Reading RF Speed Sensor, Status = %s\n",
                ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // Module Read failed
        Log(LOG_DEV_DATA, "Error Reading LF Speed Sensor, Status = %s\n",
            ConvertStatusToResponse(status).c_str());
    }

    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ReadSensorSpeeds, Status = %s\n", ModuleName().c_str(),
        ConvertStatusToResponse(status).c_str());
    // Return the status of the operation
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::SendModuleWakeUpMessage(void)
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
		{	
			Log(LOG_ERRORS,"Exception in SendModuleWakeUpMessage()\n");
			// ensure the port is unlocked
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
BEP_STATUS_TYPE KnorrModule<ProtocolFilterType>::InitializeGateway(void)
{	// Timing is critical so need to eliminate any possible latency items
	Log(LOG_FN_ENTRY, "Entering InitializeGateway\n");
	string messageTag = "InitializeGateway";
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
		{
			INT32 attempt = 0;
			do
			{   // Assert a break so the line will be driven low for the required amount of time
				
				m_protocolFilter->GetMessage(messageTag,asciiMessage);
				Log(LOG_DEV_DATA, "Converting %s message to binary\n", asciiMessage.c_str());
				m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
				
				Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims\n", GetGatewayInitLowTime(), GetGatewayInitHighTime());
				int iCommError;
				// Reset the connection so we do not have to process all the garbage that has come in over the port
				m_protocolFilter->ResetConnection();
				if ((iCommError = m_protocolFilter->EstablishComms(GetGatewayInitLowTime(), GetGatewayInitHighTime(), xmtMessage.c_str(), xmtMessage.length())) == EOK)
				{	// Get the module response -- could take up to 500ms
					Log(LOG_DEV_DATA, "Wake up delay %ims\n", GetWakeUpResponseDelay());
					BposSleep(GetWakeUpResponseDelay());
					SerialString_t moduleResponse;
					errno = EOK;
					Log(LOG_DEV_DATA, "Getting %s message response\n", messageTag.c_str());
					// Initialization requires sending and receiving non-standard messages,
					//	this requires the use of the ProtocolFilter version of GetResponse and SendMessage 
					status = m_protocolFilter->ProtocolFilter::GetResponse(messageTag, moduleResponse);
					if (BEP_STATUS_SUCCESS == status)
					{
						initSequenceSuccess = true;
						
						messageTag = "InitializeGatewayStep2";
						// Must use ProtocolFilter version 
						moduleResponse.erase();
						status = m_protocolFilter->ProtocolFilter::SendMessage(messageTag);
						if(BEP_STATUS_SUCCESS == status)
						{
                            status = m_protocolFilter->ProtocolFilter::GetResponse(messageTag, moduleResponse);
						}
						else
						{
							Log(LOG_DEV_DATA, "Failed SendMesssage(messageTag)\n");
							BposSleep(GetWakeUpResponseDelay());
						}
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
		catch (BepException &e)
		{	
			Log(LOG_ERRORS,"Exception in SendModuleWakeUpMessage()\n");
			// ensure the port is unlocked
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
	Log(LOG_FN_ENTRY, "Exit InitializeGateway\n");
	return status;
}
