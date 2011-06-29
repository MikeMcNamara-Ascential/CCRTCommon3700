//*************************************************************************
// FILE:
//    $Header: /Core/ApplicationLayer/ModuleInterfaces/Common/ABSValveControl.cpp 3     6/09/04 10:37p Cward $
//
// FILE DESCRIPTION:
//	
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
//    $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/ABSValveControl.cpp $
// 
// 3     6/09/04 10:37p Cward
// Refactored Application Layer includes, which required adding an include
// to  get access to the delay function
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     10/27/03 3:53p Derickso
// Updated to notify instantiating object when the thread completes.
//
// 1     10/06/03 9:27a Derickso
// New ABS Valve Control class
//
// 1     9/30/03 10:58a Derickso
// ABS Valve control thread.
//
//*************************************************************************
#include "ABSValveControl.h"

#include <unistd.h>	// needed for delay()


ABSValveControl::ABSValveControl(IProtocolFilter *filter,
								 const string& valveControlFireMessageTag,
								 const string& valveControlStopFireMessageTag,
								 const INT32& messageGapTime,
								 const ValveFireType valveFireType,
								 const INT32 pulseLength /* = 0 */,
								 const INT32 pulseGap /* = 0 */) :
	BackgroundMonitor(filter, messageGapTime),
	m_fireMessage(valveControlFireMessageTag),
	m_stopFireMessage(valveControlStopFireMessageTag),
	m_pulseLengthTime(pulseLength),
	m_pulseGapTime(pulseGap),
	m_controlType(valveFireType)
{   // Nothing special to do here
}

ABSValveControl::~ABSValveControl(void)
{   // Nothing special to do here
}

BEP_STATUS_TYPE ABSValveControl::MonitorData(void)
{	// Start firing the valves
	SerialString_t moduleResponse;
	BEP_STATUS_TYPE moduleStatus = m_vehicleComm->GetModuleData(GetFireMessageTag(), moduleResponse);
	// Check the status
	if (BEP_STATUS_SUCCESS == moduleStatus)
	{   // Good comms, determine the pulse type
		if (GetValveControlType() == PulseBrake)
		{   // Pulse type. Wait for the required time before stopping the valve fire
			delay((UINT32)GetPulseLengthTime());
			// Stop firing the valves
			moduleStatus = m_vehicleComm->GetModuleData(GetStopFireMessageTag(), moduleResponse);
			// Wait befor pulsing the brake again so vehicle does not climb out of rolls
			delay((UINT32)GetPulseGapTime());
		}
	}
	// Return the status of the operation
	return moduleStatus;
}

inline const string& ABSValveControl::GetFireMessageTag(void)
{   // Return the message tag
	return m_fireMessage;
}

inline const string& ABSValveControl::GetStopFireMessageTag(void)
{   // Return the stop fire message
	return m_stopFireMessage;
}

inline const INT32& ABSValveControl::GetPulseLengthTime(void)
{   // Return the pulse time length
	return m_pulseLengthTime;
}

inline const INT32& ABSValveControl::GetPulseGapTime(void)
{   // Return the pulse time gap
	return m_pulseGapTime;
}

inline const ValveFireType& ABSValveControl::GetValveControlType(void)
{   // Return the valve control type
	return m_controlType;
}

