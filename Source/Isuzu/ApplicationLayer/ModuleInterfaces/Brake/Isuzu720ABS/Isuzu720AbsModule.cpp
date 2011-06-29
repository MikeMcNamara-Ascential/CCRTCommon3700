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
#include "Isuzu720AbsModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
Isuzu720AbsModule<ProtocolFilterType>::Isuzu720AbsModule() : GenericABSModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
Isuzu720AbsModule<ProtocolFilterType>::~Isuzu720AbsModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string Isuzu720AbsModule<ProtocolFilterType>::ModuleName()
{
    return "Isuzu 720 ABS Module";
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
bool Isuzu720AbsModule<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    bool status = false;

    Log(LOG_FN_ENTRY, "Enter Isuzu720AbsModuleTemplate::InitializeHook()\n");

    // Call the base class to complete the initialization
    status = GenericABSModuleTemplate<ProtocolFilterType>::InitializeHook(configNode);
    // Make sure the keep alive comm has been initialized
    if(m_keepAliveComm != NULL)
    {
        m_keepAliveComm->Initialize(configNode);
    }
    // Store the byte containing the number of DTCs reported in the module response
    Log(LOG_DEV_DATA, "Isuzu720AbsModuleTemplate::InitializeHook: Getting Dtc Info from Config\n");
    UINT8 byte = 0x00;
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/DtcCountIndex")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 0;
        Log(LOG_ERRORS, "DTC Count index not defined, using 0");
    }
    Log(LOG_DEV_DATA, "Setting DTC Count Index to %d", byte);
    DtcCountIndex(&byte);
    // Store the index of the first byte of the first dtc
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/DtcStartByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 1;
        Log(LOG_ERRORS, "DTC start index not defined, using 0");
    }
    Log(LOG_DEV_DATA, "Setting DTC Start Index to %d", byte);
    FirstDtcByteIndex(&byte);
    // Store the number of bytes per DTC
    try
    {
        byte = BposReadInt(configNode->getChild("Setup/DtcData/BytesPerDtc")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        byte = 3;
        Log(LOG_ERRORS, "Bytes per DTC not specified, using 1");
    }
    Log(LOG_DEV_DATA, "Setting bytes per DTC to %d", byte);
    BytesPerDtc(&byte);

    Log(LOG_FN_ENTRY, "Exit Isuzu720AbsModuleTemplate::InitializeHook(), status=%d\n", status);

    return(status);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Read the module part number registers
    if(methodName == "ReadModulePartNumber")     status = ReadModulePartNumber(value);
    // No special implementation, use the base class
    else  status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    // Return the status
    return status;
}

template <class ProtocolFilterType>
//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, bool &value)  throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(methodName == "ReadFaults")
    {   // Read the module faults
        status = ReadFaults(value);
    }
    else
    {
        status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    }    

    return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, WheelSpeeds_t &value)  throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{
    return GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::ReadModulePartNumber(string &partNumber)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string partNumberFirstHalf;
    string partNumberLastHalf;

    status = ReadModuleData("ReadPartNumberFirstNumber", partNumberFirstHalf);
    if(status == BEP_STATUS_SUCCESS)
    {
        partNumber += partNumberFirstHalf;
        // Log completed parameter string
        Log(LOG_DEV_DATA, "Isuzu 720 ABS First Half Part Number: %s\n", partNumberFirstHalf.c_str());

    }
    else
    {
        //FIRST READ WAS UNSECCUSSFUL
        // Error occurred while trying to read module eeprom memory
        Log(LOG_ERRORS,"Error, Isuzu720AbsModule::ReadModulePartNumber() - Failed to read first half of part number, status = %s\n",
            ConvertStatusToResponse(status).c_str());
    }
    status = ReadModuleData("ReadPartNumberLastNumber", partNumberLastHalf);
    if(status == BEP_STATUS_SUCCESS)
    {
        partNumber += partNumberLastHalf;
        // Log completed parameter string
        Log(LOG_DEV_DATA, "Isuzu 720 ABS Last Half Part Number: %s\n", partNumberLastHalf.c_str());

    }
    else
    {
        //FIRST READ WAS UNSECCUSSFUL
        // Error occurred while trying to read module eeprom memory
        Log(LOG_ERRORS,"Error, Isuzu720AbsModule::ReadModulePartNumber() - Failed to read last half of part number, status = %s\n",
            ConvertStatusToResponse(status).c_str());
    }

    return status;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE Isuzu720AbsModule<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t moduleResponse(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 dtcCount;

    Log(LOG_FN_ENTRY, "Isuzu720AbsModule::ReadFaults() - Enter");

    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", moduleResponse);
    // Check the status of the read

    if(status == BEP_STATUS_SUCCESS)
    {
        string fullResponse;
        char temp[256];
        for(UINT16 ii = 0; ii < moduleResponse.length(); ii++)
            fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
        Log(LOG_DEV_DATA, "Module response: %s\n", fullResponse.c_str());


        dtcCount = (moduleResponse.length() - FirstDtcByteIndex()) / BytesPerDtc();
        Log(LOG_DEV_DATA, "Calculating number of DTCs\n");
        Log(LOG_DEV_DATA, "response length: %d, FirstDTCByteIndex: %d, BytesPerDTC: %d\n",
            moduleResponse.length(), FirstDtcByteIndex(), BytesPerDtc());


        // Check the number of DTCs reported
        if((moduleResponse[FirstDtcByteIndex()] == 0x00) && (moduleResponse[FirstDtcByteIndex()+1] == 0x00))
        {   // No DTCs reported
            Log(LOG_DEV_DATA, "No faults recorded in the module\n");
        }
        else
        {   // DTCs reported, loop to get them all
            UINT16 currentDTCIndex = FirstDtcByteIndex();
            Log(LOG_DEV_DATA, "Found %d DTCs recorded in the module:\n", dtcCount);
            for(UINT16 currentDTC = 0; currentDTC < dtcCount; currentDTC++)
            {   // Build a string representation of the DTC
                char buffer[8];
                string dtc = "";
                for(INT16 currentByte = 0; currentByte < (BytesPerDtc()-1); currentByte++)
                {
                    dtc = dtc + CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", moduleResponse[currentDTCIndex+currentByte]);
                    if((currentDTCIndex + currentByte) >= (UINT16)moduleResponse.length())
                    {
                        Log(LOG_ERRORS, "Accessing invalid data in moduleResonse, accessing: %d, resonse length: %d",
                            (currentDTCIndex + currentByte), moduleResponse.length());
                        status = BEP_STATUS_SOFTWARE;
                    }
                }
                // Log the DTC
                Log(LOG_DEV_DATA, "\tDTC %03d - %s\n", currentDTC, dtc.c_str());
                // Stuff the DTC into the fault vector
                faultCodes.push_back(dtc);
                // Update the index to the next DTC field
                currentDTCIndex += BytesPerDtc();
            }
            // Check to make sure all reported faults were stored
            if(faultCodes.size() != dtcCount)
            {
                Log(LOG_ERRORS, "Error storing all reported DTCs - stored: %d, reported: %d\n",
                    faultCodes.size(), dtcCount);
                status = BEP_STATUS_SOFTWARE;
            }
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read faults from %s\n", ModuleName().c_str());
    }

    Log(LOG_FN_ENTRY, "Isuzu720AbsModule::ReadFaults() - Exit:%d", status);

    // Return the status
    return(status);
}


//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& Isuzu720AbsModule<ProtocolFilterType>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& Isuzu720AbsModule<ProtocolFilterType>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline const UINT8& Isuzu720AbsModule<ProtocolFilterType>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}
