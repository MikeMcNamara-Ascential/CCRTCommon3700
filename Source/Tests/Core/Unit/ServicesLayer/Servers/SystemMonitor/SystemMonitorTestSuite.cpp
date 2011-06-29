//******************************************************************************
// Description:
//    System Monitor test suite.
//
//==============================================================================
//
//     Copyright (c) 2006 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemMonitor/SystemMonitorTestSuite.cpp $
// 
// 1     1/18/07 5:10p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SystemMonitorTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "SystemMonitorTest.h"

SystemMonitorTestSuite::SystemMonitorTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(SystemMonitorTest::suite());
}

