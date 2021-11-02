//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/Power/Battery/O2SBackgroundComponent.cpp $
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
#include "O2SBackgroundComponent.h"

template <class ModuleType>
O2SBackgroundComponent<ModuleType>::O2SBackgroundComponent(IsuzuEmissionsTc<ModuleType> *testComponent) :
BackgroundTestComponent()
{   // Save the pointer to the test component
	m_testComponent = testComponent;
}

template<class ModuleType>
O2SBackgroundComponent<ModuleType>::~O2SBackgroundComponent()
{   // Nothing special to do here
}
	
template<class ModuleType>
BEP_STATUS_TYPE O2SBackgroundComponent<ModuleType>::RunBackgroundComponent()
{
	m_testComponent->Log(LOG_FN_ENTRY, "O2SBackgroundComponent - RunBackgroundComponent() Enter\n");
	m_testComponent->Log(LOG_DEV_DATA, "Reading oxygen sensor data while at idle RPM...\n");

    m_testComponent->OxygenSensorMonitor();

    m_testComponent->Log(LOG_DEV_DATA, "Setting Complete...\n");

    m_testComponent->SetBGTestComplete();

    m_testComponent->Log(LOG_FN_ENTRY, "O2SBackgroundComponent - RunBackgroundComponent() Exit\n");
	return BEP_STATUS_SUCCESS;
}

template<class ModuleType>
INT32 O2SBackgroundComponent<ModuleType>::WaitUntilDone(void)
{
	m_testComponent->Log(LOG_FN_ENTRY, "O2SBackgroundComponent - WaitUntilDone, Calling Base class to join thread\n");
	return(BackgroundTestComponent::WaitUntilDone());
}
