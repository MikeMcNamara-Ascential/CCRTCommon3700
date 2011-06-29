//******************************************************************************
// Description:
//	Thread to monitor Faults and Limp-In Status from the transmission control module.
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
//     $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/LimpInFaultMonitor.cpp $
// 
// 2     8/18/04 10:58p Cward
// Removed include
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "LimpInFaultMonitor.h"


template <class ProtocolFilterType>
LimpInFaultMonitor<ProtocolFilterType>::LimpInFaultMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
				   ProtocolFilterType *filter, const INT32 delay)  :
BackgroundMonitor(filter, delay)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
LimpInFaultMonitor<ProtocolFilterType>::~LimpInFaultMonitor()
{	// Nothing special to do here
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE LimpInFaultMonitor<ProtocolFilterType>::MonitorData(void)
{   // Read the limp-in status from the module
	return m_module->MonitorLimpInFaultStatus(m_vehicleComm);
}

