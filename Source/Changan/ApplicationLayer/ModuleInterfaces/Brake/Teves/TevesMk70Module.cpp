//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Teves MK70 ABS.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "TevesMk70Module.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
TevesMk70Module<ProtocolFilterType>::TevesMk70Module() : GenericABSModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
TevesMk70Module<ProtocolFilterType>::~TevesMk70Module()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string TevesMk70Module<ProtocolFilterType>::ModuleName(void)
{
    return "Teves MK70 ABS";
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilter>
bool TevesMk70Module<ProtocolFilter>::InitializeHook(const XmlNode *configNode)
{   // Begin by initializing the base class
    bool status = GenericABSModuleTemplate<ProtocolFilter>::InitializeHook(configNode);
    // Initialize the keep alive comm object
    if(m_keepAliveComm != NULL)
    {
        Log(LOG_DEV_DATA, "BoschAbsModule::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize(configNode);
    }
    
    if (m_protocolFilter != NULL)
    {
       m_protocolFilter->InitializeServer("core/WinCcrt");
    }
    
    // Store the byte containing the number of DTCs reported in the module response
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
        byte = 0;
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
        byte = 1;
        Log(LOG_ERRORS, "Bytes per DTC not specified, using 1");
    }
    Log(LOG_DEV_DATA, "Setting bytes per DTC to %d", byte);
    BytesPerDtc(&byte);
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
BEP_STATUS_TYPE TevesMk70Module<ProtocolFilter>::ReadFaults(FaultVector_t &faultCodes)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    UINT16 dtcCount;
    // Read the faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", moduleResponse);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
    {	// Good read, check the number of DTCs reported
        dtcCount = moduleResponse[DtcCountIndex()];
        // Check the number of DTCs reported
        if (dtcCount == 0)
        {	// No DTCs reported
            Log(LOG_DEV_DATA, "No faults recorded in the module\n");
        }
        else
        {	// DTCs reported, loop to get them all
            UINT16 currentDTCIndex = FirstDtcByteIndex();
            Log(LOG_DEV_DATA, "Found %d DTCs recorded in the module:\n", dtcCount);
            for (UINT16 currentDTC = 0; currentDTC < dtcCount; currentDTC++)
            {	// Build a string representation of the DTC
                 char buffer[8];
                string dtc = "";
                for(INT16 currentByte = 0; currentByte < (BytesPerDtc()-1); currentByte++)
                {
                    dtc = dtc + CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", moduleResponse[currentDTCIndex+currentByte]);
                    if ((UINT16)(currentDTCIndex + currentByte) >= moduleResponse.length())
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
            if (faultCodes.size() != dtcCount)
            {
                Log(LOG_ERRORS, "Error storing all reported DTCs - stored: %d, reported: %d\n",
                    faultCodes.size(), dtcCount);
                status = BEP_STATUS_SOFTWARE;
            }
        }
    }
    else
    {	// Error reading faults
        Log(LOG_ERRORS, "Error reading faults from the module - status: %s\n", ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TevesMk70Module<ProtocolFilter>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TevesMk70Module<ProtocolFilter>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TevesMk70Module<ProtocolFilter>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}

