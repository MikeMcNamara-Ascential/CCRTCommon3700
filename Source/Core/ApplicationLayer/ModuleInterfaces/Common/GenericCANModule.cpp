//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Common/GenericCANModule.cpp 5     12/31/07 3:28p Derickso $
//
// FILE DESCRIPTION:
// Generic Daimler Chrysler module interface for a CAN based module.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Common/GenericCANModule.cpp $
// 
// 5     12/31/07 3:28p Derickso
// Removed support for obsolete vehicle buses.
// 
// 4     10/08/07 12:16p Derickso
// Updated header comments to doxygen format.
// REmoved InitializeHook since all it did was call the base class.
// 
// 3     7/19/07 8:49a Derickso
// Updated to handle UDS 3 byte DTCs.
// 
// 2     3/22/07 2:08p Dkrause
// Updated unsupported communication protocals to throw module exceptions,
// instead of just returning a fail status
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     6/09/05 1:44p Derickso
// Added ReadBitEncodedFaultRegisters.
// 
// 1     12/01/04 10:10a Derickso
// Created new VSS database.
// 
// 1     6/09/04 8:08a Derickso
// Dcx Common Module Interface files.
// 
// 1     1/27/04 9:28a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 4     1/20/04 6:02a Derickso
// Updated to inherit from new GenericDCXModuleTool interface class.
// 
// 3     10/07/03 1:14p Derickso
// Moved initialization of ILogger to beginning of Initialize().
// Corrected config file locations in Initialize.
// 
// 2     8/26/03 10:20a Derickso
// Updated for new directory structure.
// Compiles, not tested.
// 
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
// 
// 1     8/18/03 9:20a Derickso
// Created.  Initial Checkin
// 
//*************************************************************************
#include "GenericCANModule.h"

GenericCANModule::GenericCANModule(IProtocolFilter *protocolFilter) : GenericCANModuleTool(protocolFilter)
{   // Nothing special to be done here
}

GenericCANModule::~GenericCANModule()
{   // Nothing special to be done here
}

BEP_STATUS_TYPE GenericCANModule::ReadBitEncodedFaultRegisters(FaultVector_t &faultCodes)
{   // NOTE: None of the Keyword Protocol 2000 modules support reading of individual fault registers
	//       This method should never be invoked for a Keyword Protocol 2000 module
	Log(LOG_ERRORS, "WARNING: ReadBitEncodedFaultRegisters is not supported for this communication protocol !!!\n");
    // Throw an exception to cause an error sequencing drive curve 
    throw(ModuleException("ReadBitEncodedFaultRegisters is not supported for this communication protocol !!!\n"));
    // Should never get to the return
	return BEP_STATUS_SOFTWARE;
}

BEP_STATUS_TYPE GenericCANModule::ReadFaultRegisters(FaultVector_t &faultCodes)
{   // NOTE: None of the Keyword Protocol 2000 modules support reading of individual fault registers
	//       This method should never be invoked for a Keyword Protocol 2000 module
	Log(LOG_ERRORS, "WARNING: ReadFaultRegisters is not supported for this communication protocol !!!\n");
    // Throw an exception to cause an error sequencing drive curve 
    throw(ModuleException("ReadFaultRegisters is not supported for this communication protocol !!!\n"));
    // Should never get to the return
	return BEP_STATUS_SOFTWARE;
}

BEP_STATUS_TYPE GenericCANModule::ReadFaults(FaultVector_t &faultCodes)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t moduleResponse;
    UINT16 dtcCount;
	// Read the faults from the module
	SetReadFaultsInProgress();
	status = m_protocolFilter->GetModuleData("ReadFaults", moduleResponse);
	SetReadFaultsComplete();
	// Check the status of the read
	if (BEP_STATUS_SUCCESS == status)
	{	// Good read, check the number of DTCs reported
        if(GetDTCCountIndex() >= 0)
        {
            dtcCount = moduleResponse[GetDTCCountIndex()];
        }
        else
        {
            dtcCount = (moduleResponse.length() - GetFirstDTCByteIndex()) / GetBytesPerDTC();
            Log(LOG_DEV_DATA, "No DTC Count Index, calculating number of DTCs\n");
            Log(LOG_DEV_DATA, "response length: %d, FirstDTCByteIndex: %d, BytesPerDTC: %d\n",
                moduleResponse.length(), GetFirstDTCByteIndex(), GetBytesPerDTC());
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

