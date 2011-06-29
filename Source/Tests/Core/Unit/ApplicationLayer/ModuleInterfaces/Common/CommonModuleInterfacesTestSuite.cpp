//******************************************************************************
// Description:
//   This is the test suite for the Common Module Interface tests.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/CommonModuleInterfacesTestSuite.cpp $
// 
// 3     1/18/07 3:30p Rwiersem
// Changes for the 07011901 core release:
//                                                                
// - Added GenericModuleTemplateTest.
// 
// 2     10/26/06 1:39p Rwiersem
// Changes for the 06102701 core release:
// 
// Added BackgroundMonitor test suite.
// 
// Added SendModuleKeepAlive test suite.
// 
// 1     9/22/06 11:26a Rwiersem
// Changes for the 06092201 core release:
//     - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "CommonModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "BackgroundMonitorTest.h"
#include "ReplyInterpreterTest.h"
#include "SendModuleKeepAliveTest.h"
#include "GenericModuleTemplateTest.h"

CommonModuleInterfacesTestSuite::CommonModuleInterfacesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(BackgroundMonitorTest::suite());
    addTest(ReplyInterpreterTest::suite());
    addTest(SendModuleKeepAliveTest::suite());
    addTest(GenericModuleTemplateTest::suite());
}


