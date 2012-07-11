//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Common/GenericCANModuleTool.cpp 2     12/05/06 4:45p Derickso $
//
// FILE DESCRIPTION:
// Generic Daimler Chrysler module interface.
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Common/GenericCANModuleTool.cpp $
// 
// 2     12/05/06 4:45p Derickso
// Added code to the destructor to eliminate memory leaks.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     6/09/05 1:52p Derickso
// Added option to allow software to stop reading DTCs if a DTC value of 0
// is detected.
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
// 2     1/20/04 6:03a Derickso
// Corrected compile errors and typos.
// 
// 1     1/16/04 9:40a Derickso
// Generic DCX module tool interface.
// 
// 1     1/16/04 8:53a Derickso
// Generic DCX module interface.
// 
//*************************************************************************
#include "GenericCANModuleTool.h"

GenericCANModuleTool::GenericCANModuleTool(IProtocolFilter *protocolFilter) : ILogger(), 
	m_moduleConfig(NULL), m_dtcCountIndex(2), m_bytesPerDTC(3), m_firstDTCByteIndex(4)
{	// Save the pointer to the protocol filter object to use
	m_protocolFilter = protocolFilter;
}

GenericCANModuleTool::~GenericCANModuleTool()
{	// Wait until a fault read is not in progress before bailing out, check every 10ms for 500ms
	UINT16 timeRemaining = 50;
	while (FaultReadInProgress() && timeRemaining--) BposSleep(10);
    // Clean up so we do not leak memory
    m_faultByteCodes.clear(true);
    if(m_moduleConfig != NULL)  delete m_moduleConfig;
    m_moduleConfig = NULL;
}

void GenericCANModuleTool::Initialize(const XmlNode *configNode)
{	// Initialize the Logger interface
	ILogger::Initialize(configNode);
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
    Log(LOG_DEV_DATA, "Exit GenericCANModuleTool::Initialize\n");
}

inline const INT32& GenericCANModuleTool::GetDTCCountIndex(void)
{
	return m_dtcCountIndex;
}

inline const INT32& GenericCANModuleTool::GetBytesPerDTC(void)
{
	return m_bytesPerDTC;
}

inline const INT32& GenericCANModuleTool::GetFirstDTCByteIndex(void)
{
	return m_firstDTCByteIndex;
}

inline const INT16& GenericCANModuleTool::GetFirstFaultRegister(void)
{
	return m_firstFaultRegister;
}

inline const INT32& GenericCANModuleTool::GetNumberOfFaultRegisters(void)
{
	return m_numberOfFaultRegisters;
}

inline const bool& GenericCANModuleTool::FaultReadInProgress(void)
{
	return m_readingFaults;
}

inline void GenericCANModuleTool::SetDTCCountIndex(const INT32 &index)
{
	m_dtcCountIndex = index;
}

inline void GenericCANModuleTool::SetBytesPerDTC(const INT32 &bytesPerDTC)
{
	m_bytesPerDTC = bytesPerDTC;
}
inline void GenericCANModuleTool::SetFirstDTCByteIndex(const INT32 &firstDTCByteIndex)
{
	m_firstDTCByteIndex = firstDTCByteIndex;
}

inline void GenericCANModuleTool::SetFirstFaultRegister(const INT16 &firstFaultRegister)
{
	m_firstFaultRegister = firstFaultRegister;
}

inline void GenericCANModuleTool::SetNumberOfFaultRegisters(const INT32 &numberOfFaultRegisters)
{
	m_numberOfFaultRegisters = numberOfFaultRegisters;
}

void GenericCANModuleTool::SetReadFaultsInProgress(void)
{
	m_readingFaults = true;
};

void GenericCANModuleTool::SetReadFaultsComplete(void)
{
	m_readingFaults = false;
};

inline const bool& GenericCANModuleTool::GetStopNoDTCs(void)
{
	return m_stopNoDTCs;
}

inline void GenericCANModuleTool::SetStopNoDTCs(const string &stopDTCRead)
{
	if(stopDTCRead == "Yes")
       m_stopNoDTCs = true;
    else
       m_stopNoDTCs = false;
}

