//*************************************************************************
// FILE:
//    $Header:  $
//
// FILE DESCRIPTION:
//		Module Interface for the INS module.
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log:  $
// 
//
//*************************************************************************
#include "InsModule.h"
#include <time.h>
#include <vector>


template <class ProtocolFilterType>
InsModule<ProtocolFilterType>::InsModule() : GenericModuleTemplate<ProtocolFilterType>()
{   // Nothing special to be done here
}

template <class ProtocolFilterType>
InsModule<ProtocolFilterType>::~InsModule()
{   // Nothing special to be done here
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::InitializeGateway(void)
{	// Timing is critical so need to eliminate any possible latency items
	Log(LOG_FN_ENTRY, "Entering InsModule::InitializeGateway");
	string messageTag = "InitializeGateway";
	string asciiMessage;
	SerialString_t xmtMessage;
	bool initSequenceSuccess = false;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// lock the port Note: this also enables ISO comm
	bool portLocked = m_protocolFilter->LockPort();
	if (portLocked)
	{
		Log(LOG_DEV_DATA, "Locked The Port");
		try
		{
			INT32 attempt = 0;
			do
			{   // Assert a break so the line will be driven low for the required amount of time
				
				m_protocolFilter->GetMessage(messageTag,asciiMessage);
				Log(LOG_DEV_DATA, "Converting %s message to binary", asciiMessage.c_str());
				m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
				
				Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims", GatewaySignalLowTime(), GatewaySignalHighTime());
				int iCommError;
				if ((iCommError = m_protocolFilter->EstablishComms(GatewaySignalLowTime(), GatewaySignalHighTime(), xmtMessage.c_str(), xmtMessage.length())) == EOK)
				{	// Get the module response -- could take up to 500ms
					Log(LOG_DEV_DATA, "Wake up delay %ims", WakeUpResponseDelay());
					BposSleep(WakeUpResponseDelay());
					SerialString_t moduleResponse;
					errno = EOK;
					Log(LOG_DEV_DATA, "Getting %s message response", messageTag.c_str());
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
							Log(LOG_DEV_DATA, "Failed SendMesssage(messageTag)");
							BposSleep(WakeUpResponseDelay());
						}
					}
					else
					{
						Log(LOG_DEV_DATA, "Waiting for module response: attempt: %d sleeping:%dms", attempt, WakeUpResponseDelay());
						BposSleep(WakeUpResponseDelay());
					}
				}
				else
				{
					Log(LOG_ERRORS, "Error asserting break: %s", strerror(iCommError));
				}
			} while (!initSequenceSuccess && (attempt++ < WakeUpAttempts()));
		}
		catch (BepException &e)
		{	
			Log(LOG_ERRORS,"Exception in SendModuleWakeUpMessage()");
			// ensure the port is unlocked
			if (portLocked)
			{
				if (m_protocolFilter->UnlockPort() == false)
					Log(LOG_ERRORS, "\tUnable to unlock the port");
				else
					Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
			}
			throw;
		}
		// unlock the port
		if (portLocked)
		{
			if (m_protocolFilter->UnlockPort() == false)
				Log(LOG_ERRORS, "\tUnable to unlock the port");
			else
				Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
		}
	}
	// Return the result of the slow baud init
	Log(LOG_FN_ENTRY, "Exit InitializeGateway");
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::SendModuleWakeUpMessage(void)
{	// Timing is critical so need to eliminate any possible latency items
	Log(LOG_FN_ENTRY, "Entering SendModuleWakeUpMessage");
	string messageTag = "ModuleWakeUp";
	string asciiMessage;
	SerialString_t xmtMessage;
	bool initSequenceSuccess = false;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// lock the port Note: this also enables ISO comm
	bool portLocked = m_protocolFilter->LockPort();
	if (portLocked)
	{
		Log(LOG_DEV_DATA, "Locked The Port");
		try
		{	// send the message
			Log(LOG_DEV_DATA, "Sending %s message", messageTag.c_str());
			if (m_protocolFilter->GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
			{	// Convert the message to binary
				Log(LOG_DEV_DATA, "Converting %s message to binary", asciiMessage.c_str());
				m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
				// Add the checksum to the message
				m_protocolFilter->AddChecksumToMessage(xmtMessage);

				INT32 attempt = 0;
				do
				{	// Look for the response
					// Assert a break so the line will be driven low for the required amount of time
					Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims", SignalLowTime(), SignalHighTime());
					int iCommError;
					if ((iCommError = m_protocolFilter->EstablishComms(SignalLowTime(), SignalHighTime(), xmtMessage.c_str(), xmtMessage.length())) == EOK)
					{	// Get the module response -- could take up to 500ms
						Log(LOG_DEV_DATA, "Wake up delay %ims", WakeUpResponseDelay());
						BposSleep(WakeUpResponseDelay());
						SerialString_t moduleResponse;
						errno = EOK;
						Log(LOG_DEV_DATA, "Getting %s message response", messageTag.c_str());
						if (BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse(messageTag, moduleResponse))
						{
							initSequenceSuccess = true;
							status = BEP_STATUS_SUCCESS;
						}
						else
						{
							Log(LOG_DEV_DATA, "Waiting for module response: attempt: %d sleeping:%dms", attempt, WakeUpResponseDelay());
							BposSleep(WakeUpResponseDelay());
						}
					}
					else
					{
						Log(LOG_ERRORS, "Error asserting break: %s", strerror(iCommError));
					}
				} while (!initSequenceSuccess && (attempt++ < WakeUpAttempts()));
			}
			else
			{	// No message provided for this tag
				Log(LOG_ERRORS, "No message provided for tag: %s", messageTag.c_str());
			}
		}
		catch (BepException &e)
		{	
			Log(LOG_ERRORS,"Exception in SendModuleWakeUpMessage()");
			// ensure the port is unlocked
			if (portLocked)
			{
				if (m_protocolFilter->UnlockPort() == false)
					Log(LOG_ERRORS, "\tUnable to unlock the port");
				else
					Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
			}
			throw;
		}
		// unlock the port
		if (portLocked)
		{
			if (m_protocolFilter->UnlockPort() == false)
				Log(LOG_ERRORS, "\tUnable to unlock the port");
			else
				Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
		}
	}
	// Return the result of the slow baud init
	Log(LOG_FN_ENTRY, "Exit SendModuleWakeUpMessage");
	return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::UpdateKFactor(UINT16 &kFactor, string tacType)
{
    Log(LOG_FN_ENTRY, "InsModule::UpdateKFactor() - Enter");
    // Initialize gateway
    BEP_STATUS_TYPE status = InitializeGateway();
    if(BEP_STATUS_SUCCESS == status)
    {
	    status = SendModuleWakeUpMessage();
	    if(BEP_STATUS_SUCCESS == status)
	    {   // Comms initialized, enter diagnostic mode
	        status = EnterDiagnosticMode();
	        if(BEP_STATUS_SUCCESS == status)
	        {   // in diag mode, unlock security
				status = UnlockModuleSecurity(tacType);
	            if(BEP_STATUS_SUCCESS == status)
	            {
					if (tacType == "INS LTC")
					{
						Log(LOG_ERRORS, "WriteKFactor - INS LTC Selected.");
                        status = WriteKFactorLtc(kFactor);
					}
					else
					{
						Log(LOG_ERRORS, "WriteKFactor - INS 2004 Selected.");
	            	status = WriteKFactor(kFactor);
					}
	            	if(BEP_STATUS_SUCCESS == status)
	            	{   // Finalize communications
	            		status = CommandModule("FinalizeDiagnostics");
	            		if(BEP_STATUS_SUCCESS == status)
	            		{   // End communications
	            			status = CommandModule("EndCommunications");
			                Log(LOG_ERRORS, "Ended communications with the module: %s", ConvertStatusToResponse(status).c_str());
	            		}
	            		else
	            		{   // Could not finalize diagnostics
			                Log(LOG_ERRORS, "Could not finalize diagnostics with the module: %s", ConvertStatusToResponse(status).c_str());
	            		}
	            	}
	            	else
	            	{   // Could not write k-factor data to the module
		                Log(LOG_ERRORS, "Could not write K-factor data to the module: %s", ConvertStatusToResponse(status).c_str());
	            	}
	            }
	            else
	            {   // Module security not unlocked
	                Log(LOG_ERRORS, "Could not unlock security: %s", ConvertStatusToResponse(status).c_str());
	            }
	        }
	        else
	        {   // Could not enter diagnostic mode
	            Log(LOG_ERRORS, "Could not enter diagnostic mode: %s", ConvertStatusToResponse(status).c_str());
	        }
	    }
	    else
	    {   // Could not wake up module
	        Log(LOG_ERRORS, "Could not wake up module: %s", ConvertStatusToResponse(status).c_str());
	    }
	}
	else
	{
        Log(LOG_ERRORS, "Could not initialize gateway module: %s", ConvertStatusToResponse(status).c_str());
    }
    Log(LOG_FN_ENTRY, "InsModule::UpdateKFactor() - Exit");
    return status;
}

template <class ProtocolFilterType>
bool InsModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Initialize the keep alive comms
    m_keepAliveComm->Initialize(configNode);
    // Store the signal low time
    INT32 lowTime = 0;
    try
    {
        lowTime = BposReadInt(configNode->getChild("Setup/SignalLowTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find signal low time, using 25ms - %s", excpt.GetReason());
        lowTime = 25;
    }
    SignalLowTime(&lowTime);
    // Store the signal high time
    INT32 highTime = 0;
    try
    {
        highTime = BposReadInt(configNode->getChild("Setup/SignalHighTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find signal high time, using 25ms - %s", excpt.GetReason());
        highTime = 25;
    }
    SignalHighTime(&highTime);

    // Store the gateway signal low time
    INT32 gatewayLowTime = 0;
    try
    {
        gatewayLowTime = BposReadInt(configNode->getChild("Setup/GatewayInitLowTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find gateway signal low time, using 25ms - %s", excpt.GetReason());
        gatewayLowTime = 1800;
    }
    GatewaySignalLowTime(&gatewayLowTime);
    // Store the gateway signal high time
    INT32 gatewayhighTime = 0;
    try
    {
        gatewayhighTime = BposReadInt(configNode->getChild("Setup/GatewayInitHighTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find gaetway signal high time, using 25ms - %s", excpt.GetReason());
        gatewayhighTime = 25;
    }
    GatewaySignalHighTime(&gatewayhighTime);

    // Store the wake up response delay
    INT32 wakeupResponseDelay = 0;
    try
    {
        wakeupResponseDelay = BposReadInt(configNode->getChild("Setup/WakeUpResponseDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find wake up response delay, using 100ms - %s", excpt.GetReason());
        wakeupResponseDelay = 100;
    }
    WakeUpResponseDelay(&wakeupResponseDelay);
    // Store the number of attempts to try to wake up the module
    INT32 wakeupAttempts = 0;
    try
    {
        wakeupAttempts = BposReadInt(configNode->getChild("Setup/WakeUpAttempts")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find wake up attempts, using 1 - %s", excpt.GetReason());
        wakeupAttempts = 1;
    }
    WakeUpAttempts(&wakeupAttempts);
    // Call base class to complete the initialization
    return GenericModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::UnlockModuleSecurity(string tacType)
{
    Log(LOG_FN_ENTRY, "InsModule::UnlockModuleSecurity() - Enter");
    // Read the security seed from the module
    UINT16 securitySeed = 0x0000;
    BEP_STATUS_TYPE status = ReadModuleData("ReadSecuritySeed", securitySeed);
	Log(LOG_FN_ENTRY, "InsModule::UnlockModuleSecurity() - securitySeed = %d",securitySeed);
    if(BEP_STATUS_SUCCESS == status)
    {   // Convert the security seed to a key to unlock the module
		SerialArgs_t key = CalculateSecurityKey(securitySeed, tacType);
        // Write the key to the module
        status = CommandModule("UnlockSecurity", &key);
        Log(LOG_ERRORS, "Unlocked module security: %s", ConvertStatusToResponse(status).c_str());
        if(BEP_STATUS_SUCCESS == status)
        {   // Update the date in the module
            status = UpdateModuleDate();
        }
    }
    else
    {
        Log(LOG_ERRORS, "Could not read security seed from the module: %s", ConvertStatusToResponse(status).c_str());
    }
    Log(LOG_FN_ENTRY, "InsModule::UnlockModuleSecurity() - Exit");
    return status;
}

template <class ProtocolFilterType>
SerialArgs_t InsModule<ProtocolFilterType>::CalculateSecurityKey(const UINT16 &seed, string tacType)
{
    SerialArgs_t key;
	UINT16 coefA, coefB;
	Log(LOG_DEV_DATA, "InsModule::CalculateSecurityKey() tacType==%s", tacType.c_str());
	if (tacType == "INS 2004")
	{
        // INS 2004
		coefA = 0x0010;
		coefB = 0x0009;
	}
	else
	{
        // INS LTC
		coefA = 0x0001;
		coefB = 0x0002;
	}
    // Calculate the security key
    UINT16 secKey = (coefA * seed) + coefB;
    key.push_back((secKey & 0xFF00) >> 8);
    key.push_back(secKey & 0x00FF);
    Log(LOG_DEV_DATA, "InsModule::CalculateSecurityKey(%04X) returning key: %04X", seed, secKey);
    return key;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::UpdateModuleDate(void)
{   // Get the current time
    time_t currentTime = time(NULL);
    struct tm *locTime = localtime(&currentTime); 
    UINT8 month = locTime->tm_mon + 1;   // Module needs 1-12, library has 0-11
    UINT8 day   = locTime->tm_mday;
    UINT8 year  = (locTime->tm_year - 90) % 10;   // Year number in the current decade since 1990
    UINT8 decade = (locTime->tm_year - 90) / 10;  // Decades since 1990
    // Construct the date bytes to send to the module
    // DT1 - bits 0-3: Month    DT2 - bits 0-3: Decade
    //       bits 4-7: Year           bits 4-7: Day
    SerialArgs_t moduleDate;
    moduleDate.push_back((year << 4) | month);
    moduleDate.push_back((day << 4) | decade);
    // Write the date to the module
    BEP_STATUS_TYPE status = CommandModule("WriteCurrentDate", &moduleDate);
    Log(LOG_ERRORS, "Wrote current date to the module: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::WriteKFactor(UINT16 &kFactor)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the current K-factor from the module
    vector <vector<UINT8> > allModuleData;
    status = CommandModule("ReadKFactorInit");
    if(BEP_STATUS_SUCCESS == status)
    {   // Initialized k-factor read,  read all data from the module
        for(UINT16 index = 0; (index < 3) && (BEP_STATUS_SUCCESS == status); index++)
        {   // Read the k-factor data from the module
		    vector<UINT8> moduleData;
            status = ReadModuleData("ReadKData", moduleData);
            Log(LOG_ERRORS, "Read k-data set: %d - %s", index, ConvertStatusToResponse(status).c_str());
            allModuleData.push_back(moduleData);
        }
        if(BEP_STATUS_SUCCESS == status)
        {   // Stop reading the k-data
            status = CommandModule("ReadKDataStop");
            // Replace the k-factor with the correct value
            allModuleData[1][2] = kFactor & 0x00FF;
            allModuleData[1][3] = ((kFactor & 0xFF00) >> 8);
            UINT16 checksum = 0x0000;
            // Calculate the checksum
            for(UINT8 index = 0; index < 3; index++)
            {
                for(UINT8 dataItem = 0; dataItem < 6; dataItem++)
                {
                    checksum += allModuleData[index][dataItem];
                }
            }
            checksum &= 0xFFFF;
            SerialArgs_t cSum;
            cSum.push_back((checksum & 0xFF00) >> 8);
            cSum.push_back(checksum & 0x00FF);
            // Write the data back to the module
            status = CommandModule("WriteKFactorInit", &cSum);
            if(BEP_STATUS_SUCCESS == status)
            {   // Write initialized, write all data back to the module
            	for(UINT8 index = 0; (index < 3) && (BEP_STATUS_SUCCESS == status); index++)
            	{   
					status = CommandModule("WriteKFactors", &allModuleData[index]);
					Log(LOG_ERRORS, "Wrote K-Factor data set %d: %s", index, ConvertStatusToResponse(status).c_str());
            	}
            	// Done writing the data
            	status = CommandModule("KFactorWriteComplete");
				Log(LOG_ERRORS, "Wrote K-Factor complete: %s", ConvertStatusToResponse(status).c_str());
            }
            else
            {   // Could not initialize the K-factor write
		        Log(LOG_ERRORS, "Could not command k-factor write to the module: %s", ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {   // Not all k_data read from the module
            Log(LOG_ERRORS, "Could not read all k-factor data from the module: %s", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // Could not command k-factor read from the module
        Log(LOG_ERRORS, "Could not command k-factor read from the module: %s", ConvertStatusToResponse(status).c_str());
    }
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE InsModule<ProtocolFilterType>::WriteKFactorLtc(UINT16 &kFactor)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	vector<UINT8> moduleData;
	status = ReadModuleData("KFactorReadLtc", moduleData);
	Log(LOG_ERRORS, "Read LTC k-data: %s", ConvertStatusToResponse(status).c_str());
	if (BEP_STATUS_SUCCESS == status)
	{
		// Replace the k-factor with the correct value
		moduleData[0] = kFactor & 0x00FF;
		moduleData[1] = ((kFactor & 0xFF00) >> 8);
		moduleData[2] = 0xFF;
		moduleData[3] = 0xFF;
		status = CommandModule("KFactorWriteLtc", &moduleData);
		Log(LOG_ERRORS, "Wrote K-Factor LTC data: %s", ConvertStatusToResponse(status).c_str());
	}
	else
	{	// Not all k_data read from the module
		Log(LOG_ERRORS, "Could not read all k-factor data from the module: %s", ConvertStatusToResponse(status).c_str());
	}
	return status;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::SignalLowTime(INT32 *lowTime) /*= NULL*/
{
    if(lowTime != NULL)  m_lineLowTime = *lowTime;
    return m_lineLowTime;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::SignalHighTime(INT32 *highTime) /*= NULL*/
{
    if(highTime != NULL)  m_lineHighTime = *highTime;
    return m_lineHighTime;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::GatewaySignalLowTime(INT32 *lowTime)	/*= NULL*/
{
    if(lowTime != NULL)  m_gatewayLineLowTime = *lowTime;
    return m_gatewayLineLowTime;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::GatewaySignalHighTime(INT32 *highTime) /*= NULL*/
{
    if(highTime != NULL)  m_gatewayLineHighTime = *highTime;
    return m_gatewayLineHighTime;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::WakeUpResponseDelay(INT32 *delayTime) /*= NULL*/
{
    if(delayTime != NULL)  m_wakeUpResponseDelay = *delayTime;
    return m_wakeUpResponseDelay;
}

template <class ProtocolFilterType>
const inline INT32& InsModule<ProtocolFilterType>::WakeUpAttempts(INT32 *attempts) /*= NULL*/
{
    if(attempts != NULL)  m_wakeUpAttempts = *attempts;
    return m_wakeUpAttempts;
}
