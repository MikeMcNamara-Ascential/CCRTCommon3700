//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//	Module Interface for Bosch 8 ABS controller.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 4     11/15/04 8:52a Bmeinke
// Added a handler for ReadSpeedDeltas inside GetInfo(WheelSpeeds_t)
// Added the new ESP mehod handlers to the generic GetInfo() method
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 3     10/26/04 12:42p Bmeinke
// ALWAYS put 2 WheelSpeed_t entries into the vector inside GetInfo(
// StartSensorQualityTest)
// Inside StartSensorQualityTest(), we do not get a message for "Starting"
// , so don't look for one.
// Inside StartSensorQualityTest(), consider "failed" to meane "not
// passed"
// Parse the module reply inside ReadFaults() the "hard" way (i.e. no
// response interpretaion: too difficult according to Dan E)
// 
// 2     10/15/04 2:02p Bmeinke
// Modified ReadFaults to manually parse the list of faults
// 
// 1     8/30/04 7:23p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
// 1     8/09/04 9:17a Bmeinke
// Removed the RunPumpMotor() method (already provided in
// GenericABSModuleTemplate.h)
// Added Log( FN_ENTRY, ...) calls to EnterDiagnosticMode()
// 
// 1     12/22/03 12:34p Rwiersem
// Initial version.
// 
//*************************************************************************

#include "Bosch8Module.h"

template <class ProtocolFilterType>
Bosch8Module<ProtocolFilterType>::Bosch8Module(void) : KoreaAbsModuleTemplate<ProtocolFilterType>(), 
m_moduleConfig(NULL),m_faultReader(NULL)
{
}

template <class ProtocolFilterType>
Bosch8Module<ProtocolFilterType>::~Bosch8Module()
{                                  // Get rid of the module config
    if(m_moduleConfig != NULL) delete m_moduleConfig;
    m_moduleConfig = NULL;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special implementation, use the base class
    if( methodName == "ReadBrakeSwitch")
    {
        bool    switchOn;

        status = GetInfo( methodName, switchOn);
        if( status == BEP_STATUS_SUCCESS)
        {
            if( switchOn == true)   value = "On";
            else                    value = "Off";
        }
        else
        {
            value = "Unknown";
        }
    }
    else
    {
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    }

    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "ReadPumpStatus") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadParkBrakeSignal") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadEngineSpeed") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadThrottlePosition") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadPassiveSwitchState") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadAYSensorStatus") status = ReadModuleData(methodName,value);
    // No special method available, try the base class
    else status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}
template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "ReadSupplyVoltage") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadSteeringWheelAngle")  status = ReadModuleData(methodName,value);
    else if(methodName == "ReadLateralAcceleration") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadYawRate") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadPressureSensor") status = ReadModuleData(methodName,value);
    else if(methodName == "ReadEngineSpeed") status = ReadModuleData(methodName,value);
    // No special implementation, try the base class
    else  status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{                                  // No special method available, try using the base class
    return KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "CheckProcessByte") status = ReadModuleData(methodName,value);
    // No special implementation, use the base class
    else status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "ReadShiftLeverPosition") status = ReadModuleData(methodName,value);
    // No special implementation, use the base class
    else status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    // Get the communication protocol to determine whether to use a single(CAN) or multiple(J2190) reads
    string commProtocol;
    try
    {   // Get the communications protocol
        commProtocol = m_configNode->getChild("Setup/CommunicationProtocol")->getValue();
    }
    catch(XmlException &ex)
    {   // CommunicationProtocol Tag not found, set to default
        Log(LOG_DEV_DATA, "Communications Protocol tag not found, setting to KeywordProtocol2000");
        commProtocol = "ISOKeywordProtocol2000";
    }
    if(value.size() != 0)
    {
        value.clear();
    }

    if(methodName == "ReadFaults")
        if(commProtocol == "CANKeywordProtocol2000") status = ReadFaultsCAN(value);
        else status = ReadFaults(value);

    // Return the status
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException)
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
    else status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(method, value);

    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string method, vector<WheelSpeeds_t> &value)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter Bosch8Module::GetInfo(%s, vector<WheelSpeeds_t>)\n", method.c_str());

    if(method == "StartSensorQualityTest")
    {
        WheelSpeeds_t min;
        WheelSpeeds_t max;

        min[LFWHEEL] = 100.0; min[RFWHEEL] = 100.0; min[LRWHEEL] = 100.0; min[RRWHEEL] = 100.0;
        max[LFWHEEL] = 0.0; max[RFWHEEL] = 0.0; max[LRWHEEL] = 0.0; max[RRWHEEL] = 0.0;
        status = StartSensorQualityTest(min, max);

        value.clear();
        value.push_back(min);
        value.push_back(max);
    }

    Log( LOG_FN_ENTRY, "Exit Bosch8Module::GetInfo(%s, vector<WheelSpeeds_t>), status=%d\n", method.c_str(), status);
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "DisableSpeedLimit") status = CommandModule(methodName);
    else if(methodName == "DisableValveRelayShutdown") status = CommandModule(methodName);
    else if(methodName == "EnableSpeedLimit") status = CommandModule(methodName);
    else if(methodName == "EnableValveRelayShutdown") status = CommandModule(methodName);
    else if(methodName == "LFESPInit") status = CommandModule(methodName);
    else if(methodName == "LFESPStart") status = CommandModule(methodName);
    else if(methodName == "LFESPEnd") status = CommandModule(methodName);
    else if(methodName == "LFESPFinalize") status = CommandModule(methodName);

    else if(methodName == "RFESPInit") status = CommandModule(methodName);
    else if(methodName == "RFESPStart") status = CommandModule(methodName);
    else if(methodName == "RFESPEnd") status = CommandModule(methodName);

    else if(methodName == "LRESPStart") status = CommandModule(methodName);
    else if(methodName == "LRESPEnd") status = CommandModule(methodName);
    else if(methodName == "LRESPFinalize") status = CommandModule(methodName);

    else if(methodName == "RRESPInit") status = CommandModule(methodName);
    else if(methodName == "RRESPStart") status = CommandModule(methodName);
    else if(methodName == "RRESPEnd") status = CommandModule(methodName);
    else if(methodName == "RRESPFinalize") status = CommandModule(methodName);

    else if(methodName == "ESPPumpOn1") status = CommandModule(methodName);
    else if(methodName == "ESPPrimaryValve1On") status = CommandModule(methodName);
    else if(methodName == "ESPPrimaryValve1Off") status = CommandModule(methodName);
    else if(methodName == "ESPPrimaryValve2On") status = CommandModule(methodName);
    else if(methodName == "ESPPrimaryValve2Off") status = CommandModule(methodName);
    else if(methodName == "ESPPumpOff") status = CommandModule(methodName);
    else if(methodName == "LFABSReduction") status = CommandModule(methodName);
    else if(methodName == "LFABSRecovery") status = CommandModule(methodName);
    else if(methodName == "RFABSReduction") status = CommandModule(methodName);
    else if(methodName == "RFABSRecovery") status = CommandModule(methodName);
    else if(methodName == "LRABSReduction") status = CommandModule(methodName);
    else if(methodName == "LRABSRecovery") status = CommandModule(methodName);
    else if(methodName == "RRABSReduction") status = CommandModule(methodName);
    else if(methodName == "RRABSRecovery") status = CommandModule(methodName);
    else status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, vector<bool> &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(value.size() != 0)
    {
        value.clear();
    }

    // Read the relay states
    if(methodName == "ReadRelayState")
    {
        bool motorRelay;
        bool valveRelay;

        status = ReadRelayState(motorRelay,valveRelay);

        if(status == BEP_STATUS_SUCCESS)
        {
            value.push_back(motorRelay);
            value.push_back(valveRelay);
        }
    }
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::PerformPowerOnReset(void)
{                                  // Command the module to perform the power on reset
    return CommandModule("PerformPowerOnReset");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::EnterDiagnosticMode() throw(ModuleException)
{
    BEP_STATUS_TYPE status;
    string asciiMessage;
    SerialString_t xmtMessage;
    bool initSequenceSuccess = false;
    string commProtocol;
    try
    {   // Get the communications protocol
        commProtocol = m_configNode->getChild("Setup/CommunicationProtocol")->getValue();
    }
    catch(XmlException &ex)
    {   // CommunicationProtocol Tag not found, set to default
        Log(LOG_DEV_DATA, "Communications Protocol tag not found, setting to KeywordProtocol2000");
        commProtocol = "ISOK";
    }
    if(commProtocol == "CANKeywordProtocol2000")
    {
        status = GenericModuleTemplate<ProtocolFilterType>::EnterDiagnosticMode();
    }
    else
    {
        Log( LOG_FN_ENTRY, "Enter Bosch8Module::EnterDiagnosticMode()\n");

        // Check to see that all our objects are in place
        CheckObjectsStatus();

        BposSleep(1000);               // Make sure the module has completed its self test

        status = m_protocolFilter->GetMessage("EnterDiagnosticMode",asciiMessage);

        if(status == BEP_STATUS_SUCCESS)
        {
            // Convert the message to binary
            m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
            // Add the checksum to the message
            m_protocolFilter->AddChecksumToMessage(xmtMessage);

            INT32 attempt = 0;
            do
            {                      // Look for the response
                // Assert a break so the line will be driven low for the required amount of time
                if(m_protocolFilter->EstablishComms(25,25,xmtMessage.c_str(),xmtMessage.length()) == EOK)
                {
                    printf("EnterDiagnosticMode message sent to module\n");

                    SerialString_t moduleResponse;
                    errno = EOK;
                    if(BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse("EnterDiagnosticMode",moduleResponse))
                    {
                        initSequenceSuccess = true;
                    }
                    else
                    {
                        printf("Waiting for module response: attempt: %d\n", attempt);
                        BposSleep(250);
                    }
                }
                else status = BEP_STATUS_FAILURE;
            } while(!initSequenceSuccess && (attempt++ < 100));

            if(initSequenceSuccess)
            {
                status = m_protocolFilter->GetModuleData( "StartDiagnosticSession", xmtMessage);
                if( status != BEP_STATUS_SUCCESS)
                {
                    Log( LOG_ERRORS, "Error starting diagnostic session\n");
                }
            }
            else status = BEP_STATUS_FAILURE;
        }
        else status = BEP_STATUS_FAILURE;

        Log( LOG_FN_ENTRY, "Exit Bosch8Module::EnterDiagnosticMode(), status=%d\n", status);

    }
    return status;
}

template <class ProtocolFilterType>
bool Bosch8Module<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    // We need a keep alive, so initialize the keep alice protocol filter
    if( m_keepAliveComm != NULL)
    {
        Log( LOG_DEV_DATA, "Bosch8Module::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize( configNode);
    }
    // Save the config node for later use
    m_moduleConfig = configNode->Copy();
    // Call the base classes to complete initialization
    return KoreaAbsModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ABSValveActuation(string methodName, WheelSpeeds_t &speedDeltas) throw(ModuleException)
{   // Log the function entry
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
    // Ask the protocol filter for the info
    if(status == BEP_STATUS_SUCCESS)
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
        catch(XmlException &ex)
        {   // Log it
            Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());

            /*
                !!! THIS IS JUST FOR TESTING THE HBEP ABS COMMANDS !!!!!
            */
#if 0
            // set status to a software error
            status = BEP_STATUS_SOFTWARE;
#endif
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ABSValveActuation(%s)", ModuleName().c_str(), methodName.c_str());
    // Return the status of the operation
    return status;
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ReadRelayState(bool &motorRelay, bool &valveRelay) throw(ModuleException)
{
    BEP_STATUS_TYPE status;
    SerialString_t response(255,0);    // Serial string we'll get back from the PF

    // Chech if things are in order
    CheckObjectsStatus();

    // Ask the PF
    status = m_protocolFilter->GetModuleData("ReadRelayState", response);

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

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::StartSensorQualityTest(WheelSpeeds_t &min,WheelSpeeds_t &max) throw(ModuleException)
{
    BEP_STATUS_TYPE status;
    bool passed = false;
    SerialString_t response(255,0);    // The response from the protocol Filter

    Log( LOG_FN_ENTRY, "Enter Bosch8Module::StartSensorQualityTest()\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("StartSensorQualityTest",response);

    if(status == BEP_STATUS_SUCCESS)
    {
        try
        {
            // The reply interpretation that tells us if it's a pass
            const XmlNode *replyInterpNodePass = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationPass");

            // Get the reply interpretation from the Xml for Min Speeds
            Log(LOG_DEV_DATA, "Retrieving LFMin reply interpretation\n");
            const XmlNode *replyInterpNodeLFMin = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationLFMin");
            Log(LOG_DEV_DATA, "Retrieving RFMin reply interpretation\n");
            const XmlNode *replyInterpNodeRFMin = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationRFMin");
            Log(LOG_DEV_DATA, "Retrieving LRMin reply interpretation\n");
            const XmlNode *replyInterpNodeLRMin = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationLRMin");
            Log(LOG_DEV_DATA, "Retrieving RRMin reply interpretation\n");
            const XmlNode *replyInterpNodeRRMin = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationRRMin");

            // Get the reply interpretation from the Xml for Max Speeds
            Log(LOG_DEV_DATA, "Retrieving LFMax reply interpretation\n");
            const XmlNode *replyInterpNodeLFMax = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationLFMax");
            Log(LOG_DEV_DATA, "Retrieving RFMax reply interpretation\n");
            const XmlNode *replyInterpNodeRFMax = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationRFMax");
            Log(LOG_DEV_DATA, "Retrieving LRMax reply interpretation\n");
            const XmlNode *replyInterpNodeLRMax = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationLRMax");
            Log(LOG_DEV_DATA, "Retrieving RRMax reply interpretation\n");
            const XmlNode *replyInterpNodeRRMax = 
            m_vehicleMessagesNode->getChild("StartSensorQualityTest/ReplyInterpretationRRMax");

            Log(LOG_DEV_DATA, "Parsing <PASSED> byte\n");
            passed = ParseBooleanResponse(replyInterpNodePass->getValue(),response);
            if(passed)    // sensor quality test passed
            {
                // Try and parse the response into something readable
                Log(LOG_DEV_DATA, "Parsing <LFMin> byte\n");
                min[LFWHEEL] = ParseFloatResponse(replyInterpNodeLFMin->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <RFMin> byte\n");
                min[RFWHEEL] = ParseFloatResponse(replyInterpNodeRFMin->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <LRMin> byte\n");
                min[LRWHEEL] = ParseFloatResponse(replyInterpNodeLRMin->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <RRMin> byte\n");
                min[RRWHEEL] = ParseFloatResponse(replyInterpNodeRRMin->getValue(),response);

                Log( LOG_DEV_DATA, "SensorQuality min speeds: %.2f %.2f %.2f %.2f\n", 
                     min[LFWHEEL], min[RFWHEEL], min[LRWHEEL], min[RRWHEEL]);

                Log(LOG_DEV_DATA, "Parsing <LFMax> byte\n");
                max[LFWHEEL] = ParseFloatResponse(replyInterpNodeLFMax->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <RFMax> byte\n");
                max[RFWHEEL] = ParseFloatResponse(replyInterpNodeRFMax->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <LRMax> byte\n");
                max[LRWHEEL] = ParseFloatResponse(replyInterpNodeLRMax->getValue(),response);
                Log(LOG_DEV_DATA, "Parsing <RRMax> byte\n");
                max[RRWHEEL] = ParseFloatResponse(replyInterpNodeRRMax->getValue(),response);

                Log( LOG_DEV_DATA, "SensorQuality max speeds: %.2f %.2f %.2f %.2f\n", 
                     max[LFWHEEL], max[RFWHEEL], max[LRWHEEL], max[RRWHEEL]);

                status = BEP_STATUS_SUCCESS;
            }
            else
            {
                Log(LOG_ERRORS, "Sensor quality test failed\n");
                status = BEP_STATUS_FAILURE;
            }
        }
        catch(XmlException &ex)
        {
            Log(LOG_ERRORS,(ModuleName()+": XmlError: "+ex.GetReason()).c_str());
            status = BEP_STATUS_SOFTWARE;
        }
    }
    Log( LOG_FN_ENTRY, "Exit Bosch8Module::StartSensorQualityTest(), status=%d\n", status);

    return status;  
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    uint8_t     dtcCount=0, dtcIdx=0;
    uint16_t    dtcVal;
    char    dtcStr[ 16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ReadFaults", response);

    if(status == BEP_STATUS_SUCCESS)
    {
        // The first byte after the ID is a DTC count
        if( response.size() >= 2)
        {
            // Erase the first byte
            response.erase( 0, 1);
            // Read the number of DTC's to follow
            dtcCount = response[ 0];
        }
        Log( LOG_DEV_DATA, "Module is reporting %d DTC's\n", dtcCount);

        // Extract the DTCs from the respnse
        for( dtcIdx=0; (dtcIdx<dtcCount) && (response.size()>1); dtcIdx++)
        {
            // DTC are sent as [ HI,LO]
            dtcVal = response[ 0];
            dtcVal = ((dtcVal << 8) & 0xFF00) | response[1];
            Log( LOG_DEV_DATA, "\tDTC %d: $%04X\n", dtcIdx, dtcVal);
            // Erase the bytes for this DTC
            response.erase( 0,2);

            // Add the DTC to the fault list
            std::string faultStr( CreateMessage(dtcStr, 16, "%04X", dtcVal));
            FaultCode_t fault(faultStr);
            faultCodes.push_back( fault);
        }

        // If we did not receive the full DTC list from the module
        if( dtcIdx < dtcCount)
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
template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ReadFaultsCAN(FaultVector_t &faultCodes)
{   // Check if a fault reader has been created
    if(m_faultReader == NULL)
    {   // Create a new fault reader
        m_faultReader = new GenericCANModule(m_protocolFilter);
        // Initialize the fault reader object
        m_faultReader->Initialize(m_configNode);
    }
    // Return the result of the fault reader fault read
    return m_faultReader->ReadFaults(faultCodes);
}

