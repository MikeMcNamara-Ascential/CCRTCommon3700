//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the Mando ABS.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
// 
//*************************************************************************
#include "MandoAbsModule.h"

//-----------------------------------------------------------------------------
template <class ProtocolType>
MandoAbsModule<ProtocolType>::MandoAbsModule() : GenericABSModuleTemplate<ProtocolType>()
{   // Nothing to do
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
MandoAbsModule<ProtocolType>::~MandoAbsModule()
{   // Nothing to do
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
inline string MandoAbsModule<ProtocolType>::ModuleName()
{
    return "Mando ABS Module";
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadFaults() - Enter");
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
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadFaults() - Exit");
    return(status);
}

template<class ProtocolFilter>
BEP_STATUS_TYPE MandoAbsModule<ProtocolFilter>::PerformFastInitWakeup(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "MandoAbsModule::PerformFastInitWakeup() - Enter");
    // Lock the port
    bool portLocked = true; //m_protocolFilter->Lockport(0);
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
                Log(LOG_DEV_DATA, "Converting %s message to binary", asciiMessage.c_str());
                m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
                // Add the checksum to the message
                m_protocolFilter->AddChecksumToMessage(xmtMessage);

                INT32 attempt = 0;
                do
                { 
                    m_protocolFilter->ResetConnection();
                    // Look for the response
                    // Assert a break so the line will be driven low for the required amount of time
                    Log(LOG_DEV_DATA, "Asserting break lowtime:%ims hightime:%ims", 
                        SignalLowTime(), SignalHighTime());
                    int iCommError = m_protocolFilter->EstablishComms(SignalLowTime(), 
                                                                      SignalHighTime(), 
                                                                      xmtMessage.c_str(), 
                                                                      xmtMessage.length());
                    if(iCommError == EOK)
                    {   // Get the module response -- could take up to 500ms
                        Log(LOG_DEV_DATA, "Wake up delay %ims", WakeUpResponseDelay());
                        BposSleep(WakeUpResponseDelay());
                        SerialString_t moduleResponse;
                        errno = EOK;
                        Log(LOG_DEV_DATA, "Getting %s message response", messageTag.c_str());
                        if(BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse(messageTag, moduleResponse))
                        {
                            initSequenceSuccess = true;
                            status = BEP_STATUS_SUCCESS;
                        }
                        else
                        {
                            Log(LOG_DEV_DATA, "Waiting for module response: attempt: %d sleeping:%dms", 
                                attempt, WakeUpResponseDelay());
                            BposSleep(WakeUpResponseDelay());
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Error asserting break: %s", strerror(iCommError));
                    }
                } while(!initSequenceSuccess && (attempt++ < WakeUpAttempts()));
                if(initSequenceSuccess)
                {   
                    Log(LOG_DEV_DATA, "SENDING START DIAGNOSTIC SESSION");
                    string fup;
                    status = CommandModule( "StartDiagnosticSession");
                    if( status == BEP_STATUS_SUCCESS)
                    {
                        Log( LOG_ERRORS, "Error starting diagnostic session\n");
                    }
                }
            else status = BEP_STATUS_FAILURE;
            }
            else
            {   // No message provided for this tag
                Log(LOG_ERRORS, "No message provided for tag: %s", messageTag.c_str());
            }
        }
        catch(BepException &e)
        {   // ensure the port is unlocked
            if(portLocked)
            {
                if(m_protocolFilter->UnlockPort() == false)
                    Log(LOG_ERRORS, "\tUnable to unlock the port");
                else
                    Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
            }
            throw;
        }

        // unlock the port
        if(portLocked)
        {
            if(m_protocolFilter->UnlockPort() == false)
                Log(LOG_ERRORS, "\tUnable to unlock the port");
            else
                Log(LOG_DEV_DATA, "\tPort Successfully Unlocked");
        }
    }
    
    // Return the result of the slow baud init
    Log(LOG_FN_ENTRY, "Exit PerformFastInitWakeup");
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::CheckEspEquipped(bool &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule CheckEspEquipped");

	// Check to see that all our objects are in place
	CheckObjectsStatus();
    
    status = ReadModuleData("ReadEspEquipped", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadIoSignals(SerialString_t &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule ReadIoSignals");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

	status = m_protocolFilter->GetModuleData("ReadIoSignals", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadEolTestByte(SerialString_t &response) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule ReadEolTestByte");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

	status = m_protocolFilter->GetModuleData("ReadEolTestByte", response);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::WriteEolTestByte(UINT16 &testByte) throw(ModuleException)
{
	BEP_STATUS_TYPE status;

    // Log the function entry
	Log(LOG_FN_ENTRY, "Enter MandoAbsModule WriteEolTestByte");

	// Check to see that all our objects are in place
	CheckObjectsStatus();

    SerialString_t response;
    SerialArgs_t args;

    args.clear();
    args.push_back(testByte);
    status = m_protocolFilter->GetModuleData("WriteEolTestByte", response, &args);

	return( status);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadSensorSpeeds() - Enter");
    string wheels[] = {"ReadLfSpeed", "ReadRfSpeed", "ReadLrSpeed", "ReadRrSpeed"};
    UINT8 index = 0;
    float wheelSpeed = 0.0;
    do
    {   // Read the current sensor speed from the module
        status = ReadModuleData(wheels[index], wheelSpeed);
        if(BEP_STATUS_SUCCESS == status) 
        {
            Log(LOG_DEV_DATA, "ReadSensorSpeeds - %s: %.2f", wheels[index].c_str(), wheelSpeed);
            speeds[index] = wheelSpeed;
        }
        else
        {
            Log(LOG_ERRORS, "ReadSensorSpeeds - %s: Could not read sensor speed - %s", 
                wheels[index].c_str(), ConvertStatusToResponse(status).c_str());
            speeds[index] = 0.0;
        }
        index++;
    } while((index < 4) && (BEP_STATUS_SUCCESS == status));
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "MandoAbsModule::ReadSensorSpeeds() - Exit");
    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::EspRecovery(const string &valve)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule(valve+"Recovery");
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
BEP_STATUS_TYPE MandoAbsModule<ProtocolType>::EspReduction(const string &corner)  throw(ModuleException)
{   // Command the module to perform reduction on specified wheel
    return CommandModule("EspReduction");
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
bool MandoAbsModule<ProtocolType>::InitializeHook(const XmlNode *configNode)
{
    // We need a keep alive, so initialize the keep alive protocol filter
    if( m_keepAliveComm != NULL)
    {
        Log( LOG_DEV_DATA, "MandoAbsModule::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize( configNode);
    }
    // Load the DTC data
    LoadDtcData(configNode->getChild("Setup/DtcData"));

    INT32 byteCount = -1;
    try
    {
        byteCount = BposReadInt(configNode->getChild("Setup/MessageData/InitializeMessageByteCount")->getValue().c_str());
    }
    catch (XmlException &excp)
    {
        Log(LOG_ERRORS, "No byte count defined! %s", excp.GetReason());
        byteCount = 0;
    }
    InitMessageByteCount(&byteCount);

    UINT8 echo = 0x00;
    try
    {
        echo = BposReadInt(configNode->getChild("Setup/MessageData/InitializeMessageEcho")->getValue().c_str());
    }
    catch (XmlException &excp)
    {
        Log(LOG_ERRORS, "No echo defined! %s", excp.GetReason());
        echo = 0;
    }
    InitMessageEcho(&echo);

    UINT32 lowTime = 0;
    try
    {
        lowTime = BposReadInt(configNode->getChild("Setup/CommInitData/SignalLowTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Low Time not defined, using 24 - %s", excpt.GetReason());
        lowTime = 24;
    }
    SignalLowTime(&lowTime);
    UINT32 highTime = 0;
    try
    {
        highTime = BposReadInt(configNode->getChild("Setup/CommInitData/SignalHighTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "High Time not defined, using 24 - %s", excpt.GetReason());
        highTime = 24;
    }
    SignalHighTime(&highTime);





    UINT32 wakeupDelay = 0;
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
    INT32 attempts = 0;
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
   

    // Call the base class to complete the initialization
    return GenericABSModuleTemplate<ProtocolType>::InitializeHook(configNode);
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
void MandoAbsModule<ProtocolType>::LoadDtcData(const XmlNode *dtcData)
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
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::DtcCountIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcCountIndex = *index;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::BytesPerDtc(const UINT8 *numBytes /*= NULL*/)
{
    if(numBytes != NULL)  m_bytesPerDtc = *numBytes;
    return m_bytesPerDtc;
}

//-----------------------------------------------------------------------------
template <class ProtocolType>
const UINT8& MandoAbsModule<ProtocolType>::DtcStartIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcStartIndex = *index;
    return m_dtcStartIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& MandoAbsModule<ProtocolFilter>::SignalLowTime(const UINT32 *lowTime /*= NULL*/)
{
    if(lowTime != NULL)  m_signalLowTime = *lowTime;
    return m_signalLowTime;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& MandoAbsModule<ProtocolFilter>::SignalHighTime(const UINT32 *highTime /*= NULL*/)
{
    if(highTime != NULL)  m_signalHighTime = *highTime;
    return m_signalHighTime;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& MandoAbsModule<ProtocolFilter>::WakeUpResponseDelay(const UINT32 *wakeupDelay /*= NULL*/)
{
    if(wakeupDelay != NULL)  m_wakeupResponseDelay = *wakeupDelay;
    return m_wakeupResponseDelay;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const INT32& MandoAbsModule<ProtocolFilter>::WakeUpAttempts(const INT32 *attempts /*= NULL/*/)
{
    if(attempts != NULL)  m_wakeupAttempts = *attempts;
    return m_wakeupAttempts;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const INT32& MandoAbsModule<ProtocolFilter>::InitMessageByteCount(INT32 *byteCount /* = NULL /*/)
{
    if(byteCount != NULL)
    {
        m_initMessageByteCount = *byteCount;
    }
    return m_initMessageByteCount;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const UINT8& MandoAbsModule<ProtocolFilter>::InitMessageEcho(UINT8 *echo /* = NULL */)
{
    if(echo != NULL)
    {
        m_initMessageEcho = *echo;
    }
    return m_initMessageEcho;
}
