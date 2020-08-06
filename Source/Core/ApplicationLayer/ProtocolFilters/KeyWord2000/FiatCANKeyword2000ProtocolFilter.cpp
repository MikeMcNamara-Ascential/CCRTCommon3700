//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/KeyWord2000/DcxCANKeyword2000ProtocolFilter.cpp 3     12/06/06 1:29p Derickso $
//
// FILE DESCRIPTION:
//	Protocol Filter for handling Keyword 2000 Protocol as implemented by DaimlerChrysler
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ProtocolFilters/KeyWord2000/DcxCANKeyword2000ProtocolFilter.cpp $
// 
// 3     12/06/06 1:29p Derickso
// Moved typedef to base class and added SetLastTxTime to Send function.
// 
// 2     11/14/06 3:38p Bbarrett
// Latest core components do not die.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 1     12/01/04 2:36p Derickso
// Created new VSS Database.
// 
// 1     6/09/04 8:45a Derickso
// Dcx specific protocol filter for Keyword protocol 2000 on CAN bus.
// 
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 8     1/20/04 6:43a Derickso
// Updated Semaphore to BepSemaphore.
// 
// 7     12/16/03 12:42p Derickso
// Added handling for module busy negative response code.
// 
// 5     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
// 
// 4     11/13/03 7:02p Derickso
// Updates to correct for response pending handling errors.
// 
// 3     10/20/03 9:40a Derickso
// Added accessor and settor methods for data byte index.
// Added loggin level masks to Log statements.
// 
// 2     9/22/03 5:30p Derickso
// Added SensMessage methods to correct comile errors.
// Updated to add module request ID to each message prior
// to sending the message.
// 
// 1     8/25/03 1:52p Derickso
// Created.  Compiles, not tested.
// 
//*************************************************************************
#include "FiatCANKeyword2000ProtocolFilter.h"

FiatCANKeyword2000ProtocolFilter::FiatCANKeyword2000ProtocolFilter(KeepAliveTimer_t &lastTxTime, 
                                                                   StopCommsBepCondVar *stopCommsBepCondVar, 
                                                                   BepMutex *commPortInUse /* =NULL*/) : 
DcxCANKeyword2000ProtocolFilter(lastTxTime, stopCommsBepCondVar, commPortInUse)
{   // Nothing special to do here
}

FiatCANKeyword2000ProtocolFilter::~FiatCANKeyword2000ProtocolFilter()
{	// Nothing special to do here.
	Log(LOG_FN_ENTRY, "~FiatCANKeyword2000ProtocolFilter() complete\n");
}


inline void FiatCANKeyword2000ProtocolFilter::SetModuleRequestID(string moduleID)
{                  
    UINT16 idh = atoh(moduleID.substr(2,4).c_str());
    UINT16 idl = atoh(moduleID.substr(6,4).c_str());
    m_moduleRequestID.clear();
	m_moduleRequestID.push_back((uint8_t)((idh & 0xFF00) >> 8));
	m_moduleRequestID.push_back((uint8_t)((idh & 0x00FF)));
	m_moduleRequestID.push_back((uint8_t)((idl & 0xFF00) >> 8));
	m_moduleRequestID.push_back((uint8_t)(idl & 0x00FF));
}

