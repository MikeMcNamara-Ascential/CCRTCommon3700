//*************************************************************************
// FILE:
//    $Header: /Core/ApplicationLayer/ModuleInterfaces/Common/SwitchMonitor.cpp 3     1/23/04 9:24a Cward $
//
// FILE DESCRIPTION:
//	Switch monitor thread.
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
//    $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/SwitchMonitor.cpp $
// 
// 3     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 3     11/29/03 1:26p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 2     10/20/03 9:31a Derickso
// Added logic to prevent thread from being deleted while the
// communication port is locked.
// Added a flag that the instantiateing object can use to determine if the
// thread has completed and exited.  These threads typically self-delete
// when commanded to exit.
//
//*************************************************************************
#include "SwitchMonitor.h"
#include "GeneralUtilities.h"
//--------------------------------------------------------------------------

SwitchMonitor::SwitchMonitor(IGenericModule *module, IProtocolFilter *filter, const INT32 delay) :
BackgroundMonitor(filter, delay)
{   // Save the pointer to the module
	m_module = module;
}

SwitchMonitor::~SwitchMonitor()
{   // Nothing special to do here
}

inline BEP_STATUS_TYPE SwitchMonitor::MonitorData()
{
	return m_module->ReadSwitches(m_vehicleComm);
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */

