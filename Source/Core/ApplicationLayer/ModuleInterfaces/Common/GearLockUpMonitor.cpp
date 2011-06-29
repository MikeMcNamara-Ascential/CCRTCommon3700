//******************************************************************************
// Description:
//	Thread to monitor gears from the transmission control module.
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
//     $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/GearLockUpMonitor.cpp $
// 
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GearLockUpMonitor.h"


template <class ProtocolFilterType>
GearLockUpMonitor<ProtocolFilterType>::GearLockUpMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
														 ProtocolFilterType *filter,
														 const INT32 delay) :
BackgroundMonitor(filter, delay)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
GearLockUpMonitor<ProtocolFilterType>::~GearLockUpMonitor()
{	// Nothing special to do here
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GearLockUpMonitor<ProtocolFilterType>::MonitorData(void)
{   // Read the current transmission gear from the module
	return m_module->MonitorTransmissionData(m_vehicleComm);
}

