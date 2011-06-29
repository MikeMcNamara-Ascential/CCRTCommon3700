//******************************************************************************
// Description:
//       Runs all DCX specific common module interface tests
//
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/CommonDCXModuleInterfacesTestSuite.cpp $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "CommonDCXModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "GenericDCXCANModuleTest.h"
#include "GenericDCXSCIModuleTest.h"

CommonDCXModuleInterfacesTestSuite::CommonDCXModuleInterfacesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(GenericDCXCANModuleTest::suite());
    addTest(GenericDCXSCIModuleTest::suite());
}


