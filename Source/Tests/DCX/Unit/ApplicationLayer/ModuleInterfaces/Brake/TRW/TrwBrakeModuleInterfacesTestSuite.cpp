//******************************************************************************
// Description:
//   This is the test suite for the Brake Module Interface tests.
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
//     $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/TRW/TrwBrakeModuleInterfacesTestSuite.cpp $
// 
// 1     9/07/07 10:08a Dkrause
// 
// 1     10/26/06 1:35p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TrwBrakeModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "GenericTrwABSModuleTemplateTest.h"

TrwBrakeModuleInterfacesTestSuite::TrwBrakeModuleInterfacesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(GenericTrwABSModuleTemplateTest::suite());
}


