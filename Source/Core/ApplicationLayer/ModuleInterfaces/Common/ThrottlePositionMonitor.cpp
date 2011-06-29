//******************************************************************************
// Description:
//	Thread to monitor throttle position sensor.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/ThrottlePositionMonitor.cpp $
// 
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "ThrottlePositionMonitor.h"


template <class ProtocolFilterType>
ThrottlePositionMonitor<ProtocolFilterType>::ThrottlePositionMonitor(GenericEmissionsModuleTemplate<ProtocolFilterType> *module,
						ProtocolFilterType *filter, const string &messageTag,
						const INT32 delay) :
BackgroundMonitor(filter, delay), m_messageTag(messageTag)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
ThrottlePositionMonitor<ProtocolFilterType>::~ThrottlePositionMonitor()
{	// Nothing special to do here
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE ThrottlePositionMonitor<ProtocolFilterType>::MonitorData(void)
{   // Read the throttle position from the module
	return m_module->ReadThrottlePosition(m_vehicleComm, m_messageTag);
}

