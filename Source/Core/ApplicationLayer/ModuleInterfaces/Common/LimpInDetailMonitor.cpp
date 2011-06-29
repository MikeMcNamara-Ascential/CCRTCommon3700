//******************************************************************************
// Description:
//	Thread to monitor Details and Limp-In Status from the transmission control module.
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
//     $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/LimpInDetailMonitor.cpp $
// 
// 1     10/30/07 1:24p Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
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

#include "LimpInDetailMonitor.h"


template <class ProtocolFilterType>
LimpInDetailMonitor<ProtocolFilterType>::LimpInDetailMonitor(DcxAhstTransmissionModule<ProtocolFilterType> *module,
														   ProtocolFilterType *filter, const INT32 delay)  :
BackgroundMonitor(filter, delay)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
LimpInDetailMonitor<ProtocolFilterType>::~LimpInDetailMonitor()
{	// Nothing special to do here
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE LimpInDetailMonitor<ProtocolFilterType>::MonitorData(void)
{	// Read the limp-in status from the module
	return m_module->MonitorLimpInDetailStatus(m_vehicleComm);
}

