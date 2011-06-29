//******************************************************************************
// Description:
//	Thread to monitor shift lever positions from the transmission control module.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/ShiftLeverMonitor.cpp $
// 
// 1     11/11/05 4:50p Cward
// Added New Shift Level Monitor
//
// 1     8/24/05 9:54a Derickso
// Added for PM platform.
//
// 1     3/24/05 4:51p Canyanwu
// PT & PM Platforms
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ShiftLeverMonitor.h"
//*************************************************************************

template <class ProtocolFilterType>
ShiftLeverMonitor<ProtocolFilterType>::ShiftLeverMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
											 ProtocolFilterType *filter,
											 const INT32 delay)  :
BackgroundMonitor(filter, delay)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
ShiftLeverMonitor<ProtocolFilterType>::~ShiftLeverMonitor()
{	// Nothing special to do here
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE ShiftLeverMonitor<ProtocolFilterType>::MonitorData(void)
{	// Read the current transmission Shift Lever Position from the module
	return m_module->MonitorTransmissionShiftLever(m_vehicleComm);
}

