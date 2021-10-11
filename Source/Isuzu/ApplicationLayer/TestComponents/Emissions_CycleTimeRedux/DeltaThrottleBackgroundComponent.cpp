//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/Power/Battery/ThrottleBackgroundComponent.cpp $
//
// FILE DESCRIPTION:
//	High Voltage Battery Test Component Background Thread to allow component to run in background.
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
//*************************************************************************
#include "DeltaThrottleBackgroundComponent.h"

template <class ModuleType>
DeltaThrottleBackgroundComponent<ModuleType>::DeltaThrottleBackgroundComponent(IsuzuDeltaEmissionsTc<ModuleType> *testComponent) :
BackgroundTestComponent()
{   // Save the pointer to the test component
	m_testComponent = testComponent;
}

template<class ModuleType>
DeltaThrottleBackgroundComponent<ModuleType>::~DeltaThrottleBackgroundComponent()
{   // Nothing special to do here
}
	
template<class ModuleType>
BEP_STATUS_TYPE DeltaThrottleBackgroundComponent<ModuleType>::RunBackgroundComponent()
{
	m_testComponent->Log(LOG_FN_ENTRY, "ThrottleBackgroundComponent - RunBackgroundComponent() Enter\n");
	m_testComponent->Log(LOG_DEV_DATA, "Reading throttle position\n");

    m_testComponent->ThrottlePositionMonitor();

    m_testComponent->Log(LOG_DEV_DATA, "Setting Complete...\n");

    m_testComponent->Log(LOG_FN_ENTRY, "ThrottleBackgroundComponent - RunBackgroundComponent() Exit\n");
	return BEP_STATUS_SUCCESS;
}

template<class ModuleType>
INT32 DeltaThrottleBackgroundComponent<ModuleType>::WaitUntilDone(void)
{
	m_testComponent->Log(LOG_FN_ENTRY, "ThrottleBackgroundComponent - WaitUntilDone, Calling Base class to join thread\n");
	return(BackgroundTestComponent::WaitUntilDone());
}
