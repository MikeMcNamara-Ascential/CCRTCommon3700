//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "DensoAirbagModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DensoAirbagModule<ProtocolFilterType>::DensoAirbagModule() : GenericModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DensoAirbagModule<ProtocolFilterType>::~DensoAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string DensoAirbagModule<ProtocolFilterType>::ModuleName()
{
    return "Denso Airbag Module";
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE DensoAirbagModule<ProtocolFilter>::PerformModuleLinkup(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "DensoAirbagModule::PerformModuleLinkup() - Enter");
    // Lock the port
    bool portLocked = true;/*m_protocolFilter->Lockport(0);*/
    if(portLocked)
    {
        string messageTag = "ModuleWakeUp";
        string asciiMessage;
        SerialString_t xmtMessage;
        bool initSequenceSuccess = false;
        Log(LOG_DEV_DATA, "Locked The Port");
        try
        {   // send the message
            Log(LOG_DEV_DATA, "Sending %s message", messageTag.c_str());
            if(m_protocolFilter->GetMessage(messageTag,asciiMessage) == BEP_STATUS_SUCCESS)
            {   // Convert the message to binary
                Log(LOG_DEV_DATA, "Converting %s message to binary\n", asciiMessage.c_str());
                m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
                // Add the checksum to the message
                m_protocolFilter->AddChecksumToMessage(xmtMessage);

                INT32 attempt = 0;
                do
                {   // Look for the response
                    // Assert a break so the line will be driven low for the required amount of time
                    Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims\n", 
                        SignalLowTime(), SignalHighTime());
                    int iCommError = m_protocolFilter->EstablishComms(SignalLowTime(), 
                                                                      SignalHighTime(), 
                                                                      xmtMessage.c_str(), 
                                                                      xmtMessage.length());
                    if(iCommError == EOK)
                    {   // Get the module response -- could take up to 500ms
                        Log(LOG_DEV_DATA, "Wake up delay %ims\n", WakeUpResponseDelay());
                        BposSleep(WakeUpResponseDelay());
                        SerialString_t moduleResponse;
                        errno = EOK;
                        Log(LOG_DEV_DATA, "Getting %s message response\n", messageTag.c_str());
                        if(BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse(messageTag, moduleResponse))
                        {
                            initSequenceSuccess = true;
                            status = BEP_STATUS_SUCCESS;
                        }
                        else
                        {
                            Log(LOG_DEV_DATA, "Waiting for module response: attempt: %d sleeping:%dms\n", 
                                attempt, WakeUpResponseDelay());
                            BposSleep(WakeUpResponseDelay());
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Error asserting break: %s\n", strerror(iCommError));
                    }
                } while(!initSequenceSuccess && (attempt++ < WakeUpAttempts()));
            }
            else
            {   // No message provided for this tag
                Log(LOG_ERRORS, "No message provided for tag: %s\n", messageTag.c_str());
            }
        }
        catch(BepException &e)
        {   // ensure the port is unlocked
            if(portLocked)
            {
                if(m_protocolFilter->UnlockPort() == false)
                    Log(LOG_ERRORS, "\tUnable to unlock the port\n");
                else
                    Log(LOG_DEV_DATA, "\tPort Successfully Unlocked\n");
            }
            throw;
        }

        // unlock the port
        if(portLocked)
        {
            if(m_protocolFilter->UnlockPort() == false)
                Log(LOG_ERRORS, "\tUnable to unlock the port\n");
            else
                Log(LOG_DEV_DATA, "\tPort Successfully Unlocked\n");
        }
    }
    // Return the result of the slow baud init
    Log(LOG_FN_ENTRY, "Exit SendModuleWakeUpMessage\n");
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE DensoAirbagModule<ProtocolFilter>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "DensoAirbagModule::ReadFaults() - Enter");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", response);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    {   // Determine if there are any faults present
        if(response[DtcCountIndex()] > 0)
        {   // Faults are present, parse them out
            UINT8 currentDtc = DtcStartIndex();
            char buff[32];
            string reportedDtc;
            for(UINT8 dtcCount = 0; dtcCount < response[DtcCountIndex()]; dtcCount++)
            {
                reportedDtc = CreateMessage(buff, sizeof(buff), "%02X%02X", response[currentDtc], response[currentDtc+1]);
                Log(LOG_DEV_DATA, "DTC %02d: %s", dtcCount+1, reportedDtc.c_str());
                faultCodes.push_back(reportedDtc);
                currentDtc += BytesPerDtc();
            }
        }
        else
        {   // No faults present, clear the fault vector
            Log(LOG_DEV_DATA, "No faults detected in the module");
            faultCodes.clear();
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read faults from %s\n", ModuleName().c_str());
    }
    // Return the status
    Log(LOG_FN_ENTRY, "DensoAirbagModule::ReadFaults() - Exit");
    return(status);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
bool DensoAirbagModule<ProtocolFilter>::InitializeHook(const XmlNode *configNode)
{
    // Get the Signal high time from the config file
    UINT32 sigTime = 0;
    try
    {
        sigTime = BposReadInt(configNode->getChild("Setup/CommInitData/SignalHighTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Signal high time not defined, using 25ms - %s", excpt.GetReason());
        sigTime = 25;
    }
    SignalHighTime(&sigTime);
    // Get the signal low time from the config file
    sigTime = 0;
    try
    {
        sigTime = BposReadInt(configNode->getChild("Setup/CommInitData/SignalLowTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Signal low time not defined, using 25ms - %s", excpt.GetReason());
        sigTime = 25;
    }
    SignalLowTime(&sigTime);
    // Get the wakeup response delay from the module
    INT32 wakeupDelay = 0;
    try
    {
        wakeupDelay = BposReadInt(configNode->getChild("Setup/CommInitData/WakeUpResponseDelay")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Wake up response delay not found, using 100ms - %s", excpt.GetReason());
        wakeupDelay = 100;
    }
    WakeUpResponseDelay(&wakeupDelay);
    // Get the number of wake up attempts
    UINT8 attempts = 0;
    try
    {
        attempts = BposReadInt(configNode->getChild("Setup/CommInitData/WakeUpAttempts")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Wake up attempts not defined, using 3 - %s", excpt.GetReason());
        attempts = 3;
    }
    WakeUpAttempts(&attempts);
    // Load the DTC data
    LoadDtcData(configNode->getChild("Setup/DtcData"));
    m_keepAliveComm->Initialize(configNode);
    // Call the base class to complete the initialization
    return GenericModuleTemplate<ProtocolFilter>::InitializeHook(configNode);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
UINT32& DensoAirbagModule<ProtocolFilter>::SignalLowTime(UINT32 *lowTime /*= NULL*/)
{
    if(lowTime != NULL)  m_signalLowTime = *lowTime;
    return m_signalLowTime;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
UINT32& DensoAirbagModule<ProtocolFilter>::SignalHighTime(UINT32 *highTime /*= NULL*/)
{
    if(highTime != NULL)  m_signalHighTime = *highTime;
    return m_signalHighTime;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
INT32 DensoAirbagModule<ProtocolFilter>::WakeUpResponseDelay(const INT32 *wakeupDelay /*= NULL*/)
{
    if(wakeupDelay != NULL) m_wakeupResponseDelay = *wakeupDelay;
    return m_wakeupResponseDelay;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
UINT8 DensoAirbagModule<ProtocolFilter>::WakeUpAttempts(const UINT8 *attempts /*= NULL*/)
{
    if(attempts != NULL)  m_wakeupAttempts = *attempts;
    return m_wakeupAttempts;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
void DensoAirbagModule<ProtocolFilter>::LoadDtcData(const XmlNode *dtcData)
{
    UINT8 data = 0x00;
    // Store the index to the number of DTCs returned by the module
    try
    {   // Store the index to the number of DTCs returned by the module
        data = BposReadInt(dtcData->getChild("DtcCountIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC Count Index not defined, using 1: %s", excpt.GetReason());
        data = 1;
    }
    DtcCountIndex(&data);
    // Store the number of bytes for each DTC record
    data = 0;
    try
    {
        data = BposReadInt(dtcData->getChild("BytesPerDtc")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Bytes per DTC not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    BytesPerDtc(&data);
    // Store the starting index of the first DTC
    data = 0;
    try
    {
        data = BposReadInt(dtcData->getChild("DtcStartIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC starting index not defined, using 2: %s", excpt.GetReason());
        data = 2;
    }
    DtcStartIndex(&data);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
const UINT8& DensoAirbagModule<ProtocolFilter>::DtcCountIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcCountIndex = *index;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
const UINT8& DensoAirbagModule<ProtocolFilter>::BytesPerDtc(const UINT8 *numBytes /*= NULL*/)
{
    if(numBytes != NULL)  m_bytesPerDtc = *numBytes;
    return m_bytesPerDtc;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
const UINT8& DensoAirbagModule<ProtocolFilter>::DtcStartIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcStartIndex = *index;
    return m_dtcStartIndex;
}
