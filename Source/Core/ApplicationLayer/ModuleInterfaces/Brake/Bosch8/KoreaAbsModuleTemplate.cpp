//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/KoreaAbsModuleTemplate.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//  Module Interface for a Korea ABS Controller module.
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
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/KoreaAbsModuleTemplate.h $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 1     8/30/04 7:23p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
//*************************************************************************

#include "../Include/KoreaAbsModuleTemplate.h"

template <class ProtocolFilterType>
KoreaAbsModuleTemplate<ProtocolFilterType>::KoreaAbsModuleTemplate() : GenericABSModuleTemplate<ProtocolFilterType>(),
    m_readSensorSpeeds("ReadSensorSpeeds")
{
}

template <class ProtocolFilterType>
KoreaAbsModuleTemplate<ProtocolFilterType>::~KoreaAbsModuleTemplate()
{
}

template <class ProtocolFilterType>
string KoreaAbsModuleTemplate<ProtocolFilterType>::ModuleName()
{
    return "KoreaAbsModuleTemplate";
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{   
    BEP_STATUS_TYPE retVal = BEP_STATUS_ERROR;
    // No special method, try the base class
    retVal = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    return retVal;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string method, bool &value)  throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    
    retVal = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(method, value);
    return retVal;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{   // No special method, try the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException)
{	// Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering WheelSpeeds_t GetInfo(%s)", 
        KoreaAbsModuleTemplate<ProtocolFilterType>::ModuleName().c_str(), method.c_str());
    // set the initial value
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Any of these could throw module exception
    // Read the wheel speed sensors
    if (method == m_readSensorSpeeds) retVal = ReadSensorSpeeds(value);
    else
    {	// No special method, try using the base class
        Log(LOG_DEV_DATA, "%s::GetInfo(WheelSpeeds_t) calling GenericModuleTemplate::GetInfo(WheelSpeeds_t)", 
            KoreaAbsModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
        retVal = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(method, value);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting GetInfo WheelSpeeds_t", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    // Return the return value
    return retVal;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{	// No special method available, try using the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{   // No special method, try the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{   
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // No special method, try the base class
    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{   // No special method, try the base class
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{   // No special method, just try the base clss
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE KoreaAbsModuleTemplate<ProtocolFilterType>::ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException)
{	// Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ReadSensorSpeeds", ModuleName().c_str());
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // The response from the protocol Filter
    SerialString_t response;
    Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Checking object status", ModuleName().c_str());
    // Check to see that all our objects are in place
    // This throws a ModuleException if things aren't good
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Calling GetModuleData", ModuleName().c_str());
    status = m_protocolFilter->GetModuleData("ReadSensorSpeeds", response);
    // Ask the protocol filter for the info
    if (status == BEP_STATUS_SUCCESS)
    {
        try
        {
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Getting reply interpretation for LF", ModuleName().c_str());
            // Get the reply interpretation from the Xml
            const XmlNode *replyInterpNodeLF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLF");
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Getting reply interpretation for RF", ModuleName().c_str());
            const XmlNode *replyInterpNodeRF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRF");
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Getting reply interpretation for LR", ModuleName().c_str());
            const XmlNode *replyInterpNodeLR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLR");
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Getting reply interpretation for RR", ModuleName().c_str());
            const XmlNode *replyInterpNodeRR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRR");
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Parsing LF Response", ModuleName().c_str());
            // Try to parse the response into something readable
            speeds[LFWHEEL] = ParseFloatResponse(replyInterpNodeLF->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Parsing RF Response", ModuleName().c_str());
            speeds[RFWHEEL] = ParseFloatResponse(replyInterpNodeRF->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Parsing LR Response", ModuleName().c_str());
            speeds[LRWHEEL] = ParseFloatResponse(replyInterpNodeLR->getValue(), response);
            Log(LOG_DEV_DATA, "%s: ReadSensorSpeeds: Parsing RR Response", ModuleName().c_str());
            speeds[RRWHEEL] = ParseFloatResponse(replyInterpNodeRR->getValue(), response);
            // Log the wheel speed sensors read from the module
            Log(LOG_DEV_DATA, "Read Sensor Speeds: \n\tLF = %g\n\tRF = %g\n\tLR = %g\n\tRR = %g\n",
                speeds[LFWHEEL], speeds[RFWHEEL], speeds[LRWHEEL], speeds[RRWHEEL]);
        }
        catch (XmlException &ex)
        {	// Log it
            Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
            // set status to a software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ReadSensorSpeeds", ModuleName().c_str());
    // Return the status of the operation
    return status;
}

template <class ProtocolFilterType>
bool KoreaAbsModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    Log(LOG_FN_ENTRY, "%s: InitializeHook()", GenericABSModuleTemplate<ProtocolFilterType>::ModuleName().c_str());
    return GenericABSModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
}

