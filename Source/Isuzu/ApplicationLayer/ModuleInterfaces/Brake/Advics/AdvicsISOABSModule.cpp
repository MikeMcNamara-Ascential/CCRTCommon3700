//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the Advics ABS.
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
#include "AdvicsISOABSModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
AdvicsISOABSModule<ProtocolFilter>::AdvicsISOABSModule() : GenericABSModuleTemplate<ProtocolFilter>()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
AdvicsISOABSModule<ProtocolFilter>::~AdvicsISOABSModule()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline string AdvicsISOABSModule<ProtocolFilter>::ModuleName()
{
    return "Advics ISO ABS Module";
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::PerformModuleLinkup()
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    INT32 connectStat = -1;
    Log(LOG_FN_ENTRY, "AdvicsISOABS::PerformModuleLinkup() - Enter");
    // Send the low baud init command
    if((connectStat = m_protocolFilter->ResetConnection()) == EOK)
    {
            status = (m_protocolFilter->LowSpeedInit() == EOK) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            // Clear the serial port
            m_protocolFilter->ResetConnection();
            Log(LOG_FN_ENTRY, "AdvicsISOABS::LowSpeedInit status %s", ConvertStatusToResponse(status).c_str());
        if(BEP_STATUS_SUCCESS == status)
        {   // Get the command data from the module and echo back alll bytes
            SerialString_t initMessage;
            status = m_protocolFilter->GetInitialMessage(InitMessageByteCount(), InitMessageEcho(), initMessage);
            // If the initial message was properly recieved, get the module type data
            if(BEP_STATUS_SUCCESS == status)
            {
                string typeData;
                status = ReadPartNumber(typeData);
                Log(LOG_ERRORS, "Module type data - status: %s, data: %s", 
                    ConvertStatusToResponse(status).c_str(), typeData.c_str());
            }
        }
    }
    else
    {   // Could not reset the connection
        Log(LOG_ERRORS, "Failed to reset the connection: %d - %s", connectStat, strerror(connectStat));
        status = BEP_STATUS_HARDWARE;
    }
    Log(LOG_FN_ENTRY, "AdvicsISOABS::PerformModuleLinkup() - Exit: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::ReadPartNumber(string &partNumber)
{
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::ReadPartNumber() - Enter");
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    string partialPartNum;
    // Get the first set of type number data
    for(INT32 index = 0; (index < NumberOfPartNumberReads()) && (BEP_STATUS_SUCCESS == status); index++)
    {
        status = ReadModuleData(m_readPartNumber, partialPartNum);
        Log(LOG_DEV_DATA, "Part number read %d - %s", index+1, ConvertStatusToResponse(status).c_str());
        if(BEP_STATUS_SUCCESS == status)  partNumber += partialPartNum;
    }
    // Get the last portion of the type number data
    /*status = ReadModuleData(m_readPartNumber+"2", partialPartNum);
    Log(LOG_DEV_DATA, "Part number read %d - %s", NumberOfPartNumberReads(), ConvertStatusToResponse(status).c_str());
    if(BEP_STATUS_SUCCESS == status)  partNumber += partialPartNum;*/
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::ReadPartNumber() - Exit - Status: %s, part number: %s", 
        ConvertStatusToResponse(status).c_str(), partNumber.c_str());
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
bool AdvicsISOABSModule<ProtocolFilter>::InitializeHook(const XmlNode *configNode)
{
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::InitializeHook() - Enter");
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
    // Get the number of part number messages
    INT32 number = 0;
    try
    {
        number = BposReadInt(configNode->getChild("Setup/MessageData/NumberOfPartNumberMessages")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Number of part number messages not defined, using 4 - %s", excpt.GetReason());
        number = 4;
    }
    NumberOfPartNumberReads(&number);
    // Load the DTC data
    UINT8 data = 0x00;
    try
    {
        data = BposReadInt(configNode->getChild("Setup/DtcData/BytesPerDtc")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Bytes per DTC not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    BytesPerDtc(&data);
    data = 0;
    try
    {
        data = BposReadInt(configNode->getChild("Setup/DtcData/DtcStartIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC start index not defined, using 3: %s", excpt.GetReason());
        data = 3;
    }
    Log(LOG_DEV_DATA,"Setting DTC start index to %d", data);
    DtcStartIndex(&data);
    data = 0;
    try
    {
        data = BposReadInt(configNode->getChild("Setup/DtcData/DTCCountIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DTC count index not defined, using 4: %s", excpt.GetReason());
        data = 4;
    }
    Log(LOG_DEV_DATA,"Setting DTC count index to %d", data);
    DtcCountIndex(&data);
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

    //Set the low and high times for the module
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
    // Initialize the keep alive communication thread
    m_keepAliveComm->Initialize(configNode);
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::InitializeHook() - Exit");
    return GenericABSModuleTemplate<ProtocolFilter>::InitializeHook(configNode);
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const INT32& AdvicsISOABSModule<ProtocolFilter>::InitMessageByteCount(INT32 *byteCount /* = NULL */)
{
    if(byteCount != NULL)
    {
        m_initMessageByteCount = *byteCount;
    }
    return m_initMessageByteCount;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const UINT8& AdvicsISOABSModule<ProtocolFilter>::InitMessageEcho(UINT8 *echo /* = NULL */)
{
    if(echo != NULL)
    {
        m_initMessageEcho = *echo;
    }
    return m_initMessageEcho;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const INT32& AdvicsISOABSModule<ProtocolFilter>::NumberOfPartNumberReads(const INT32 *number /*= NULL*/)
{
    if(number != NULL)  m_numberOfPartNumberReads = *number;
    return m_numberOfPartNumberReads;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const UINT8& AdvicsISOABSModule<ProtocolFilter>::BytesPerDtc(const UINT8 *bytes /*= NULL*/)
{
    if(bytes != NULL)  m_bytesPerDtc = *bytes;
    return m_bytesPerDtc ;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const UINT8& AdvicsISOABSModule<ProtocolFilter>::DtcStartIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcStartIndex = *index;
    return m_dtcStartIndex;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
const UINT8& AdvicsISOABSModule<ProtocolFilter>::DtcCountIndex(const UINT8 *index /*= NULL*/)
{
    if(index != NULL)  m_dtcCountIndex = *index;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& AdvicsISOABSModule<ProtocolFilter>::SignalLowTime(const UINT32 *lowTime /*= NULL*/)
{
    if(lowTime != NULL)  m_signalLowTime = *lowTime;
    return m_signalLowTime;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& AdvicsISOABSModule<ProtocolFilter>::SignalHighTime(const UINT32 *highTime /*= NULL*/)
{
    if(highTime != NULL)  m_signalHighTime = *highTime;
    return m_signalHighTime;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT32& AdvicsISOABSModule<ProtocolFilter>::WakeUpResponseDelay(const UINT32 *wakeupDelay /*= NULL*/)
{
    if(wakeupDelay != NULL)  m_wakeupResponseDelay = *wakeupDelay;
    return m_wakeupResponseDelay;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const INT32& AdvicsISOABSModule<ProtocolFilter>::WakeUpAttempts(const INT32 *attempts /*= NULL*/)
{
    if(attempts != NULL)  m_wakeupAttempts = *attempts;
    return m_wakeupAttempts;
}
//-----------------------------------------------------------------------------
template<class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::PerformFastInitWakeup(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::PerformFastInitWakeup() - Enter");
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
                Log(LOG_DEV_DATA, "Converting %s message to binary", asciiMessage.c_str());
                m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
                // Add the checksum to the message
                m_protocolFilter->AddChecksumToMessage(xmtMessage);

                INT32 attempt = 0;
                do
                {   // Look for the response
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

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::ABSValveActuation(string methodName, WheelSpeeds_t &speedDeltas) throw(ModuleException)
{	// Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ABSValveActuation(%s)", ModuleName().c_str(), methodName.c_str());
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // The response from the protocol Filter
    SerialString_t response;
    Log(LOG_DEV_DATA, "%s: ABSValveActuation: Checking object status", ModuleName().c_str());
    // Check to see that all our objects are in place
    // This throws a ModuleException if things aren't good
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "%s: ABSValveActuation: Calling GetModuleData", ModuleName().c_str());
    status = m_protocolFilter->GetModuleData(methodName, response);

    if (status == BEP_STATUS_SUCCESS)
    {
       string method2 = methodName + "2";
       status = m_protocolFilter->GetModuleData(method2, response);
    }
    // Ask the protocol filter for the info
    if (status == BEP_STATUS_SUCCESS)
    {
        try
        {
            // Get the reply interpretation from the Xml
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Getting reply interpretation for LF", ModuleName().c_str());
            const XmlNode *replyInterpNodeLF = m_vehicleMessagesNode->getChild(methodName)->getChild("ReplyInterpretationLF");
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Getting reply interpretation for RF", ModuleName().c_str());
            const XmlNode *replyInterpNodeRF = m_vehicleMessagesNode->getChild(methodName)->getChild("ReplyInterpretationRF");
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Getting reply interpretation for LR", ModuleName().c_str());
            const XmlNode *replyInterpNodeLR = m_vehicleMessagesNode->getChild(methodName)->getChild("ReplyInterpretationLR");
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Getting reply interpretation for RR", ModuleName().c_str());
            const XmlNode *replyInterpNodeRR = m_vehicleMessagesNode->getChild(methodName)->getChild("ReplyInterpretationRR");

            // Try to parse the response into something readable
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Parsing LF Response", ModuleName().c_str());
            speedDeltas[LFWHEEL] = ParseFloatResponse(replyInterpNodeLF->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Parsing RF Response", ModuleName().c_str());
            speedDeltas[RFWHEEL] = ParseFloatResponse(replyInterpNodeRF->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Parsing LR Response", ModuleName().c_str());
            speedDeltas[LRWHEEL] = ParseFloatResponse(replyInterpNodeLR->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ABSValveActuation: Parsing RR Response", ModuleName().c_str());
            speedDeltas[RRWHEEL] = ParseFloatResponse(replyInterpNodeRR->getValue(), response);

            // Log the wheel speed sensors read from the module
            Log(LOG_DEV_DATA, "ABSValveActuation: \n\tLF = %g\n\tRF = %g\n\tLR = %g\n\tRR = %g\n",
                speedDeltas[LFWHEEL], speedDeltas[RFWHEEL], speedDeltas[LRWHEEL], speedDeltas[RRWHEEL]);
        }
        catch (XmlException &ex)
        {	// Log it
            Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ABSValveActuation(%s)", ModuleName().c_str(), methodName.c_str());
    // Return the status of the operation
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special implementation, use the base class
    status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, bool &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if("ReadVacuumFillingEndFlag" == methodName) status = ReadModuleData(methodName, value);
	else
    { 
        // No special method available, try the base class
        status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
    }

    // Return the status
    return status;
}
template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, float &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    /*if(methodName == "ReadSupplyVoltage") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadSteeringWheelAngle")  status = ReadModuleData(methodName,value);
    else if(methodName == "ReadLateralAcceleration") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadYawRate") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadPressureSensor") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadEngineSpeed") status = ReadModuleData(methodName,value);
    // No special implementation, try the base class
    else*/
    status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, int &value) throw(ModuleException)
{                                  // No special method available, try using the base class
    return GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    /*if(methodName == "CheckProcessByte") status = ReadModuleData(methodName,value);
    else if(methodName == "CheckEolByte") status = ReadModuleData(methodName,value);
    // No special implementation, use the base class
    else*/
    status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    /*if(methodName == "ReadShiftLeverPosition") status = ReadModuleData(methodName,value);
    // No special implementation, use the base class
    else */
    status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(value.size() != 0)
    {
        value.clear();
    }

    if(methodName == "ReadFaults") status = ReadFaults(value);

    // Return the status
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(method == "ReadSpeedDeltas") status = ABSValveActuation( method, value);
    else if(method == "LFABSReduction") status = ABSValveActuation( method, value);
    else if(method == "LFABSRecovery") status = ABSValveActuation( method, value);
    else if(method == "RFABSReduction") status = ABSValveActuation( method, value);
    else if(method == "RFABSRecovery") status = ABSValveActuation( method, value);
    else if(method == "LRABSReduction") status = ABSValveActuation( method, value);
    else if(method == "LRABSRecovery") status = ABSValveActuation( method, value);
    else if(method == "RRABSReduction") status = ABSValveActuation( method, value);
    else if(method == "RRABSRecovery") status = ABSValveActuation( method, value);
    // No specific method, use the base class
    else status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(method, value);
    
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string method, vector<WheelSpeeds_t> &value)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter AdvicsISOABSModule::GetInfo(%s, vector<WheelSpeeds_t>)\n", method.c_str());

    /*if(method == "StartSensorQualityTest")
    {
        WheelSpeeds_t min;
        WheelSpeeds_t max;

        min[LFWHEEL] = 100.0; min[RFWHEEL] = 100.0; min[LRWHEEL] = 100.0; min[RRWHEEL] = 100.0;
        max[LFWHEEL] = 0.0; max[RFWHEEL] = 0.0; max[LRWHEEL] = 0.0; max[RRWHEEL] = 0.0;
        status = StartSensorQualityTest(min, max);

        value.clear();
        value.push_back(min);
        value.push_back(max);
    }*/

    Log( LOG_FN_ENTRY, "Exit AdvicsISOABSModule::GetInfo(%s, vector<WheelSpeeds_t>), status=%d\n", method.c_str(), status);
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "DisableSpeedLimit") status = CommandModule(methodName);
    else if(methodName == "EnableSpeedLimit") status = CommandModule(methodName);
    else if(methodName == "DisableValveRelayShutdown") status = CommandModule(methodName);
    else if(methodName == "EnableValveRelayShutdown") status = CommandModule(methodName);
    else if(methodName == "ReadRelayState") status = CommandModule(methodName);
    else if(methodName == "ReadPumpMotorState") status = CommandModule(methodName);
    else if (methodName == "UnlockModuleSecurity") status = CommandModule(methodName);
    else if("WriteVacuumFillingEndFlag" == methodName) status = CommandModule(methodName);

    else status = GenericABSModuleTemplate<ProtocolFilter>::GetInfo(methodName);
    
    return( status);
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::GetInfo(string methodName, vector<bool> &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(value.size() != 0)
    {
        value.clear();
    }

    // Read the relay states
    /*if(methodName == "ReadRelayState")
    {
        bool motorRelay;
        bool valveRelay;

        status = ReadRelayState(motorRelay,valveRelay);

        if(status == BEP_STATUS_SUCCESS)
        {
            value.push_back(motorRelay);
            value.push_back(valveRelay);
        }
    }*/
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilterType>::ReadRelayState(bool &motorRelay, bool &valveRelay) throw(ModuleException)
{
    BEP_STATUS_TYPE status;
    SerialString_t response(255,0);    // Serial string we'll get back from the PF

    // Chech if things are in order
    CheckObjectsStatus();

    // Ask the PF
    status = m_protocolFilter->GetModuleData("ReadRelayState", response);

    //MAM 4/17/09 - debug
    int ii;
    Log( LOG_DEV_DATA, "ReadRelayState response = ");
    for (ii=0; ii<40; ii++)
    {
      Log( LOG_DEV_DATA, "%2X ", response[ii]);
    }
    Log( LOG_DEV_DATA, " \n");


    const XmlNode *replyInterpNode;

    if(status == BEP_STATUS_SUCCESS)
    {
        try
        {
            // Get the Motor Relay
            // Get the reply interp node for motor relay
            replyInterpNode = m_vehicleMessagesNode->getChild("ReadRelayState/ReplyInterpretationMotorRelay");
            motorRelay = ParseBooleanResponse( replyInterpNode->getValue(),response );
            // Get the Valve Relay
            // Get the reply interp node for valve relay
            replyInterpNode =
            m_vehicleMessagesNode->getChild("ReadRelayState/ReplyInterpretationValveRelay");
            valveRelay = ParseBooleanResponse( replyInterpNode->getValue(),response );
        }
        catch(XmlException &ex)
        {
            // Return a software failure and log
            Log(LOG_ERRORS,(ModuleName() + ": ReadRelayState: "+ex.GetReason()).c_str());
            return BEP_STATUS_SOFTWARE;
        }
    }

    // Return the status
    return status;
}
//-------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::UnlockModuleSecurity()
{
    SerialArgs_t seed;         // Security seed read from the module
    SerialArgs_t securityKey;  // Security unlock key
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t  moduleResponse;

    // Read the security seed from the module
    if((status = ReadModuleData("ReadSecuritySeed",seed)) == BEP_STATUS_SUCCESS)
    {
        // Read security seed from the module. Check for special conditions
        Log(LOG_DEV_DATA, "Read security seed\n");

        // Check for lock
        bool bLocked = false;
        for(UINT32 iteratorSeed = 0; iteratorSeed < seed.size(); iteratorSeed++)
        {
            UINT8 uSeed = (UINT8) seed[iteratorSeed];   
            if(uSeed != 0x00) bLocked = true;
        }

        if(!bLocked)
        {   // Module already unlocked, no need to do anything else
            Log(LOG_DEV_DATA, "Module Security already unlocked\n");
        }
        else
        {   // Security not unlocked, proceed with algorithm
            Log(LOG_DEV_DATA, "Unlocking module\n");
            // Make the key the same size as the seed
            securityKey.reserve(seed.size());
            securityKey.resize(seed.size());
            if((status = CalculateSecurityKey(seed, securityKey)) == BEP_STATUS_SUCCESS)
            {   // Calculated the security key, unlock the module
                Log(LOG_DEV_DATA, "Calculated security key\n");
                //status = CommandModule("UnlockModuleSecurity", &securityKey);
                status = m_protocolFilter->GetModuleData("UnlockModuleSecurity",moduleResponse,&securityKey);
                Log(LOG_ERRORS, "Security unlock status: %s", ConvertStatusToResponse(status).c_str()); 
            }
            else
            {   // Error calculating the security key
                Log(LOG_ERRORS, "Failed to calculate the security key - status: %s\n",
                    ConvertStatusToResponse(status).c_str());
            }
        }
    }
    else
    {   // Error reading security seed
        Log(LOG_ERRORS, "Failed to read security seed from the module - status: %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

//-------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::CalculateSecurityKey(const SerialArgs_t &seedInput, 
                                                                          SerialArgs_t &keyOutput)
{
    
    UINT16 seed;
    UINT16 key;

    //this will stay - convert serial arguement to uint16
    seed = (seedInput[1] & 0x00FF) + ((seedInput[0] << 8) & 0xFF00);
    Log(LOG_DEV_DATA, "Assembled seed into UINT16 variable: %04X\n", seed);

    //Exclusive OR operations
    key = ((((seed>>4) ^ seed)<<1) ^ seed);
    Log(LOG_DEV_DATA, "Calculated security key: %04X\n", key);
    keyOutput[0] = ((key & 0xFF00) >> 8);
    keyOutput[1] = (key & 0x00FF);

    /*UINT32 seed;
    UINT32 key;

    //this will stay - convert serial argument to uint16
    seed = ((seedInput[0] << 24) & 0xFF000000) + ((seedInput[1] << 16) & 0x00FF0000) + 
        ((seedInput[2] << 8) & 0x0000FF00) + ((seedInput[3]) & 0x000000FF);
    Log(LOG_DEV_DATA, "Assembled seed into UINT32 variable: %08X\n", seed);

    //Exclusive OR operations
    key = ((((seed>>2) ^ seed)<<3) ^ seed);
    Log(LOG_DEV_DATA, "Calculated security key: %08X\n", key);
    keyOutput[0] = ((key & 0xFF000000) >> 24);
    keyOutput[1] = ((key & 0x00FF0000) >> 16);
    keyOutput[2] = ((key & 0x0000FF00) >> 8);
    keyOutput[3] = (key & 0x000000FF);
    Log(LOG_DEV_DATA, "Broke key into SerialArgs_t variable\n");*/
    // return the status
    return(BEP_STATUS_SUCCESS);
}
//-----------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::ReadSensorSpeeds() - Enter");
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
    Log(LOG_FN_ENTRY, "AdvicsISOABSModule::ReadSensorSpeeds() - Exit");
    return status;
}

template <class ProtocolFilter>
BEP_STATUS_TYPE AdvicsISOABSModule<ProtocolFilter>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    uint8_t     dtcCount=0, dtcIdx=0;
    uint16_t    dtcVal;
    char    dtcStr[ 16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ReadFaults", response);

    if (status == BEP_STATUS_SUCCESS)
    {
        //first bytes are header bytes
        if ( response.size() >= DtcStartIndex())
        {
            //no count byte so we erase header bytes and divide remaining # bytes by bytes per dtc
            response.erase( 0, (DtcStartIndex()));
            dtcCount =  response.size()/BytesPerDtc();
        }
        Log( LOG_DEV_DATA, "AdvicsISOABSModule - Module is reporting %d DTC's\n", dtcCount);

        // Extract the DTCs from the respnse
        for ( dtcIdx=0; (dtcIdx<dtcCount) && (response.size()>1); dtcIdx++)
        {
            // DTC are sent as [ HI,LO]
            dtcVal = response[ 0];
            dtcVal = ((dtcVal << 8) & 0xFF00) | response[1];
            Log( LOG_DEV_DATA, "\tDTC %d: $%04X\n", dtcIdx, dtcVal);

            // Erase the bytes for this DTC
            response.erase( 0,3);   
            // Add the DTC to the fault list
            std::string faultStr( CreateMessage(dtcStr, 16, "%04X", dtcVal));
            FaultCode_t fault(faultStr);
            faultCodes.push_back( fault);
        }

        // If we did not receive the full DTC list from the module
        if ( dtcIdx < dtcCount)
        {
            // Log a message
            Log( LOG_ERRORS, "Did not receive full list of DTCs: %d of %d DTCs received\n",
                 dtcIdx, dtcCount);
        }

        return(BEP_STATUS_SUCCESS);
    }
    else
    {
        return(status);
    }
}
