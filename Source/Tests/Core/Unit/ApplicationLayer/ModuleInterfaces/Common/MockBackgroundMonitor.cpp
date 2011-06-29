//******************************************************************************
// Description:
//  The mock BackgroundMonitor class.  This is needed because MonitorData() is
//  pure virtual in the BackgroundMonitor class.
//
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/MockBackgroundMonitor.cpp $
// 
// 1     10/26/06 1:40p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockBackgroundMonitor.h"

MockBackgroundMonitor::MockBackgroundMonitor(IProtocolFilter *vehicleComm,
                                             const INT32 interMessageGap) :
    BackgroundMonitor(vehicleComm,interMessageGap)
{

}

MockBackgroundMonitor::~MockBackgroundMonitor(void)
{

}

BEP_STATUS_TYPE MockBackgroundMonitor::MonitorData(void)
{
    return(BEP_STATUS_SUCCESS);
}
