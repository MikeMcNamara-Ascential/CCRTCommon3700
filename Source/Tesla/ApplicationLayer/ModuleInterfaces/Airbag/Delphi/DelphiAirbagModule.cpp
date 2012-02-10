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
#include "DelphiAirbagModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DelphiAirbagModule<ProtocolFilterType>::DelphiAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DelphiAirbagModule<ProtocolFilterType>::~DelphiAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string DelphiAirbagModule<ProtocolFilterType>::ModuleName()
{
    return "Delphi Airbag Module";
}



//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE DelphiAirbagModule<ProtocolFilterType>::GetSecurityAccess()
{
    // from something else 
    SerialArgs_t seed;           // Security seed read from the module
    SerialArgs_t securityKeyc;    // Security unlock key
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t  moduleResponse;
    string test123="";
    Log(LOG_FN_ENTRY, "DelphiAirbagModule::GetSecurityAccess() - Enter\n");
    //bool AccessGranted;
    // Read the security seed from the module
    if ((status = ReadModuleData("CommandSecurityAccess", seed)) == BEP_STATUS_SUCCESS)
    {   
        if (seed.size()!=0) 
        {
        
            //calculate key using Ones Complement
            for (UINT32 ii = 0; ii < seed.size();ii++) 
            {
                securityKeyc.push_back(~seed[ii]);
            }
            //transmist key back to module
            if ((status=CommandModule("TransmitSecurityKey",&securityKeyc)) == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Security Key sent successfully");
            }
            else
            {
                Log(LOG_ERRORS, "Failed to send Security Key from the module - status: %s",ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {
             Log(LOG_DEV_DATA, "Seed Key is NULL");
        }
        
        

    }
    else
    {   
        Log(LOG_ERRORS, "Failed to read Security Seed from the module - status: %s",ConvertStatusToResponse(status).c_str());
    }
    Log(LOG_FN_ENTRY, "DelphiAirbagModule::GetSecurityAccess() - Exit\n");
    // Return the status     
    return status;
}

template <class ProtocolFilterType>
bool DelphiAirbagModule<ProtocolFilterType>::Initialize (const XmlNode *configNode)
{
    Log(LOG_DEV_DATA, "Enter DelphiAirbagModule<ProtocolFilterType>::Initialize\n");
    GenericModuleTemplate<ProtocolFilterType>::Initialize(configNode);

    // Store the DTC count index
	try
	{	// Get the DTC count index
		SetDTCCountIndex(BposReadInt(configNode->getChild("Setup/DTCData/DTCCountIndex")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "XmlException getting DTC count index, using 2: %s\n", err.Reason().c_str());
		SetDTCCountIndex(2);
	}
	// Store the Number of bytes per DTC
	try
	{
		SetBytesPerDTC(BposReadInt(configNode->getChild("Setup/DTCData/BytesPerDTC")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "XmlException getting number of bytes per DTC field, using 3: %s\n", err.Reason().c_str());
		SetBytesPerDTC(3);
	}
	// Store the index of the first DTC byte
	try
	{
		SetFirstDTCByteIndex(BposReadInt(configNode->getChild("Setup/DTCData/FirstDTCByteIndex")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "XmlException getting index of the first DTC field, using 4: %s\n", err.Reason().c_str());
		SetFirstDTCByteIndex(4);
	}
	// Load the FaultRegisters used to determine which byte of the module readfault response corresponds to what set
	// 		of register codes
	try
	{
		m_faultByteCodes.DeepCopy(configNode->getChild("Setup/FaultByteCodes")->getChildren());
	}
	catch (XmlException &err)
	{	// Error loading the Fault Registers
		Log(LOG_ERRORS, "No Fault byte codes to load - %s\n", err.what());
	}
	// Load the first byte address of the fault registers
	try
	{
		SetFirstFaultRegister(atoh(configNode->getChild("Setup/DTCData/FirstFaultRegister")->getValue().c_str()));
	}
	catch (XmlException &err)
	{	// Error loading the First Fault Register address
		Log(LOG_ERRORS, "No Fault Register address to load - %s\n", err.what());
	}
	// Load the number of fault registers
	try
	{
		SetNumberOfFaultRegisters(atoi(configNode->getChild("Setup/DTCData/NumberOfFaultRegisters")->getValue().c_str()));
	}
	catch (XmlException &err)
	{	// Error loading the number of fault registers
		Log(LOG_ERRORS, "No Number of Fault Registers to load - %s\n", err.what());
	}
  	// Load the option to stop read if first data byte returned is zero
	try
	{
        SetStopNoDTCs(configNode->getChild("Setup/DTCData/StopDTCReadOnZero")->getValue());
	}
	catch (XmlException &err)
	{	// Error loading the number of fault registers
		Log(LOG_ERRORS, "No stop DTC read option to load - %s\n", err.what());
	}
	// Save the configuration data
    m_moduleConfig = configNode->Copy();
    Log(LOG_DEV_DATA, "Exit DelphiAirbagModule<ProtocolFilterType>::Initialize\n");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE DelphiAirbagModule<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    UINT16 dtcCount;
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read th faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", response);
    // Check the status of the read
    if (BEP_STATUS_SUCCESS == status)
	{	// Good read, check the number of DTCs reported
        if(GetDTCCountIndex() >= 0)
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
		{	// No DTCs reported
			Log(LOG_DEV_DATA, "No faults recorded in the module\n");
		}
		else
		{	// DTCs reported, loop to get them all
			UINT16 currentDTCIndex = GetFirstDTCByteIndex();
			Log(LOG_DEV_DATA, "Found %d DTCs recorded in the module:\n", dtcCount);
			for (UINT16 currentDTC = 0; currentDTC < dtcCount; currentDTC++)
			{	// Build a string representation of the DTC
			     char buffer[8];
				string dtc = "";
                for(INT16 currentByte = 0; currentByte < (GetBytesPerDTC()-1); currentByte++)
                {
                    dtc = dtc + CreateMessage(buffer, (UINT32)sizeof(buffer), "%02X", response[currentDTCIndex+currentByte]);
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
	{	// Error reading faults
		Log(LOG_ERRORS, "Error reading faults from the module - status: %s\n", ConvertStatusToResponse(status).c_str());
	}
	// Return the status
	return status;
}

template <class ProtocolFilterType>
const INT32& DelphiAirbagModule<ProtocolFilterType>::GetDTCCountIndex(void)
{
	return m_dtcCountIndex;
}

template <class ProtocolFilterType>
const INT32& DelphiAirbagModule<ProtocolFilterType>::GetBytesPerDTC(void)
{
	return m_bytesPerDTC;
}

template <class ProtocolFilterType>
const INT32& DelphiAirbagModule<ProtocolFilterType>::GetFirstDTCByteIndex(void)
{
	return m_firstDTCByteIndex;
}

template <class ProtocolFilterType>
const INT16& DelphiAirbagModule<ProtocolFilterType>::GetFirstFaultRegister(void)
{
	return m_firstFaultRegister;
}

template <class ProtocolFilterType>
const INT32& DelphiAirbagModule<ProtocolFilterType>::GetNumberOfFaultRegisters(void)
{
	return m_numberOfFaultRegisters;
}

template <class ProtocolFilterType>
const bool& DelphiAirbagModule<ProtocolFilterType>::FaultReadInProgress(void)
{
	return m_readingFaults;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetDTCCountIndex(const INT32 &index)
{
	m_dtcCountIndex = index;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetBytesPerDTC(const INT32 &bytesPerDTC)
{
	m_bytesPerDTC = bytesPerDTC;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetFirstDTCByteIndex(const INT32 &firstDTCByteIndex)
{
	m_firstDTCByteIndex = firstDTCByteIndex;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetFirstFaultRegister(const INT16 &firstFaultRegister)
{
	m_firstFaultRegister = firstFaultRegister;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetNumberOfFaultRegisters(const INT32 &numberOfFaultRegisters)
{
	m_numberOfFaultRegisters = numberOfFaultRegisters;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetReadFaultsInProgress(void)
{
	m_readingFaults = true;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetReadFaultsComplete(void)
{
	m_readingFaults = false;
}

template <class ProtocolFilterType>
const bool& DelphiAirbagModule<ProtocolFilterType>::GetStopNoDTCs(void)
{
	return m_stopNoDTCs;
}

template <class ProtocolFilterType>
void DelphiAirbagModule<ProtocolFilterType>::SetStopNoDTCs(const string &stopDTCRead)
{
	if(stopDTCRead == "Yes")
       m_stopNoDTCs = true;
    else
       m_stopNoDTCs = false;
}

