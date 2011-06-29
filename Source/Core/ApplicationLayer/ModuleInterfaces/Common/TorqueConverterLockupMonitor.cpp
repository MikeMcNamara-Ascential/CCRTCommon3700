//******************************************************************************
// Description:
//	Thread to monitor torque converter clutch lockup status from the
//  transmission control module.
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
//     $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/TorqueConverterLockupMonitor.cpp $
// 
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TorqueConverterLockupMonitor.h"

/**
 * Background monitor to look for the various torque converter clutch
 * lockup states.  Once all three states are seen, this thread will
 * stop running.  All results are stored in the module interface that
 * instantiated this thread.
 *
 * @author Dan Erickson
 * @version Version 1.0
 * @see AtomicThread
 * @see GenericTransmissionModuleTemplate
 * @since May 22, 2003
 */

template <class ProtocolFilterType>
TorqueConverterLockupMonitor<ProtocolFilterType>::TorqueConverterLockupMonitor(GenericTransmissionModuleTemplate<ProtocolFilterType> *module,
							 ProtocolFilterType *filter, const INT32 delay) :
BackgroundMonitor(filter, delay)
{	// Save the pointer to the module
	m_module = module;
}

template <class ProtocolFilterType>
TorqueConverterLockupMonitor<ProtocolFilterType>::~TorqueConverterLockupMonitor()
{	// Nothing special to do here
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE TorqueConverterLockupMonitor<ProtocolFilterType>::MonitorData(void)
{   // Read the torque converter lockup status from the module
	return m_module->MonitorTorqueConverterLockupStatus(m_vehicleComm);
}



