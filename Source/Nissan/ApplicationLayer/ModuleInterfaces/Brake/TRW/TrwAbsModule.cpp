//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the TRW ABS.
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
#include "TrwAbsModule.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
TrwAbsModule<ProtocolFilterType>::TrwAbsModule() : GenericABSModuleTemplate<ProtocolFilterType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
TrwAbsModule<ProtocolFilterType>::~TrwAbsModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string TrwAbsModule<ProtocolFilterType>::ModuleName(void)
{
    return "TRW ABS";
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFilter>
bool TrwAbsModule<ProtocolFilter>::InitializeHook(const XmlNode *configNode)
{   // Begin by initializing the base class
    bool status = GenericABSModuleTemplate<ProtocolFilter>::InitializeHook(configNode);
    // Initialize the keep alive comm object
    if(m_keepAliveComm != NULL)
    {
        Log(LOG_DEV_DATA, "BoschAbsModule::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize(configNode);
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
BEP_STATUS_TYPE TrwAbsModule<ProtocolFilter>::ReadFaults(FaultVector_t &faultCodes)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t moduleResponse;
    UINT16 dtcCount;
	// Read the faults from the module
	status = m_protocolFilter->GetModuleData("ReadFaults", moduleResponse);
	// Check the status of the read
	if (BEP_STATUS_SUCCESS == status)
	{	// Good read, check the number of DTCs reported
        if(DtcCountIndex() >= 0)
        {
        dtcCount = moduleResponse[DtcCountIndex()];
        }
        else
        {
            dtcCount = (moduleResponse.length() - FirstDtcByteIndex()) / BytesPerDtc();
            Log(LOG_DEV_DATA, "No DTC Count Index, calculating number of DTCs\n");
            Log(LOG_DEV_DATA, "response length: %d, FirstDTCByteIndex: %d, BytesPerDTC: %d\n",
                moduleResponse.length(), FirstDtcByteIndex(), BytesPerDtc());
        }
		// Check the number of DTCs reported
		if (dtcCount == 0 || dtcCount > 5000)
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
                    if ((currentDTCIndex + currentByte) >= moduleResponse.length())
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


//-------------------------------------------------------------------------------------------------
template <class ProtocolFilter>
BEP_STATUS_TYPE TrwAbsModule<ProtocolFilter>::UnlockModuleSecurity(void)
{   // Log the entry
	Log(LOG_FN_ENTRY, "TrwAbsModule::UnlockModuleSecurity() - Enter");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT32 Seed;
    // Read the security key from the module
    if ((status = ReadModuleData("ReadSecuritySeed", Seed)) == BEP_STATUS_SUCCESS)
    {   
		Log(LOG_DEV_DATA, "Seed from Module: 0x%08X", Seed);
        // Calculate the key
        UINT32 AppKeyConst = 0x70F47006;
        UINT32 Seed1 = Seed; //make a copy so it can be used later
        //ratate Seed into Seed2
        UINT32 Seed2 = 0;
        for (int i = 0; i < 32; i++)
        {
            Seed2 = (Seed2 << 1) + (Seed & 1); 
            Seed >>= 1; 
        }
        UINT32 Key1 = Seed1 ^ AppKeyConst;
        UINT32 Key2 = Seed2 ^ AppKeyConst;
        UINT32 Key = Key1 + Key2;

        //Key is now calculated.  Get ready to send it!
        union _keyData
        {
            UINT32 fullKey;
            struct
            {
                UINT8 key_0;
                UINT8 key_1;
                UINT8 key_2;
                UINT8 key_3;
            } keyChars;
        } keyData;
        keyData.keyChars.key_0 = (Key & 0x000000FF);
		keyData.keyChars.key_1 = ((Key & 0x0000FF00) >> 8);
		keyData.keyChars.key_2 = ((Key & 0x00FF0000) >> 16);
		keyData.keyChars.key_3 = ((Key & 0xFF000000) >> 24);
		keyData.fullKey = Key;
		Log(LOG_DEV_DATA, "Calcualted key: 0x%08X  ($%02X $%02X $%02X $%02X)", keyData.fullKey, keyData.keyChars.key_3, keyData.keyChars.key_2, keyData.keyChars.key_1, keyData.keyChars.key_0);

		SerialArgs_t keyArgs;
		keyArgs.push_back(keyData.keyChars.key_3);
		keyArgs.push_back(keyData.keyChars.key_2);
		keyArgs.push_back(keyData.keyChars.key_1);
		keyArgs.push_back(keyData.keyChars.key_0);
		// Write the key to the module
		status = CommandModule("UnlockSecurity", &keyArgs);
		Log(LOG_ERRORS, "Security unlock: %s", ConvertStatusToResponse(status).c_str());
	}
    else
	{
		Log(LOG_ERRORS, "Error reading security seed from the module: %s", ConvertStatusToResponse(status).c_str());
	}
	// Log the exit and return the status
	Log(LOG_FN_ENTRY, "TrwAbsModule::UnlockModuleSecurity() - Exit");
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TrwAbsModule<ProtocolFilter>::DtcCountIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_dtcCountIndex = *byte;
    return m_dtcCountIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TrwAbsModule<ProtocolFilter>::FirstDtcByteIndex(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_firstDtcIndex = *byte;
    return m_firstDtcIndex;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilter>
inline const UINT8& TrwAbsModule<ProtocolFilter>::BytesPerDtc(const UINT8 *byte /*= NULL*/)
{
    if(byte != NULL)  m_bytesPerDtc = *byte;
    return m_bytesPerDtc;
}
