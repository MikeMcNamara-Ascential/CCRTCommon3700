//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//  Module Interface for Bosch 8 ABS controller.
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

#include "../Include/Bosch8Module.h"

template<class ProtocolFilterType>
Bosch8Module<ProtocolFilterType>::Bosch8Module(void) : KoreaAbsModuleTemplate<ProtocolFilterType>(),
    m_moduleConfig(NULL) {
}

template<class ProtocolFilterType>
Bosch8Module<ProtocolFilterType>::~Bosch8Module() {                                  // Get rid of the module config
    if (m_moduleConfig != NULL)
        delete m_moduleConfig;
    m_moduleConfig = NULL;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special implementation, use the base class
    if (methodName == "ReadBrakeSwitch")
    {
        bool    switchOn;

        status = GetInfo(methodName, switchOn);
        if (status == BEP_STATUS_SUCCESS)
        {
            if (switchOn == true)
                value = "On";
            else
                value = "Off";
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

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, bool &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "ReadPumpStatus")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadParkBrakeSignal")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadEngineSpeed")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadThrottlePosition")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadPassiveSwitchState")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadAYSensorStatus")
        status = ReadModuleData(methodName, value);
    // No special method available, try the base class
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}
template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "ReadSupplyVoltage")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadSteeringWheelAngle")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadLateralAcceleration")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadYawRate")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadPressureSensor")
        status = ReadModuleData(methodName, value);
    else if (methodName == "ReadEngineSpeed")
        status = ReadModuleData(methodName, value);
    // No special implementation, try the base class
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException) {                                  // No special method available, try using the base class
    return KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "CheckProcessByte")
        status = ReadModuleData(methodName, value);
    // No special implementation, use the base class
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "ReadShiftLeverPosition")
        status = ReadModuleData(methodName, value);
    // No special implementation, use the base class
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (value.size() != 0)
    {
        value.clear();
    }

    if (methodName == "ReadFaults")
        status = ReadFaults(value);
    else if (methodName == "UDSReadFaults")
        status = UDSReadFaults(value);

    // Return the status
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (method == "ReadSpeedDeltas")
        status = ABSValveActuation(method, value);
    else if (method == "LFABSReduction")
        status = ABSValveActuation(method, value);
    else if (method == "LFABSRecovery")
        status = ABSValveActuation(method, value);
    else if (method == "RFABSReduction")
        status = ABSValveActuation(method, value);
    else if (method == "RFABSRecovery")
        status = ABSValveActuation(method, value);
    else if (method == "LRABSReduction")
        status = ABSValveActuation(method, value);
    else if (method == "LRABSRecovery")
        status = ABSValveActuation(method, value);
    else if (method == "RRABSReduction")
        status = ABSValveActuation(method, value);
    else if (method == "RRABSRecovery")
        status = ABSValveActuation(method, value);
    // No specific method, use the base class
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(method, value);

    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string method, vector<WheelSpeeds_t> &value)  throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Bosch8Module::GetInfo(%s, vector<WheelSpeeds_t>)\n", method.c_str());

    if (method == "StartSensorQualityTest")
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

    Log(LOG_FN_ENTRY, "Exit Bosch8Module::GetInfo(%s, vector<WheelSpeeds_t>), status=%d\n", method.c_str(), status);
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (methodName == "DisableSpeedLimit")
        status = CommandModule(methodName);
    else if (methodName == "DisableValveRelayShutdown")
        status = CommandModule(methodName);
    else if (methodName == "EnableSpeedLimit")
        status = CommandModule(methodName);
    else if (methodName == "EnableValveRelayShutdown")
        status = CommandModule(methodName);
    else if (methodName == "WriteVariantCode")
        status = CommandModule(methodName);
    else if (methodName == "LFESPInit")
        status = CommandModule(methodName);
    else if (methodName == "LFESPStart")
        status = CommandModule(methodName);
    else if (methodName == "LFESPEnd")
        status = CommandModule(methodName);
    else if (methodName == "LFESPFinalize")
        status = CommandModule(methodName);

    else if (methodName == "RFESPInit")
        status = CommandModule(methodName);
    else if (methodName == "RFESPStart")
        status = CommandModule(methodName);
    else if (methodName == "RFESPEnd")
        status = CommandModule(methodName);
    else if (methodName == "RFESPFinalize")
        status = CommandModule(methodName);

    else if (methodName == "LRESPInit")
        status = CommandModule(methodName);
    else if (methodName == "LRESPStart")
        status = CommandModule(methodName);
    else if (methodName == "LRESPEnd")
        status = CommandModule(methodName);
    else if (methodName == "LRESPFinalize")
        status = CommandModule(methodName);

    else if (methodName == "RRESPInit")
        status = CommandModule(methodName);
    else if (methodName == "RRESPStart")
        status = CommandModule(methodName);
    else if (methodName == "RRESPEnd")
        status = CommandModule(methodName);
    else if (methodName == "RRESPFinalize")
        status = CommandModule(methodName);

    else if (methodName == "ESPPumpOn1")
        status = CommandModule(methodName);
    else if (methodName == "ESPPrimaryValve1On")
        status = CommandModule(methodName);
    else if (methodName == "ESPPrimaryValve1Off")
        status = CommandModule(methodName);
    else if (methodName == "ESPPrimaryValve2On")
        status = CommandModule(methodName);
    else if (methodName == "ESPPrimaryValve2Off")
        status = CommandModule(methodName);
    else if (methodName == "ESPPumpOff")
        status = CommandModule(methodName);
    else if (methodName == "LFABSReduction")
        status = CommandModule(methodName);
    else if (methodName == "LFABSRecovery")
        status = CommandModule(methodName);
    else if (methodName == "RFABSReduction")
        status = CommandModule(methodName);
    else if (methodName == "RFABSRecovery")
        status = CommandModule(methodName);
    else if (methodName == "LRABSReduction")
        status = CommandModule(methodName);
    else if (methodName == "LRABSRecovery")
        status = CommandModule(methodName);
    else if (methodName == "RRABSReduction")
        status = CommandModule(methodName);
    else if (methodName == "RRABSRecovery")
        status = CommandModule(methodName);
    else
        status = KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(methodName);

    return (status);
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::GetInfo(string methodName, vector<bool> &value) throw(ModuleException) {
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if (value.size() != 0)
    {
        value.clear();
    }

    // Read the relay states
    if (methodName == "ReadRelayState")
    {
        bool motorRelay;
        bool valveRelay;

        status = ReadRelayState(motorRelay, valveRelay);

        if (status == BEP_STATUS_SUCCESS)
        {
            value.push_back(motorRelay);
            value.push_back(valveRelay);
        }
    }
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::PerformPowerOnReset(void) {                                  // Command the module to perform the power on reset
    return CommandModule("PerformPowerOnReset");
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::EnterDiagnosticMode() throw(ModuleException) {
    BEP_STATUS_TYPE status;
    string asciiMessage;
    SerialString_t xmtMessage;
    bool initSequenceSuccess = false;
    string commProtocol;
    try
    {   // Get the communications protocol
        commProtocol = m_configNode->getChild("Setup/CommunicationProtocol")->getValue();
    } catch (XmlException &ex)
    {   // CommunicationProtocol Tag not found, set to default
        Log(LOG_DEV_DATA, "Communications Protocol tag not found, setting to KeywordProtocol2000");
        commProtocol = "ISOK";
    }
    if (commProtocol == "CANKeywordProtocol2000")
    {
        status = GenericModuleTemplate<ProtocolFilterType>::EnterDiagnosticMode();
    }
    else
    {
        Log(LOG_FN_ENTRY, "Enter Bosch8Module::EnterDiagnosticMode()\n");

        // Check to see that all our objects are in place
        CheckObjectsStatus();

        BposSleep(1000);               // Make sure the module has completed its self test

        status = m_protocolFilter->GetMessage("EnterDiagnosticMode", asciiMessage);

        if (status == BEP_STATUS_SUCCESS)
        {
            // Convert the message to binary
            m_protocolFilter->GetBinaryMssg(asciiMessage, xmtMessage);
            // Add the checksum to the message
            m_protocolFilter->AddChecksumToMessage(xmtMessage);

            INT32 attempt = 0;
            do
            {                      // Look for the response
                                   // Assert a break so the line will be driven low for the required amount of time
                if (m_protocolFilter->EstablishComms(25, 25, xmtMessage.c_str(), xmtMessage.length()) == EOK)
                {
                    printf("EnterDiagnosticMode message sent to module\n");

                    SerialString_t moduleResponse;
                    errno = EOK;
                    if (BEP_STATUS_SUCCESS == m_protocolFilter->GetResponse("EnterDiagnosticMode", moduleResponse))
                    {
                        initSequenceSuccess = true;
                    }
                    else
                    {
                        printf("Waiting for module response: attempt: %d\n", attempt);
                        BposSleep(250);
                    }
                }
                else
                    status = BEP_STATUS_FAILURE;
            } while (!initSequenceSuccess && (attempt++ < 100));

            if (initSequenceSuccess)
            {
                status = m_protocolFilter->GetModuleData("StartDiagnosticSession", xmtMessage);
                if (status != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "Error starting diagnostic session\n");
                }
            }
            else
                status = BEP_STATUS_FAILURE;
        }
        else
            status = BEP_STATUS_FAILURE;

        Log(LOG_FN_ENTRY, "Exit Bosch8Module::EnterDiagnosticMode(), status=%d\n", status);

    }
    return status;
}

template<class ProtocolFilterType>
bool Bosch8Module<ProtocolFilterType>::InitializeHook(const XmlNode *configNode) {
    Log(LOG_FN_ENTRY, "Enter Bosch8Module::InitializeHook()\n");
    // We need a keep alive, so initialize the keep alice protocol filter
    if (m_keepAliveComm != NULL)
    {
        Log(LOG_DEV_DATA, "Bosch8Module::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize(configNode);
    }
    // Get the DTC format (UDS style DTCs or not)
    try
    {
        SetDTCFormatUDS(atob(configNode->getChild("Setup/DTCData/DTCFormatUDS")->getValue().c_str()));
        Log(LOG_DEV_DATA, "Using UDS format for DTCs: %s\n", GetDTCFormatUDS() ? "TRUE" : "FALSE");
    } catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException getting DTCFormatUDS, defaulting to FALSE: %s\n", err.Reason().c_str());
        SetDTCFormatUDS(false);
    }
    // Store the DTC count index
    try
    {   // Get the DTC count index
        SetDTCCountIndex(BposReadInt(configNode->getChild("Setup/DTCData/DTCCountIndex")->getValue().c_str()));
    } catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException getting DTC count index, using 2: %s\n", err.Reason().c_str());
        SetDTCCountIndex(2);
    }
    // Store the Number of bytes per DTC
    try
    {
        SetBytesPerDTC(BposReadInt(configNode->getChild("Setup/DTCData/BytesPerDTC")->getValue().c_str()));
    } catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException getting number of bytes per DTC field, using 3: %s\n", err.Reason().c_str());
        SetBytesPerDTC(3);
    }
    // Store the index of the first DTC byte
    try
    {
        SetFirstDTCByteIndex(BposReadInt(configNode->getChild("Setup/DTCData/FirstDTCByteIndex")->getValue().c_str()));
    } catch (XmlException &err)
    {
        Log(LOG_ERRORS, "XmlException getting index of the first DTC field, using 4: %s\n", err.Reason().c_str());
        SetFirstDTCByteIndex(4);
    }
    // Load the FaultRegisters used to determine which byte of the module readfault response corresponds to what set
    //      of register codes
    try
    {
        m_faultByteCodes.DeepCopy(configNode->getChild("Setup/FaultByteCodes")->getChildren());
    } catch (XmlException &err)
    {   // Error loading the Fault Registers
        Log(LOG_ERRORS, "No Fault byte codes to load - %s\n", err.what());
    }
    // Load the first byte address of the fault registers
    try
    {
        SetFirstFaultRegister(atoh(configNode->getChild("Setup/DTCData/FirstFaultRegister")->getValue().c_str()));
    } catch (XmlException &err)
    {   // Error loading the First Fault Register address
        Log(LOG_ERRORS, "No Fault Register address to load - %s\n", err.what());
    }
    // Load the number of fault registers
    try
    {
        SetNumberOfFaultRegisters(atoi(configNode->getChild("Setup/DTCData/NumberOfFaultRegisters")->getValue().c_str()));
    } catch (XmlException &err)
    {   // Error loading the number of fault registers
        Log(LOG_ERRORS, "No Number of Fault Registers to load - %s\n", err.what());
    }
    // Load the option to stop read if first data byte returned is zero
    try
    {
        SetStopNoDTCs(configNode->getChild("Setup/DTCData/StopDTCReadOnZero")->getValue());
    } catch (XmlException &err)
    {   // Error loading the number of fault registers
        Log(LOG_ERRORS, "No stop DTC read option to load - %s\n", err.what());
    }
    try
    {
        m_securityUnlockType = (configNode->getChild("Setup/SecurityUnlockType")->getValue());
    } catch (XmlException &err)
    {   // Error loading the number of fault registers
        Log(LOG_ERRORS, "No Security Unlock Type defined - %s\n", err.what());
        m_securityUnlockType = "Generic";
    }
    // Save the config node for later use
    m_moduleConfig = configNode->Copy();
    // Call the base classes to complete initialization
    return KoreaAbsModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ABSValveActuation(string methodName, WheelSpeeds_t &speedDeltas) throw(ModuleException) {   // Log the function entry
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
        } catch (XmlException &ex)
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


template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ReadRelayState(bool &motorRelay, bool &valveRelay) throw(ModuleException) {
    BEP_STATUS_TYPE status;
    SerialString_t response(255, 0);    // Serial string we'll get back from the PF

    // Chech if things are in order
    CheckObjectsStatus();

    // Ask the PF
    status = m_protocolFilter->GetModuleData("ReadRelayState", response);

    const XmlNode *replyInterpNode;

    if (status == BEP_STATUS_SUCCESS)
    {
        try
        {
            // Get the Motor Relay
            // Get the reply interp node for motor relay
            replyInterpNode = m_vehicleMessagesNode->getChild("ReadRelayState/ReplyInterpretationMotorRelay");
            motorRelay = ParseBooleanResponse(replyInterpNode->getValue(), response);
            // Get the Valve Relay
            // Get the reply interp node for valve relay
            replyInterpNode =
                m_vehicleMessagesNode->getChild("ReadRelayState/ReplyInterpretationValveRelay");
            valveRelay = ParseBooleanResponse(replyInterpNode->getValue(), response);
        } catch (XmlException &ex)
        {
            // Return a software failure and log
            Log(LOG_ERRORS, (ModuleName() + ": ReadRelayState: " + ex.GetReason()).c_str());
            return BEP_STATUS_SOFTWARE;
        }
    }

    // Return the status
    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::StartSensorQualityTest(WheelSpeeds_t &min, WheelSpeeds_t &max) throw(ModuleException) {
    BEP_STATUS_TYPE status;
    bool passed = false;
    SerialString_t response(255, 0);    // The response from the protocol Filter

    Log(LOG_FN_ENTRY, "Enter Bosch8Module::StartSensorQualityTest()\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("StartSensorQualityTest", response);

    if (status == BEP_STATUS_SUCCESS)
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
            passed = ParseBooleanResponse(replyInterpNodePass->getValue(), response);
            if (passed)    // sensor quality test passed
            {
                // Try and parse the response into something readable
                Log(LOG_DEV_DATA, "Parsing <LFMin> byte\n");
                min[LFWHEEL] = ParseFloatResponse(replyInterpNodeLFMin->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <RFMin> byte\n");
                min[RFWHEEL] = ParseFloatResponse(replyInterpNodeRFMin->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <LRMin> byte\n");
                min[LRWHEEL] = ParseFloatResponse(replyInterpNodeLRMin->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <RRMin> byte\n");
                min[RRWHEEL] = ParseFloatResponse(replyInterpNodeRRMin->getValue(), response);

                Log(LOG_DEV_DATA, "SensorQuality min speeds: %.2f %.2f %.2f %.2f\n",
                    min[LFWHEEL], min[RFWHEEL], min[LRWHEEL], min[RRWHEEL]);

                Log(LOG_DEV_DATA, "Parsing <LFMax> byte\n");
                max[LFWHEEL] = ParseFloatResponse(replyInterpNodeLFMax->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <RFMax> byte\n");
                max[RFWHEEL] = ParseFloatResponse(replyInterpNodeRFMax->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <LRMax> byte\n");
                max[LRWHEEL] = ParseFloatResponse(replyInterpNodeLRMax->getValue(), response);
                Log(LOG_DEV_DATA, "Parsing <RRMax> byte\n");
                max[RRWHEEL] = ParseFloatResponse(replyInterpNodeRRMax->getValue(), response);

                Log(LOG_DEV_DATA, "SensorQuality max speeds: %.2f %.2f %.2f %.2f\n",
                    max[LFWHEEL], max[RFWHEEL], max[LRWHEEL], max[RRWHEEL]);

                status = BEP_STATUS_SUCCESS;
            }
            else
            {
                Log(LOG_ERRORS, "Sensor quality test failed\n");
                status = BEP_STATUS_FAILURE;
            }
        } catch (XmlException &ex)
        {
            Log(LOG_ERRORS, (ModuleName() + ": XmlError: " + ex.GetReason()).c_str());
            status = BEP_STATUS_SOFTWARE;
        }
    }
    Log(LOG_FN_ENTRY, "Exit Bosch8Module::StartSensorQualityTest(), status=%d\n", status);

    return status;
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes) {
//    Log(LOG_DEV_DATA, "GetDTCFormatUDS() returned %s. Using method %s\n",
//        GetDTCFormatUDS() ? "TRUE" : "FALSE",
//        GetDTCFormatUDS() ? "UDSReadFaults()" : "ReadFaults()");
//    if(GetDTCFormatUDS())
//    {
//        return (UDSReadFaults(faultCodes));
//    }
//    else
//    {
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    uint8_t     dtcCount = 0, dtcIdx = 0;
    uint16_t    dtcVal;
    char    dtcStr[16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ReadFaults", response);

    if (status == BEP_STATUS_SUCCESS)
    {
        // The first byte after the ID is a DTC count
        if (response.size() >= 2)
        {
            // Erase the first byte
            response.erase(0, 1);
            // Read the number of DTC's to follow
            dtcCount = response[0];
        }
        Log(LOG_DEV_DATA, "Module is reporting %d DTC's\n", dtcCount);

        // Extract the DTCs from the respnse
        for (dtcIdx = 0; (dtcIdx < dtcCount) && (response.size() > 1); dtcIdx++)
        {
            // DTC are sent as [ HI,LO]
            dtcVal = response[0];
            dtcVal = ((dtcVal << 8) & 0xFF00) | response[1];
            Log(LOG_DEV_DATA, "\tDTC %d: $%04X\n", dtcIdx, dtcVal);
            // Erase the bytes for this DTC
            response.erase(0, 2);

            // Add the DTC to the fault list
            std::string faultStr(CreateMessage(dtcStr, 16, "%04X", dtcVal));
            FaultCode_t fault(faultStr);
            faultCodes.push_back(fault);
        }

        // If we did not receive the full DTC list from the module
        if (dtcIdx < dtcCount)
        {
            // Log a message
            Log(LOG_ERRORS, "Did not receive full list of DTCs: %d of %d DTCs received\n",
                dtcIdx, dtcCount);
        }

        return (BEP_STATUS_SUCCESS);
    }
    else
    {
        return (status);
    }
//    }
}

template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::UDSReadFaults(FaultVector_t &faultCodes) {
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 dtcCount;
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", response);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {   // Good read, check the number of DTCs reported
        if (GetDTCCountIndex() >= 0)
        {
            dtcCount = response[GetDTCCountIndex()];
        }
        else
        {
            dtcCount = (response.length() - GetFirstDTCByteIndex()) / GetBytesPerDTC();
            Log(LOG_DEV_DATA, "No DTC Count Index, calculating number of DTCs\n");
            Log(LOG_DEV_DATA, "response length: %d, FirstDTCByteIndex: %d, BytesPerDTC: %d\n",
                response.length(), GetFirstDTCByteIndex(), GetBytesPerDTC());
        }
        // Check the number of DTCs reported
        if (dtcCount == 0 || dtcCount > 5000)
        {   // No DTCs reported
            Log(LOG_DEV_DATA, "No faults recorded in the module\n");
        }
        else
        {   // DTCs reported, loop to get them all
            UINT16 currentDTCIndex = GetFirstDTCByteIndex();
            Log(LOG_DEV_DATA, "Found %d DTCs recorded in the module:\n", dtcCount);
            for (UINT16 currentDTC = 0; currentDTC < dtcCount; currentDTC++)
            {   // Build a string representation of the DTC
                char buffer[8];
                string dtc = "";
                for (INT16 currentByte = 0; currentByte < (GetBytesPerDTC() - 1); currentByte++)
                {
                    dtc = dtc + CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", response[currentDTCIndex + currentByte]);
                    if ((currentDTCIndex + currentByte) >= response.length())
                    {
                        Log(LOG_ERRORS, "Accessing invalid data in moduleResonse, accessing: %d, resonse length: %d",
                            (currentDTCIndex + currentByte), response.length());
                        status = BEP_STATUS_SOFTWARE;
                    }
                }
                // Log the DTC
                Log(LOG_DEV_DATA, "\tDTC %03d - %s\n", currentDTC, dtc.c_str());
                // Stuff the DTC into the fault vector
                faultCodes.push_back(dtc);
                // Update the index to the next DTC field
                currentDTCIndex += GetBytesPerDTC();
            }
            // Check to make sure all reported faults were stored
            if (faultCodes.size() != dtcCount)
            {
                Log(LOG_ERRORS, "Error storing all reported DTCs - stored: %d, reported: %d\n",
                    faultCodes.size(), dtcCount);
                status = BEP_STATUS_SOFTWARE;
            }
        }
    }
    else
    {   // Error reading faults
        Log(LOG_ERRORS, "Error reading faults from the module - status: %s\n", ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

template<class ProtocolFilterType>
const INT32& Bosch8Module<ProtocolFilterType>::GetDTCCountIndex(void) {
    return m_dtcCountIndex;
}

template<class ProtocolFilterType>
const INT32& Bosch8Module<ProtocolFilterType>::GetBytesPerDTC(void) {
    return m_bytesPerDTC;
}

template<class ProtocolFilterType>
const INT32& Bosch8Module<ProtocolFilterType>::GetFirstDTCByteIndex(void) {
    return m_firstDTCByteIndex;
}

template<class ProtocolFilterType>
const INT16& Bosch8Module<ProtocolFilterType>::GetFirstFaultRegister(void) {
    return m_firstFaultRegister;
}

template<class ProtocolFilterType>
const INT32& Bosch8Module<ProtocolFilterType>::GetNumberOfFaultRegisters(void) {
    return m_numberOfFaultRegisters;
}

template<class ProtocolFilterType>
const bool& Bosch8Module<ProtocolFilterType>::FaultReadInProgress(void) {
    return m_readingFaults;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetDTCCountIndex(const INT32 &index) {
    m_dtcCountIndex = index;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetBytesPerDTC(const INT32 &bytesPerDTC) {
    m_bytesPerDTC = bytesPerDTC;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetFirstDTCByteIndex(const INT32 &firstDTCByteIndex) {
    m_firstDTCByteIndex = firstDTCByteIndex;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetFirstFaultRegister(const INT16 &firstFaultRegister) {
    m_firstFaultRegister = firstFaultRegister;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetNumberOfFaultRegisters(const INT32 &numberOfFaultRegisters) {
    m_numberOfFaultRegisters = numberOfFaultRegisters;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetReadFaultsInProgress(void) {
    m_readingFaults = true;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetReadFaultsComplete(void) {
    m_readingFaults = false;
}

template<class ProtocolFilterType>
const bool& Bosch8Module<ProtocolFilterType>::GetStopNoDTCs(void) {
    return m_stopNoDTCs;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetStopNoDTCs(const string &stopDTCRead) {
    if (stopDTCRead == "Yes")
        m_stopNoDTCs = true;
    else
        m_stopNoDTCs = false;
}

template<class ProtocolFilterType>
void Bosch8Module<ProtocolFilterType>::SetDTCFormatUDS(const bool &setValue) {
    m_dtcFormatUds = setValue;
}

template<class ProtocolFilterType>
bool Bosch8Module<ProtocolFilterType>::GetDTCFormatUDS(void) {
    return m_dtcFormatUds;
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::ProgramVIN(string moduleVin) {
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    SerialArgs_t vinToWrite;
    Log(LOG_FN_ENTRY, "Bosch8Module::ProgramVIN() - Enter\n");
    if (moduleVin.size())
    { // add VIN to serial args
        for (int ind = 0; ind < moduleVin.size(); ind++)
        {
            vinToWrite.push_back((UINT8)moduleVin.at(ind));
        }
        status = CommandModule("WriteVIN", &vinToWrite);
    }
    else
    { // nothing to do with argument "moduleVin" if it is empty.
      // Tell module to learn VIN from bus
        status = CommandModule("LearnVIN");
    }
    if (status == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "Vehicle VIN learned in module successfully");
    }
    else
    {
        Log(LOG_ERRORS, "Failed to learn Vehicle VIN in the module - status: %s", ConvertStatusToResponse(status).c_str());
    }
    Log(LOG_FN_ENTRY, "Bosch8Module::ProgramVIN() - Exit\n");
    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::UnlockModuleSecurity() {
    Log(LOG_FN_ENTRY, "Bosch8Module::UnlockModuleSecurity() - Enter");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if (m_securityUnlockType == "BIAbs" || m_securityUnlockType == "Abs91")
    {
        SerialArgs_t seed;         // Security seed read from the module
        SerialArgs_t securityKey;  // Security unlock key
        SerialString_t  moduleResponse;

        Log(LOG_DEV_DATA, "BIAbs security key\n");
        // Read the security seed from the module
        if ((status = ReadModuleData("ReadSecuritySeed", seed)) == BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "Seed (MSB - LSB): 0x%02X%02X%02X%02X\n",
                seed[3], seed[2], seed[1], seed[0]);
            securityKey.reserve(seed.size());
            securityKey.resize(seed.size());
            if ((status = CalculateSecurityKey(seed, securityKey)) == BEP_STATUS_SUCCESS)
            {   // Calculated the security key, unlock the module
                Log(LOG_DEV_DATA, "Calculated security key\n");
                //status = CommandModule("UnlockModuleSecurity", &securityKey);
                status = m_protocolFilter->GetModuleData("UnlockModuleSecurity", moduleResponse, &securityKey);
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
    {
        // Read the key from the module
        UINT32 seed = 0x00000000;
        status = ReadModuleData("ReadSecuritySeed", seed);
        if (status == BEP_STATUS_SUCCESS)
        {   // Calculate the security key
            UINT32 key = ((((seed >> 6) ^ seed) << 2) ^ seed);
            Log(LOG_DEV_DATA, "Calculated key %08X from seed %08X", key, seed);
            // Write the key back to the module
            SerialArgs_t keyArgs;
            keyArgs.push_back((key & 0xFF000000) >> 24);
            keyArgs.push_back((key & 0x00FF0000) >> 16);
            keyArgs.push_back((key & 0x0000FF00) >> 8);
            keyArgs.push_back(key & 0x000000FF);
            status = CommandModule("UnlockModuleSecurity", &keyArgs);
            Log(LOG_ERRORS, "Security unlock status: %s", ConvertStatusToResponse(status).c_str());
        }
        else
        {
            Log(LOG_ERRORS, "Failed to read security seed from the module");
        }
    }
    Log(LOG_FN_ENTRY, "Bosch8Module::UnlockModuleSecurity() - Exit");
    return status;
}

//-------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE Bosch8Module<ProtocolFilterType>::CalculateSecurityKey(const SerialArgs_t &seedInput,
                                                                       SerialArgs_t &keyOutput) {
    UINT32 seed = 0x00000000;

    for (int n = 3; n >= 0; n--) seed = (seed << 8) + seedInput[n];

    Log(LOG_DEV_DATA, "Assembled seed into UINT32 variable: %08X\n", seed);

    UINT32 dataMask = 0x00000000;
    UINT8 dataMaskIndex = (seedInput[0] & 0x0F);
    UINT8 rotateCount = 0x00;

    UINT32 lookupTable[16] =
    {
        0xF549506A,
        0x47152AB0,
        0x2E31DA76,
        0x53266FE2,
        0x874318C0,
        0x2FE296BF,
        0xCB84F0FE,
        0xE806EC53,
        0X018F28ED,
        0X487E89DE,
        0XCDC1B660,
        0XFC0829D3,
        0X847FCE7C,
        0X17E8DD4D,
        0XC666FF1C,
        0XBDC5DBE9
    };

    UINT32 lookupTableAbs91[16] =
    {
        0x428A2F98,
        0x71374491,
        0xB5C0FBCF,
        0xE9B5DBA5,
        0x3956C25B,
        0x59F111F1,
        0x923F82A4,
        0xAB1C5ED5,
        0xD807AA98,
        0x12835B01,
        0x243186BE,
        0x550C7DC3,
        0x72BE5D74,
        0x80DBB1FE,
        0x9BDC06A7,
        0xC19BF174
    };
    Log(LOG_DEV_DATA,"Security Unlock Type: %s",m_securityUnlockType.c_str());
    //If the Abs module is 9.1 or newer then use new lookup table values
    if (m_securityUnlockType.compare("Abs91") == 0)
    {
        dataMask = lookupTableAbs91[dataMaskIndex];
    }

    else
    {
        dataMask = lookupTable[dataMaskIndex];
    }

    Log(LOG_DEV_DATA, "Data mask -- Index: %d; Value: %08X\n",
        dataMaskIndex, dataMask);

    seed = (seed ^ dataMask);
    Log(LOG_DEV_DATA, "Assembled seed into UINT32 variable: %08X\n", seed);

    rotateCount = ((seed >> 16) & 0x0F);
    Log(LOG_DEV_DATA, "Rotation count: %d\n", rotateCount);

    bool rotateRight = (((seed >> 24) & 0x80) == 0x80);
    Log(LOG_DEV_DATA, "Rotating %s ;)\n", rotateRight ? "right" : "left");
    if (rotateRight)
    {
        for (int n = 0; n < rotateCount; n++)
        {
            UINT32 rotator = seed & 0x00000001;
            seed = seed >> 1;
            seed = (seed | (rotator << 31));
        }
    }
    else
    {
        for (int n = 0; n < rotateCount; n++)
        {
            UINT32 rotator = seed & 0x80000000;
            seed = seed << 1;
            seed = (seed | (rotator >> 31));
        }
    }

    Log(LOG_DEV_DATA, "Calculated security key: %08X\n", seed);
    keyOutput[0] = ((seed & 0xFF000000) >> 24);
    keyOutput[1] = ((seed & 0x00FF0000) >> 16);
    keyOutput[2] = ((seed & 0x0000FF00) >> 8);
    keyOutput[3] = (seed & 0x000000FF);

    /*keyOutput[3] = ((seed & 0xFF000000) >> 24);
    keyOutput[2] = ((seed & 0x00FF0000) >> 16);
    keyOutput[1] = ((seed & 0x0000FF00) >> 8);
    keyOutput[0] = (seed & 0x000000FF);*/

    // return the status
    return (BEP_STATUS_SUCCESS);
}
