//******************************************************************************
// Description:
//   This is the test suite for the Module Interface tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/ModuleInterfacesTestSuite.cpp $
// 
// 4     1/18/07 3:21p Rwiersem
// Changes for the 07011901 core release:
//                                                            
// - Added Transmission test suite.
// 
// 3     10/26/06 1:34p Rwiersem
// Changes for the 06102701 core release:
// 
// Added Brake test suite.
// 
// 2     9/22/06 11:24a Rwiersem
// Changes for the 06092201 core release:
//     - Added CommonModuleInterfacesTestSuite
// 
// 1     9/06/06 10:09a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Brake/BrakeModuleInterfacesTestSuite.h"
#include "Common/CommonModuleInterfacesTestSuite.h"
#include "EngineController/EngineControllerTestSuite.h"
#include "Transmission/TransmissionModuleInterfacesTestSuite.h"

ModuleInterfacesTestSuite::ModuleInterfacesTestSuite()
{
    addTest(new BrakeModuleInterfacesTestSuite());
    addTest(new CommonModuleInterfacesTestSuite());
    addTest(new EngineControllerTestSuite());
    addTest(new TransmissionModuleInterfacesTestSuite());
}


